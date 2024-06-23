/**
 * \file	configUDPSocket.c
 * \brief	
 *	
 * \author	Aaron Montalvo, <aaron.montalvo@uah.es>
 * 
 * \internal
 * Created:		03/03/2021 at 13:35:17
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2021, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#if(defined _WIN32 || __CYGWIN__)
#define _WIN32_WINNT 0x0601
#include <ws2tcpip.h>
#include <io.h>
#elif(defined __linux__)
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>                                  /* inet_pton */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>                                     /* strerror */
#include <unistd.h>                                     /* close */
#include <errno.h>
#endif
#include <stdio.h>										/* sprintf, snprintf */
#include "configUDPSocket.h"
#include "configTCPSocket.h"							/* ConfigureAddr */

static int errorNumber = 0;

enum {
    CREATE_SOCKET_ERROR = -1,
    SOCKET_OPTION_ERROR = -2,
    BIND_ERROR = -3,
    INET_PTON_WRONG_ADDRESS = -6,
    INET_PTON_ERROR = -7,
    WSASTARTUP_ERROR = -8
};

int ConfigureUDPSocket(const char localIp[16], unsigned int localPort, 
        struct sockaddr_in * pLocalAddr, const char remoteIp[16],
        unsigned int remotePort, struct sockaddr_in * pRemoteAddr,
        int * pSocket)
{
    if(*pSocket == -1)
    {
        /* configure server socket if not configured */
#if(defined _WIN32 || __CYGWIN__)
        WSADATA wsaData;
        if((errorNumber = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
        {
            return WSASTARTUP_ERROR;
        }
#endif
    
        if((*pSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
        {
#if(defined _WIN32 || __CYGWIN__)
            errorNumber = WSAGetLastError();
#elif(defined __linux__)
            errorNumber = errno;
#endif
            return CREATE_SOCKET_ERROR;
        }

#if(defined _WIN32 || __CYGWIN__)
        char opt = 1;
#elif(defined __linux__)
        int opt = 1;
#endif
        if(setsockopt(*pSocket, SOL_SOCKET, SO_REUSEADDR, &opt,
                sizeof(opt)) != 0) 
        {
#if(defined _WIN32 || __CYGWIN__)
            errorNumber = WSAGetLastError();
#elif(defined __linux__)
            errorNumber = errno;
#endif
            return SOCKET_OPTION_ERROR;
        }
    }
    int status = 0;
    status = ConfigureAddr(localIp, localPort, pLocalAddr);
    if(status < 0)
        //pLocalAddr->sin_addr.s_addr = INADDR_ANY;
        return status;
    status = ConfigureAddr(remoteIp, remotePort, pRemoteAddr);
    if(status < 0)
        //pRemoteAddr->sin_addr.s_addr = INADDR_ANY;
        return status;
    
    if((status=bind(*pSocket, (struct sockaddr *)pLocalAddr, sizeof(*pLocalAddr))) != 0)
    {
#if(defined _WIN32 || __CYGWIN__)
        errorNumber = WSAGetLastError();
#elif(defined __linux__)
        errorNumber = errno;
#endif
        return BIND_ERROR;
    }
    return 0;
}

void DisplayUDPSocketError(int error, char * msg, int msgSize)
{
    unsigned int msgLen = 0;
    
    switch (error)
    {
        case WSASTARTUP_ERROR:
            msgLen = snprintf(msg, msgSize, "WSA Initialization failure: %d", 
                    errorNumber);
            break;
            
        case CREATE_SOCKET_ERROR:
            msgLen = snprintf(msg, msgSize, "Socket error: ");
            break;
            
        case SOCKET_OPTION_ERROR:
            msgLen = snprintf(msg, msgSize, "Socket option error: ");
            break;
            
        case BIND_ERROR:
            msgLen = snprintf(msg, msgSize, "Bind error: ");
            break;
            
        case INET_PTON_WRONG_ADDRESS:
            snprintf(msg, msgSize, "Wrong IP address");
            break;
            
        case INET_PTON_ERROR:
            msgLen = snprintf(msg, msgSize, "inet_pton error: ");
            break;
            
        default:
            snprintf(msg, msgSize, "Unknown error %d", error);
            break;
    }
    
    if(msgLen != 0)
    {
#if(defined _WIN32 || __CYGWIN__)
    LPVOID lpMsgBuf;
    DWORD dwRC;
#ifdef UNICODE
    dwRC = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorNumber,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPWSTR) &lpMsgBuf,
            0, NULL);
    if (dwRC && lpMsgBuf)
        snprintf(&msg[msgLen], msgSize-msgLen, "(%d) %ls", errorNumber,
                (wchar_t*)lpMsgBuf);
#else
    dwRC = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorNumber,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,
            0, NULL);
    if (dwRC && lpMsgBuf)
        snprintf(&msg[msgLen], msgSize-msgLen, "(%d) %s", errorNumber,
                (char*)lpMsgBuf);
#endif
    LocalFree(lpMsgBuf);
#elif(defined __linux__)
        snprintf(&msg[msgLen], msgSize-msgLen, "(%d) %s", errorNumber,
                strerror(errorNumber));
#endif
    }
}

int CloseUDPSocket(int * pSocket)
{
#if(defined _WIN32 || __CYGWIN__)
    closesocket(*pSocket);
#elif(defined __linux__)
    shutdown(*pSocket, SHUT_RDWR);
    close(*pSocket);
#endif
    *pSocket = -1;
    return 0;
}
