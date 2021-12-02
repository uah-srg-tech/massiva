/**
 * \file	raw.c
 * \brief	functions for write and read data with interfaces (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		22/03/2012 at 16:06:42
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdio.h>			/* snprintf */
#include <string.h>			/* memcpy */
#include "rawSpWmk2.h"			/* ReadRawSpWmk2, WriteRawSpWmk2, ... */
#include "rawSpWusb.h"			/* ReadRawSpWusb, WriteRawSpWusb, ... */
#include "rawSpWpci.h"			/* ReadRawSpWpci, WriteRawSpWpci, ... */
#include "rawSerial.h"			/* ReadRawSerial, WriteRawSerial, ... */
#include "rawSocket.h"			/* ReadRawSocket, WriteRawSocket, ... */
#include "raw.h"			/* SingleTickIn */
#include "rawProtocol.h"		/* readProtocolPacketsBuffer */
#include "configSpW.h"			/* timeout */
#include "configSocket.h"			/* timeout */
#include "../definitions.h"		/* MAX_PACKET_SIZE */
#include "../CommonTools/GetSetFieldTools.h"

enum {
    WRONG_SPW_LOGIC_PORT = -101,
    WRONG_LENGTH = -102,
    WRONG_PACKET_SYNC = -103
};

static int wrongValue[MAX_INTERFACES];

int ReadRaw(unsigned char * packet, unsigned int * pLength, portConfig * pPort)
{
    int status = 0;
    *pLength = MAX_PACKET_SIZE + MAX_PHY_HDR;

    switch(pPort->portType)
    {
        case SPW_USB_MK2_PORT:
            status = ReadRawSpWmk2(packet, pLength, &pPort->config.spw.channel);
            break;

        case SPW_USB_PORT:
            status = ReadRawSpWusb(packet,(int *)pLength, pPort->config.spw.ptr);
            break;

        case SPW_PCI_PORT:
            status = ReadRawSpWpci(packet, (int *)pLength, pPort->config.spw.ptr,
                    pPort->config.spw.link);
            break;

        case UART_PORT:
            status = ReadRawSerial(packet, pLength, &pPort->ptcl);
            break;
            
        case SOCKET_SRV_PORT: case SOCKET_CLI_PORT:
            status = ReadRawSocket(packet, pLength,
                    &pPort->config.socket.socketHdl, &pPort->ptcl);
            break;

        default:
            break;
    }
    /* if device closed status = 0 */
    return status;
}

int WriteRaw(const unsigned char * packet, int length, portConfig * pPort)
{
    int status = 0;

    switch(pPort->portType)
    {
        case SPW_USB_MK2_PORT:
            status = WriteRawSpWmk2(packet, length, &pPort->config.spw.channel);
            break;

        case SPW_USB_PORT:
            status = WriteRawSpWusb(packet, length, pPort->config.spw.ptr);
            break;

        case SPW_PCI_PORT:
            status = WriteRawSpWpci(packet, length, pPort->config.spw.ptr,
                    pPort->config.spw.link);
            break;

        case UART_PORT:
            status = WriteRawSerial(packet, length, pPort->config.uart.portNum);
            break;
            
        case SOCKET_SRV_PORT: case SOCKET_CLI_PORT:
            status = WriteRawSocket(packet, length, pPort->config.socket.socketHdl);
            break;

        case SPW_TC_PORT:
            status = 0;
            /* periodical tcs, 0xFE enable, 0xFF disable */
            if((packet[0] == 0xFE) || (packet[0] == 0xFF))
            {
                status = PeriodicalTickIns(pPort, 0xFF - packet[0], packet[1]);
            }
            else
            {
                /* single tc */
                status = SingleTickIn(pPort, packet[0], 1);
            }
            break;

        case SPW_ERROR_PORT:
            switch(pPort->portTypeAux)
            {
                case SPW_USB_MK2_PORT:
                    status = InjectErrorSpWMk2(&pPort->config.spw.deviceId,
                            pPort->config.spw.link, packet[0]);
                    break;

                case SPW_USB_PORT: case SPW_PCI_PORT: default:
                    break;
            }
            break;

        case DUMMY_PORT: case NO_PORT:
            status = length;
            break;

        default:
            break;
    }
    return status;
}

void RawRWError(int status, char * msg, int msgSize, portConfig * pPort)
{
    switch(pPort->portType)
    {
        case SPW_USB_MK2_PORT:
            RawSpWmk2RWError(status, msg, msgSize, &pPort->config.spw.deviceId);
            break;

        case SPW_USB_PORT:
            RawSpWusbRWError(status, msg, msgSize);
            break;

        case SPW_PCI_PORT:
            RawSpWpciRWError(status, msg, msgSize, pPort->config.spw.ptr,
                    pPort->config.spw.link);
            break;

        case SPW_TC_PORT:
            switch(pPort->portTypeAux)
            {
                case SPW_USB_MK2_PORT:
                    RawSpWmk2RWError(status, msg, msgSize,
                            &pPort->config.spw.deviceId);
                    break;

                case SPW_USB_PORT:
                    RawSpWusbRWError(status, msg, msgSize);
                    break;

                case SPW_PCI_PORT:
                    RawSpWpciRWError(status, msg, msgSize,
                            pPort->config.spw.ptr, pPort->config.spw.link);
                    break;

                default:
                    break;
            }
            break;

        case UART_PORT:
            RawSerialRWError(status, msg, msgSize);
            break;

        case DUMMY_PORT: case NO_PORT:
            status = 0;
            break;
            
        case SOCKET_SRV_PORT: case SOCKET_CLI_PORT:
            RawSocketRWError(status, msg, msgSize);
            break;

        default:
            break;
    }
    return;
}

int FlushPort(portConfig * pPort, unsigned char * tempBuffer,
        unsigned int tempBufferSize)
{
    int status = 0;
    switch(pPort->portType)
    {
        case SPW_USB_MK2_PORT:
            status = FlushSpWmk2(tempBuffer, &tempBufferSize, &pPort->config.spw.channel);
            break;

        case SPW_USB_PORT:
            status = FlushSpWusb(tempBuffer, &tempBufferSize, pPort->config.spw.ptr);
            break;

        case SPW_PCI_PORT:
            status = FlushSpWpci(tempBuffer, &tempBufferSize, pPort->config.spw.ptr,
                    pPort->config.spw.link);
            break;

        case UART_PORT:
            FlushOutSerialQueues(pPort->config.uart.portNum);
            status = FlushInSerialQueues(pPort->config.uart.portNum);
            break;

        default:
            status = 0;
            break;
    }
    return status;
}

int PeriodicalTickIns(portConfig * pPort, int enable, unsigned int hertz)
{
    int status;
    switch(pPort->portTypeAux)
    {
        case SPW_USB_MK2_PORT:
            status = PeriodicalTickInsSpWmk2(&pPort->config.spw.deviceId, enable, hertz);
            break;

        case SPW_USB_PORT:
            status = PeriodicalTickInsSpWusb(pPort->config.spw.ptr, enable, hertz);
            break;

        case SPW_PCI_PORT:
            status = PeriodicalTickInsSpWpci(pPort->config.spw.ptr, enable, hertz);
            break;

        default:
            status = 0;
            break;
    }
    if(status == 0) //status = 0 in all SpW means OK
        status = 1;
    return status;
}

int SingleTickIn(portConfig * pPort, unsigned char value, 
	unsigned char external)
{
    int status = 1;
    switch(pPort->portTypeAux)
    {
        case SPW_USB_MK2_PORT:
            status = SingleTickInSpWmk2(&pPort->config.spw.deviceId, value, external);
            break;

        case SPW_USB_PORT:
            status = SingleTickInSpWusb(pPort->config.spw.ptr, value, external);
            break;

        case SPW_PCI_PORT:
            status = SingleTickInSpWpci(pPort->config.spw.ptr, value, external);
            break;

        default:
            break;
    }
    if(status == 0) //status = 0 in all SpW means OK
        status = 1;
    return status;
}

void EnableDisableEEP(portConfig * pPort, char enableDisable) 
{
    switch(pPort->portType)
    {
            case SPW_USB_MK2_PORT:
                EnableDisableEEPSpWmk2(enableDisable);
                break;

            case SPW_USB_PORT:
                EnableDisableEEPSpWusb(pPort->config.spw.ptr, enableDisable);
                break;

            case SPW_PCI_PORT:
                EnableDisableEEPSpWpci(pPort->config.spw.ptr, enableDisable,
                        pPort->config.spw.link);
                break;

            default:
                break;
    }
    return;
}

void *prepareReadProtocolPacketsBuffer(void * ports)
{
    readProtocolPacketsBuffer((portConfig*)ports);
    return (void *)0;
}

int CheckRawHeader(unsigned char * packet, unsigned int packetLen,
        portConfig * pPort, unsigned int portIndex)
{
    int status = 0;
    int dataLen = packetLen-pPort->portPhyHeaderOffsetTM;
    
    if(dataLen <= 0)
    {
        //this is because packet sync is not well formed
        //wrong byte will be last, which is length-1
        wrongValue[portIndex] = packetLen-1;
        status = WRONG_PACKET_SYNC;
    }
    else
    {
        if((pPort->portType == SPW_USB_MK2_PORT) ||
                (pPort->portType == SPW_USB_PORT) ||
                (pPort->portType == SPW_PCI_PORT))
        {
            unsigned int port = 0;
            /* look for valid reading ports */
            for(port=0; port<pPort->config.spw.numberOfReadingPorts; ++port)
            {
                if(packet[0] == pPort->config.spw.readingPorts[port])
                    break;
            }
            if(port == pPort->config.spw.numberOfReadingPorts)
            {
                wrongValue[portIndex] = (unsigned int)packet[0];
                status = WRONG_SPW_LOGIC_PORT;
            }
        }
        if((status == 0) && ((dataLen > MAX_PACKET_SIZE) || (dataLen < 0)))
        {
            wrongValue[portIndex] = dataLen;
            status = WRONG_LENGTH;
        }
    }
    return status;
}

void GetCheckRawHeaderError(int status, char * msg, int msgSize,
        portConfig * pPort, unsigned int portIndex)
{
    unsigned int idx = 0, len = 0;
    switch(status)
    {
        case WRONG_SPW_LOGIC_PORT:
            snprintf(msg, msgSize, "Packet Logical Address Received %02X != ",
                    wrongValue[portIndex]);
            for(idx=0; idx<pPort->config.spw.numberOfReadingPorts; ++idx)
            {
                snprintf(&msg[strlen(msg)], msgSize-strlen(msg), "%02X, ",
                        pPort->config.spw.readingPorts[idx]);
            }
            snprintf(&msg[strlen(msg)-2], 3, ": "); //remove last ", "
            break;
            
        case WRONG_LENGTH:
            len = snprintf(msg, msgSize, "Wrong length %d ",
                    wrongValue[portIndex]);
            if(wrongValue[portIndex] < 0)
                snprintf(&msg[len], msgSize-len, "< 0: ");
            else
                snprintf(&msg[len], msgSize-len, "> %d: ", MAX_PACKET_SIZE);
            break;
            
        case WRONG_PACKET_SYNC:
            len = snprintf(msg, msgSize, "Wrong packet sync at byte %d: ",
                    wrongValue[portIndex]);
            break;
            
        default:
            snprintf(msg, msgSize, "Unknown error %d: ", status);
            break;
    }
}

int PrepareServer(portConfig * pPort, unsigned int portIdx, int * pRetry)
{
    int status = 0;
    switch(pPort->portType)
    {
        case SOCKET_SRV_PORT:
            status = AcceptSocketServer(&pPort->config.socket.address,
                    &pPort->config.socket.socketHdl);
            if(status == 0)
            {
                pthread_mutex_lock(&ptclMutex[portIdx]);
                pPort->ptcl.portConnected = 1;
                pthread_mutex_unlock(&ptclMutex[portIdx]);
            }
            else
            {
                if(status == ACCEPT_NOT_POSSIBLE_SERVER_CLOSED)
                {
                    *pRetry = 0;
                    status = 0;
                }
            }
            break;

        default:
            break;
    }
    return status;
}

void DisplayPrepareServerError(int error, char * msg, int msgSize,
        portConfig * pPort, unsigned int portIdx)
{
    int length = snprintf(msg, MAX_MSG_SIZE,
            "Error at %s port (%u) while preparing server port %d: ",
            pPort->name, portIdx, pPort->config.socket.portNum);

    switch(pPort->portType)
    {
        case SOCKET_SRV_PORT:
            DisplaySocketError(error, &msg[length], msgSize-length);
            break;

        default:
            break;
    }
}

void UnprepareServer(portConfig * pPort, unsigned int portIdx)
{
    switch(pPort->portType)
    {
        case SOCKET_SRV_PORT:
            pthread_mutex_lock(&ptclMutex[portIdx]);
            pPort->ptcl.portConnected = 0;
            CloseSocket(NULL);
            pthread_mutex_unlock(&ptclMutex[portIdx]);
            break;

        default:
            break;
    }
}