/**
 * \file	rawProtocol.c
 * \brief	functions for write and read data packets with a raw protocol
 *
 * \author	Aaron Montalvo, <aaron.montalvo@uah.es>
 *
 * \internal
 * Created:		10/03/2021 at 15:39:42
 * Company:		Space Research Group, Universidad de Alcalï¿½.
 * Copyright:	Copyright (c) 2021, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include "configSerial.h"			/* comports */
#include "configTCPSocket.h"			/* CloseSocket */
#if(defined _WIN32 || __CYGWIN__)
#include <windows.h>				/* Sleep */
#elif(defined __linux__)
#include <unistd.h>                             /* usleep */
#include <string.h>                             /* memcpy */
#include <sys/ioctl.h>				
#include <errno.h>                              /* errno */
#include <termios.h>                            /* tcflush */
#endif
#include <stdio.h>				/* sprintf */
#include <math.h>				/* pow */
#include <time.h>				/* clock_gettime */
#include "../definitions.h"                     /* MAX_INTERFACES, portConfig */
#include "../CommonTools/GetSetFieldTools.h"    /* GetFieldAsUint */

#define DELAY_MS					20

pthread_mutex_t ptclMutex[MAX_INTERFACES];
int dbgReceivedPackets = 0;

static unsigned int TryGetPacketHeaderLenBytes(const unsigned char * buffer,
	unsigned int initialPointer, unsigned int bufferMaxSize,
        const protocolConfig * cpPtcl);
static int readData(portConfig * pPort, unsigned char * pBufferData,
        unsigned int bufferOffset, unsigned int dataLen, unsigned int *pRxLen);
static void msleep(unsigned long msec);

int ReadRawProtocol(unsigned char * packet, unsigned int * pLength,
        protocolConfig * pPtcl)
{
    unsigned int length = 0, availableLen = 0;
    unsigned int idx = 0, headIdx = 0;
    
    /* wait for packet header length (sync + packetLen) or port closed */
    do {
        availableLen = (PTCL_BUFFER_SIZE+pPtcl->buffer.tail - pPtcl->buffer.head)%PTCL_BUFFER_SIZE;
        msleep(DELAY_MS);
    }while((availableLen < pPtcl->buffer.minLenBytes) && (pPtcl->portValid == 1)
                && (pPtcl->portConnected == 1) && (pPtcl->buffer.rxErrors == 0));
    if(pPtcl->buffer.rxErrors != 0)
        return -pPtcl->buffer.rxErrors;
    else if((pPtcl->portValid == 0) || (pPtcl->portConnected == 0))
        return 0;
    
    /* check packet sync */
    if(pPtcl->syncPatternLength != 0)
    {
        headIdx = pPtcl->buffer.head;
        idx = 0;
        while(idx<pPtcl->syncPatternLength)
        {
            /* copy for later index */
            packet[idx] = pPtcl->buffer.data[headIdx];
            if(pPtcl->buffer.data[headIdx] != pPtcl->syncPattern[idx])
                break;
            idx++;
            headIdx = (headIdx+1)%PTCL_BUFFER_SIZE;
        }
        if(idx != pPtcl->syncPatternLength)
        {
            /* no packet sync pattern found, move head pointer and return */
            pPtcl->buffer.head = (pPtcl->buffer.head+idx+1)%PTCL_BUFFER_SIZE;
            *pLength = idx+1;
            return idx+1;
        }
    }
    
    /* get packet header */
    length = TryGetPacketHeaderLenBytes(pPtcl->buffer.data, pPtcl->buffer.head,
            PTCL_BUFFER_SIZE, pPtcl);
    
    /* wait for full packet length or port closed */
    do {
        availableLen = (PTCL_BUFFER_SIZE+pPtcl->buffer.tail - pPtcl->buffer.head)%PTCL_BUFFER_SIZE;
        msleep(DELAY_MS);
    }while((availableLen < length) && (pPtcl->portValid == 1)
            && (pPtcl->portConnected == 1) && (pPtcl->buffer.rxErrors == 0));
    if(pPtcl->buffer.rxErrors != 0)
        return -pPtcl->buffer.rxErrors;
    else if((pPtcl->portValid == 0) || (pPtcl->portConnected == 0))
        return 0;
    
    /* get full packet byte by byte */
    headIdx = pPtcl->buffer.head;
    idx = 0;
    while((idx<(*pLength)) && (idx<length))
    {
        packet[idx] = pPtcl->buffer.data[headIdx];
        idx++;
        headIdx = (headIdx+1)%PTCL_BUFFER_SIZE;
    }
    /* if length is bigger than allowed for GSS, 
     * discard last bytes, moving on head pointer like in every packet,
     * and let upper process deal with it */
    pPtcl->buffer.head = (pPtcl->buffer.head+length)%PTCL_BUFFER_SIZE;
    *pLength = length;
    dbgReceivedPackets++;
    return length;
}

int readProtocolPacketsBuffer(portConfig * ports)
{
    int status = 0, currentHWLen = 0;
    unsigned int portIdx = 0, readLen = 0;
    unsigned int protocolPorts = 0, protocolPortsNotValid = 0;
    struct timespec markIni, markEnd;
    double elapsedMs = 0.0, intervalMs = DELAY_MS;
#if(defined _WIN32 || __CYGWIN__)
    DWORD comErrors;
    COMSTAT comStatus;
#endif
    
    for(portIdx=0; portIdx<MAX_INTERFACES; ++portIdx)
    {
        if((ports[portIdx].portType != UART_PORT) &&
                (ports[portIdx].portType != TCP_SOCKET_SRV_PORT) &&
                (ports[portIdx].portType != TCP_SOCKET_CLI_PORT))
            continue;
        ports[portIdx].ptcl.buffer.head = 0;
        ports[portIdx].ptcl.buffer.tail = 0;
        ports[portIdx].ptcl.buffer.minLenBytes =
                (ports[portIdx].ptcl.sizeFieldOffsetInBits + ports[portIdx].ptcl.sizeFieldTrimInBits) / 8;
        if((ports[portIdx].ptcl.sizeFieldOffsetInBits + ports[portIdx].ptcl.sizeFieldTrimInBits) % 8)
                ports[portIdx].ptcl.buffer.minLenBytes++;
        protocolPorts++;
    }

    while(1)
    {
        clock_gettime(CLOCK_MONOTONIC, &markIni);
        for(portIdx=0; portIdx<MAX_INTERFACES; ++portIdx)
        {                    
            if((ports[portIdx].portType != UART_PORT) &&
                    (ports[portIdx].portType != TCP_SOCKET_SRV_PORT) &&
                    (ports[portIdx].portType != TCP_SOCKET_CLI_PORT))
                continue;

            /* call mutex for avoiding port closed when reading */
            pthread_mutex_lock(&ptclMutex[portIdx]);
            
            if(((ports[portIdx].portType == TCP_SOCKET_SRV_PORT) ||
                    (ports[portIdx].portType == TCP_SOCKET_CLI_PORT))
                    && (ports[portIdx].ptcl.portConnected == 1)
                    && (ports[portIdx].config.socket.socketHdl != -1))
            {
                char test;
                int status = 0;
                if((status = recv(ports[portIdx].config.socket.socketHdl, &test, 1, MSG_PEEK)) <= 0)
                {
                    /* if read == 0 (MSG_PEEK means don't move pointer) socket closed gratefully */
#if(defined _WIN32 || __CYGWIN__)
                    /* windows can detect also connection resets */
                    status = WSAGetLastError();
                    if(status == WSAEWOULDBLOCK) //connection reset by peer
                        status = 1;
                    else if(status == WSAECONNRESET)//no data at non-blocking operation
                        status = 0;
                    else
                        ports[portIdx].ptcl.buffer.rxErrors = status;
#elif(defined __linux__)
                    if(status == -1)
                    {
                        status = errno;
                        if((status == EAGAIN) || (status == EWOULDBLOCK))//no data at non-blocking operation
                            status = 1;
                        else
                            ports[portIdx].ptcl.buffer.rxErrors = errno;
                    }
#endif
                    if(status == 0)
                    {
                        ports[portIdx].ptcl.buffer.head = 0;
                        ports[portIdx].ptcl.buffer.tail = 0;
                        ports[portIdx].ptcl.portConnected = 0;
                        CloseTCPSocket(&ports[portIdx].config.socket.socketHdl);
                    }
                    pthread_mutex_unlock(&ptclMutex[portIdx]);
                    continue;
                }
            }
            
            /* check if port is still valid again (not closed) */
            if(ports[portIdx].ptcl.portValid == 0)
            {
                pthread_mutex_unlock(&ptclMutex[portIdx]);
                protocolPortsNotValid++;
                /* stop polling loop if all ports have been closed (1/2) */
                if(protocolPorts == protocolPortsNotValid)
                    break;
                else
                    continue;
            }
            if(ports[portIdx].ptcl.portConnected == 0)
            {
                pthread_mutex_unlock(&ptclMutex[portIdx]);
                continue;
            }
            
            /* clear errors from port (serial, win) and
             * get current data length to be read */
            if(ports[portIdx].portType == UART_PORT)
            {
#if(defined _WIN32 || __CYGWIN__)
                comErrors = 0;
                ClearCommError(comports[ports[portIdx].config.uart.portNum],
                        &comErrors, &comStatus);
                if(comErrors != 0)
                {
                    ports[portIdx].ptcl.buffer.rxErrors = GetLastError();
                    pthread_mutex_unlock(&ptclMutex[portIdx]);
                    continue;
                }
                currentHWLen = comStatus.cbInQue;
#elif(defined __linux__)
                ioctl(ttyports[ports[portIdx].config.uart.portNum], FIONREAD, &currentHWLen);
#endif
            }
            else
            {
#if(defined _WIN32 || __CYGWIN__)
                ioctlsocket(ports[portIdx].config.socket.socketHdl, FIONREAD, (u_long*)&currentHWLen);
#elif(defined __linux__)
                ioctl(ports[portIdx].config.socket.socketHdl, FIONREAD, &currentHWLen);
#endif
            }
            status = 0;
            if(currentHWLen != 0)
            {
                status = readData(&ports[portIdx], ports[portIdx].ptcl.buffer.data,
                            ports[portIdx].ptcl.buffer.tail, currentHWLen, &readLen);
                ports[portIdx].ptcl.buffer.tail =
                        (ports[portIdx].ptcl.buffer.tail+currentHWLen)%PTCL_BUFFER_SIZE;
                if(status != 0)
                {
                    ports[portIdx].ptcl.buffer.rxErrors = status;
                }
            }
            pthread_mutex_unlock(&ptclMutex[portIdx]);
        }
        
        /* stop polling if all ports closed (2/2) */
        if(protocolPorts == protocolPortsNotValid)
        {
            break;
        }
        clock_gettime(CLOCK_MONOTONIC, &markEnd);
        elapsedMs = (markEnd.tv_sec - markIni.tv_sec) * 1000.0;
        elapsedMs += (markEnd.tv_nsec - markIni.tv_nsec) / 1000000.0;
        if(elapsedMs < intervalMs)
            msleep(intervalMs - elapsedMs);       
    }
    for(portIdx=0; portIdx<MAX_INTERFACES; ++portIdx)
    {
        if(ports[portIdx].portType != UART_PORT)
            continue;
        pthread_mutex_destroy(&ptclMutex[portIdx]);
    }
    return status;
}

static unsigned int TryGetPacketHeaderLenBytes(const unsigned char * buffer,
	unsigned int initialPointer, unsigned int bufferMaxSize,
	const protocolConfig * cpPtcl)
{
    unsigned int aux1 = 0, aux2 = 0, lengthInBytes = 0;
    GetFieldFromBufferAsUint(buffer, initialPointer, bufferMaxSize,
            cpPtcl->sizeFieldOffsetInBits, cpPtcl->sizeFieldTrimInBits, &aux1);

    switch(cpPtcl->refPower)
    {
        case PTCL_POWER_BASE_2:
            if(aux1 > 1024)
            {
                /* can not power up to 2^1024) */
                return 0;
            }
            aux2 = (unsigned int) pow(2, aux1);
            break;

        case PTCL_POWER_BASE_2_WITH_0:
            if(aux1 == 0)
                aux2 = 0;
            else
            {
                if(aux1 > 1024)
                {
                    /* can not power up to 2^1024) */
                    return 0;
                }
                aux2 = (unsigned int) pow(2, aux1);
            }
            break;

        case PTCL_POWER_NONE: default:
            aux2 = aux1;
            break;
    }
    lengthInBytes = ((aux2 * cpPtcl->refUnit) + cpPtcl->constSizeInBits) / 8;
    return lengthInBytes;
}

static void msleep(unsigned long msec)
{
#if(defined _WIN32 || __CYGWIN__)
    Sleep(msec);
#elif(defined __linux__)
    usleep(msec * 1000);
#endif
}

static int readData(portConfig * pPort, unsigned char * pBufferData,
        unsigned int bufferOffset, unsigned int dataLen, unsigned int *pRxLen)
{
#if(defined _WIN32 || __CYGWIN__)
    BOOL res;
#endif
    int status = 0;
    unsigned int partialLen = 0;
    
    /* read data from port */
    if((bufferOffset + dataLen) > PTCL_BUFFER_SIZE)
    {
        /* data has to be read in two parts */
        /* 1- from currentPos to end of buffer */
        if(pPort->portType == UART_PORT)
        {
#if(defined _WIN32 || __CYGWIN__)
            res = ReadFile(comports[pPort->config.uart.portNum],
                    &pBufferData[bufferOffset], PTCL_BUFFER_SIZE-bufferOffset,
                    (LPDWORD)&partialLen, NULL);
            if(res == FALSE)
                status = GetLastError();
            else
                status = 0;
#elif(defined __linux__)
            partialLen = read(ttyports[pPort->config.uart.portNum],
                    &pBufferData[bufferOffset], PTCL_BUFFER_SIZE-bufferOffset);
            if(status < 0)
                status = errno;
            else
                status = 0;
#endif
        }
        else if((pPort->portType == TCP_SOCKET_SRV_PORT) ||
                (pPort->portType == TCP_SOCKET_CLI_PORT))
        {
            status = recv(pPort->config.socket.socketHdl,
                    (char*)&pBufferData[bufferOffset], PTCL_BUFFER_SIZE-bufferOffset, 0);
            if(status >= 0)
            {
                partialLen = status;
                status = 0;
            }
        }
        if(status != 0)
        {
            return status;
        }

        /* 2- from beginning of buffer to remaining size */
        if(pPort->portType == UART_PORT)
        {
#if(defined _WIN32 || __CYGWIN__)
            res = ReadFile(comports[pPort->config.uart.portNum],
                    &pBufferData[0], dataLen-(PTCL_BUFFER_SIZE-bufferOffset),
                    (LPDWORD)pRxLen, NULL);
            if(res == FALSE)
                status = GetLastError();
            else
                status = 0;
#elif(defined __linux__)
            *pRxLen = read(ttyports[pPort->config.uart.portNum],
                    &pBufferData[0], dataLen-(PTCL_BUFFER_SIZE-bufferOffset));
            if(status < 0)
                status = errno;
            else
                status = 0;
#endif
        }
        else if((pPort->portType == TCP_SOCKET_SRV_PORT) ||
                (pPort->portType == TCP_SOCKET_CLI_PORT))
        {
            status = recv(pPort->config.socket.socketHdl,
                    (char*)&pBufferData[0], dataLen-(PTCL_BUFFER_SIZE-bufferOffset), 0);
            if(status >= 0)
            {
                *pRxLen = status;
                status = 0;
            }
        }
        *pRxLen = *pRxLen+partialLen;
    }
    else
    {
        /* data is read all at together */
        if(pPort->portType == UART_PORT)
        {
#if(defined _WIN32 || __CYGWIN__)
            res = ReadFile(comports[pPort->config.uart.portNum],
                    &pBufferData[bufferOffset], dataLen,
                    (LPDWORD)pRxLen, NULL);
            if(res == FALSE)
                status = GetLastError();
            else
                status = 0;
#elif(defined __linux__)
            *pRxLen = read(ttyports[pPort->config.uart.portNum],
                    &pBufferData[bufferOffset], dataLen);
            if(status < 0)
                status = errno;
            else
                status = 0;
#endif
        }
        else if((pPort->portType == TCP_SOCKET_SRV_PORT) ||
                (pPort->portType == TCP_SOCKET_CLI_PORT))
        {
            status = recv(pPort->config.socket.socketHdl,
                    (char*)&pBufferData[bufferOffset], dataLen, 0);
            if(status >= 0)
            {
                *pRxLen = status;
                status = 0;
            }
        }
    }
    return status;
}