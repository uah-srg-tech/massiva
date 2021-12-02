/**
 * \file	configSocket.c
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
#endif
#include <errno.h>
#include <stdio.h>					/* sprintf, snprintf */
#include "configSocket.h"

static int errorNumber = 0;
static int server_fd = -1;

enum {
    CREATE_SOCKET_ERROR = -1,
    SOCKET_OPTION_ERROR = -2,
    BIND_ERROR = -3,
    LISTEN_ERROR = -4,
    ACCEPT_ERROR = -5,
    INET_PTON_WRONG_ADDRESS = -6,
    INET_PTON_ERROR = -7,
    CONNECT_ERROR = -8,
    WSASTARTUP_ERROR = -9,
    SELECT_ERROR = -10
};

int ConfigureSocketServer(unsigned int portNum, struct sockaddr_in * pAddress)
{
    if(server_fd == -1)
    {
        /* configure server socket if not configured */
#if(defined _WIN32 || __CYGWIN__)
        WSADATA wsaData;
        if((errorNumber = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
        {
            return WSASTARTUP_ERROR;
        }
#endif
    
        if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
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
        if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
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
    
    (*pAddress).sin_family = AF_INET; 
    (*pAddress).sin_addr.s_addr = INADDR_ANY; 
    (*pAddress).sin_port = htons(portNum);
    
    if(bind(server_fd, (struct sockaddr *)pAddress, sizeof(*pAddress)) != 0) 
    {
#if(defined _WIN32 || __CYGWIN__)
        errorNumber = WSAGetLastError();
#elif(defined __linux__)
        errorNumber = errno;
#endif
        return BIND_ERROR;
    }
    
    if (listen(server_fd, 3) < 0) 
    {
#if(defined _WIN32 || __CYGWIN__)
        errorNumber = WSAGetLastError();
#elif(defined __linux__)
        errorNumber = errno;
#endif
        return LISTEN_ERROR;
    }
    
    return 0;
}

int AcceptSocketServer(struct sockaddr_in * pAddress, int * pAcceptedSocket)
{    
    fd_set readSet;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 50000;
    int addrlen = sizeof(*pAddress); 
    
    while((server_fd != -1) && (*pAcceptedSocket == -1))
    {
        int status = 0;
        FD_ZERO(&readSet);
        FD_SET(server_fd, &readSet);
        if((status = select(server_fd + 1, &readSet, NULL, NULL, &timeout)) != 0)
        {
            if (status < 0) 
            {
#if(defined _WIN32 || __CYGWIN__)
                errorNumber = WSAGetLastError();
#elif(defined __linux__)
                errorNumber = errno;
#endif
                return SELECT_ERROR;
            }
            if((*pAcceptedSocket = accept(server_fd, (struct sockaddr *)pAddress,
                    (socklen_t*)&addrlen)) < 0) 
            {
#if(defined _WIN32 || __CYGWIN__)
                errorNumber = WSAGetLastError();
#elif(defined __linux__)
                errorNumber = errno;
#endif
                return ACCEPT_ERROR;
            }
            /* enable non-blocking socket */
            u_long mode = 1;
#if(defined _WIN32 || __CYGWIN__)
            ioctlsocket(*pAcceptedSocket, FIONBIO, &mode);
#elif(defined __linux__)
            ioctl(*pAcceptedSocket, FIONBIO, &mode);
#endif
        }
    }
    
    if(server_fd == -1)
        return ACCEPT_NOT_POSSIBLE_SERVER_CLOSED;
    else
        return 0;
}

int ConfigureSocketClient(unsigned int portNum, const char ipAddress[16],
        int * pConnectedSocket)
{
    struct sockaddr_in address;
    int status = 0;

#if(defined _WIN32 || __CYGWIN__)
    WSADATA wsaData;
    if((errorNumber = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
    {
        return WSASTARTUP_ERROR;
    }
#endif
    if((*pConnectedSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        errorNumber = errno;
        return CREATE_SOCKET_ERROR;
    } 
    address.sin_family = AF_INET; 
    address.sin_port = htons(portNum);
    
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if((status=inet_pton(AF_INET, ipAddress, &address.sin_addr)) != 1)
    {
        if(status == 0)
        {
            return INET_PTON_WRONG_ADDRESS;
        }
        else
        {
#if(defined _WIN32 || __CYGWIN__)
            errorNumber = WSAGetLastError();
#elif(defined __linux__)
            errorNumber = errno;
#endif
            return INET_PTON_ERROR;
        }
    }    
    
    if(connect(*pConnectedSocket, (struct sockaddr *)&address, sizeof(address)) != 0)
    {
#if(defined _WIN32 || __CYGWIN__)
        errorNumber = WSAGetLastError();
#elif(defined __linux__)
        errorNumber = errno;
#endif
        return CONNECT_ERROR; 
    }
    /* enable non-blocking socket */
    u_long mode = 1;
#if(defined _WIN32 || __CYGWIN__)
    ioctlsocket(*pConnectedSocket, FIONBIO, &mode);
#elif(defined __linux__)
    ioctl(*pConnectedSocket, FIONBIO, &mode);
#endif
    return 0; 
}

void DisplaySocketError(int error, char * msg, int msgSize)
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
            
        case LISTEN_ERROR:
            msgLen = snprintf(msg, msgSize, "Listen error: ");
            break;
            
        case ACCEPT_ERROR:
            msgLen = snprintf(msg, msgSize, "Accept error: ");
            break;
            
        case INET_PTON_WRONG_ADDRESS:
            snprintf(msg, msgSize, "Wrong IP address");
            break;
            
        case INET_PTON_ERROR:
            msgLen = snprintf(msg, msgSize, "inet_pton error: ");
            break;
            
        case CONNECT_ERROR:
            msgLen = snprintf(msg, msgSize, "Connect error: ");
            break;
            
        case SELECT_ERROR:
            msgLen = snprintf(msg, msgSize, "Select error: ");
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
                (char*)lpMsgBuf);
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

int CloseSocket(int * pConnectedSocket)
{
    if(pConnectedSocket == NULL)
    {
#if(defined _WIN32 || __CYGWIN__)
        closesocket(server_fd);
        WSACleanup();
#elif(defined __linux__)
        close(server_fd);
#endif
        server_fd = -1;
    }
    else if((*pConnectedSocket) != -1)
    {
#if(defined _WIN32 || __CYGWIN__)
        closesocket(*pConnectedSocket);
#elif(defined __linux__)
        close(*pConnectedSocket);
#endif
        *pConnectedSocket = -1;
    }
    return 0;
}