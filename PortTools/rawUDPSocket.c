/**
 * \file	rawSocket.c
 * \brief	functions for write and read data in UDP socket port (definition)
 *
 * \author	Aaron Montalvo, <aaron.montalvo@uah.es>
 * 
 * \internal
 * Created:		03/03/2021 at 13:35:17
 * Company:		Space Research Group, Universidad de Alcalï¿½.
 * Copyright:	Copyright (c) 2021, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#if(defined _WIN32 || __CYGWIN__)
#include <ws2tcpip.h>
#elif(defined __linux__)
#include <sys/types.h>
#include <netinet/ip.h>
#include <poll.h>
#include <errno.h>                                      /* errno */
#include <string.h>                                     /* strerror */
#endif
#include <stdio.h>

int ReadRawUDPSocket(unsigned char * packet, unsigned int * pLength,
        int * pSocket)
{
    /* first check if socket is still opened */
    if((*pSocket) == -1)
        return 0;
    /* then receive */
    int status = 0;
    struct sockaddr_in from;
#if(defined _WIN32 || __CYGWIN__)
    int fromLen = sizeof(from);
#elif(defined __linux__)
    unsigned int fromLen = sizeof(from);
#endif
    
#if(defined _WIN32 || __CYGWIN__)
    do {
#endif
    status = recvfrom((*pSocket), (char*)packet, *pLength, 0,
            (struct sockaddr*)&from, &fromLen);
#if(defined _WIN32 || __CYGWIN__)
    }while((status == -1) && (WSAGetLastError() == WSAECONNRESET));
    if((status == -1) && (WSAGetLastError() == WSAEINTR))
        status = 0;
#endif
    if(status > 0)
        *pLength = status;
    return status;
}

int WriteRawUDPSocket(const unsigned char * packet, int length, int socket,
        struct sockaddr_in remoteAddr)
{
    /* first check if socket is still opened */
    if(socket == -1)
        return 0;
    /* then send */
    int status = sendto(socket, (const char*)packet, length, 0,
            (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
    return status;
}

void RawUDPSocketRWError(int error, char * msg, int msgSize)
{
#if(defined _WIN32 || __CYGWIN__)
    error = WSAGetLastError();
    LPVOID lpMsgBuf;
    DWORD dwRC;
#ifdef UNICODE
    dwRC = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPWSTR) &lpMsgBuf,
            0, NULL);
    if (dwRC && lpMsgBuf)
        snprintf(msg, msgSize, "(%d) %ls", error, (char*)lpMsgBuf);
#else
    dwRC = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,
            0, NULL);
    if (dwRC && lpMsgBuf)
        snprintf(msg, msgSize, "(%d) %s", error, (char*)lpMsgBuf);
#endif
    LocalFree(lpMsgBuf);
#elif(defined __linux__)
    error = errno;
    snprintf(msg, msgSize, "(%d) %s", error, strerror(error));
#endif
}