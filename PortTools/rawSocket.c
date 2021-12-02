/**
 * \file	rawSocket.c
 * \brief	functions for write and read data in socket port (definition)
 *
 * \author	Aaron Montalvo, <aaron.montalvo@uah.es>
 * 
 * \internal
 * Created:		03/03/2021 at 13:35:17
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2021, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#if(defined _WIN32 || __CYGWIN__)
#include <ws2tcpip.h>
#elif(defined __linux__)
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>                                     /* strerror */
#endif
#include <stdio.h>
#include "rawProtocol.h"

int ReadRawSocket(unsigned char * packet, unsigned int * pLength,
        int * pSocket, protocolConfig * pPtcl)
{
    if((*pSocket) == -1)
        return 0;
    int length = ReadRawProtocol(packet, pLength, pPtcl);
    return length;
}

int WriteRawSocket(const unsigned char * packet, int length, int socket)
{
    /* first check if socket is still opened */
    if(socket == -1)
        return 0;
    /* then send */
    int status = send(socket, (const char*)packet, length, 0);
    return status;
}

void RawSocketRWError(int error, char * msg, int msgSize)
{
    int realError = -error;
#if(defined _WIN32 || __CYGWIN__)
    LPVOID lpMsgBuf;
    DWORD dwRC;
#ifdef UNICODE
    dwRC = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, realError,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPWSTR) &lpMsgBuf,
            0, NULL);
    if (dwRC && lpMsgBuf)
        snprintf(msg, msgSize, "(%d) %ls", error, (char*)lpMsgBuf);
#else
    dwRC = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, realError,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,
            0, NULL);
    if (dwRC && lpMsgBuf)
        snprintf(msg, msgSize, "(%d) %s", error, (char*)lpMsgBuf);
#endif
    LocalFree(lpMsgBuf);
#elif(defined __linux__)
    snprintf(msg, msgSize, "(%d) %s", realError, strerror(realError));
#endif
}