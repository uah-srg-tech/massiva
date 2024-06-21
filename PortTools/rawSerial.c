/**
 * \file	rawSerial.c
 * \brief	functions for write and read data in serial port (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		05/03/2012 at 15:36:42
 * Company:		Space Research Group, Universidad de AlcalÃ¯Â¿Â½.
 * Copyright:	Copyright (c) 2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include "configSerial.h"			/* comports */
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
#include "rawProtocol.h"

int ReadRawSerial(unsigned char * packet, unsigned int * pLength,
        protocolConfig * pPtcl)
{
    int length = ReadRawProtocol(packet, pLength, pPtcl);
    return length;
}

int WriteRawSerial(const unsigned char * packet, int length,
	unsigned int portHandle)
{
    int status = 0;
#if(defined _WIN32 || __CYGWIN__)
    BOOL res = WriteFile(comports[portHandle], packet, length,
            (LPDWORD)&status, NULL);
    if(res == FALSE)
        status = -GetLastError();
#elif(defined __linux__)
    status = write(ttyports[portHandle], packet, length);
    if(status < 0)
        status = -errno;
#endif
    return status;
}

void RawSerialRWError(int error, char * msg, int msgSize)
{
    int realError = -error;
#if(defined _WIN32 || __CYGWIN__)
    DWORD dwRC;
    LPVOID lpMsgBuf;
    int printNumLen = snprintf(msg, msgSize, "(%d) ", realError);
    char msgWin[msgSize-printNumLen];
    
#ifdef UNICODE
    dwRC = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, realError,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPWSTR) &lpMsgBuf,
            0, NULL);
    if (dwRC && lpMsgBuf)
        snprintf(msgWin, msgSize-printNumLen, "(%d) %ls", realError, (wchar_t*)lpMsgBuf);
#else
    dwRC = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, realError,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,
            0, NULL);
    if (dwRC && lpMsgBuf)
        snprintf(msgWin, msgSize-printNumLen, "(%d) %s", realError, (char*)lpMsgBuf);
#endif
    LocalFree(lpMsgBuf);
    printNumLen = strlen(msg);
    if(msg[printNumLen-1] == '\n')
    {
        if(msg[printNumLen-3] == '.')
            msg[printNumLen-2] = '.';
        if((msg[printNumLen-2] == '\r') || (msg[printNumLen-2] == '.'))
            msg[printNumLen-2] = '\0';
        msg[printNumLen-1] = '\0';
    }
#elif(defined __linux__)
    snprintf(msg, msgSize, "(%d) %s", realError, strerror(realError));
#endif
    return;
}

int FlushInSerialQueues(unsigned int portHandle)
{
    int status = 0;
#if(defined _WIN32 || __CYGWIN__)
    PurgeComm(comports[portHandle], PURGE_RXCLEAR);
#elif(defined __linux__)
    tcflush(ttyports[portHandle], TCIFLUSH);
#endif
    
    /* TODO: look for RS232 equivalent function */
    return status;
}

int FlushOutSerialQueues(unsigned int portHandle)
{
    int status = 0;
#if(defined _WIN32 || __CYGWIN__)
    PurgeComm(comports[portHandle], PURGE_TXCLEAR);
#elif(defined __linux__)
    tcflush(ttyports[portHandle], TCOFLUSH);
#endif
    return status;
}
