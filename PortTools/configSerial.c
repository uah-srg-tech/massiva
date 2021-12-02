/**
 * \file	configSerial.c
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		14/03/2012 at 12:27:17
 * Company:		Space Research Group, Universidad de AlcalÃ¡.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include "configSerial.h"
#include <stdio.h>					/* sprintf, snprintf */

#if(defined _WIN32 || __CYGWIN__)
#include <windows.h>
HANDLE comports[MAX_SERIAL_PORTS];
#elif(defined __linux__)
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#ifndef CMSPAR
#define CMSPAR   010000000000
#endif
#endif

enum {
    COMPORT_NUMBER_OUT_OF_RANGE = -1,
#if(defined _WIN32 || __CYGWIN__)
    INVALID_HANDLE = -2,
#elif(defined __linux__)
    INVALID_BAUD_RATE = -2,
    INVALID_DATA_BITS = -3,
    INVALID_STOP_BITS =- 4,
    INVALID_PARITY = -5,
    CANT_OPEN_PORT = -6,
    CANT_SET_OUT_BAUDRATE = -7,
    CANT_SET_IN_BAUDRATE = -8,
    CANT_SET_CONFIG = -9,
#endif
};
static int errorNumber = 0;
static int currPortHdl = 0;
#ifdef __linux__
int ttyports[MAX_SERIAL_PORTS];
static char ttyNames[5][MAX_DEV_NAME_LEN] =
{
    "/usr/dev/ttyUSB",
    "/dev/ttyUSB", 
    "/usr/dev/ttyS",
    "/dev/tty", 
    "/dev/pts/"
    
};
static int errorNumbers[4] = {0, 0, 0, 0}, errorPort = -1;
#endif
void resetUartPortHandles()
{   
    currPortHdl = 0;
}

/**
 * \brief Function to configure serial port (currently as 8-N-1)
 * \param uart                  Port number and string
 * \param baudRate		Baud rate (currently 115200)
 * \param parity		Parity mode (currently 0 = no parity)
 * \param dataBits		Number of data bits (currently 8)
 * \param stopBits		Number of stop bits (currently 1)
 * \param inputQueueSize	Size of the input queue (non-used)
 * \param outputQueueSize	Size of the output queue (non-used)
 * \return Returns the status (0 if correct, < 0 if error)
 */
int ConfigureSerial(uartConfig * uart, long baudRate, int parity, int dataBits,
        int stopBits, flowControl control, char deviceInfo[MAX_DEV_INFO_LEN])
{   
    errorNumber = 0;
    char flowControlStr[11] = "";
    if(uart->portNum != -1)
    {
        /* create portName from portNum */
        if(uart->portNum > MAX_SERIAL_PORTS)
        {
            errorNumber = uart->portNum;
            return COMPORT_NUMBER_OUT_OF_RANGE;
        }
#if(defined _WIN32 || __CYGWIN__)
        snprintf(uart->portName, MAX_DEV_NAME_LEN, "\\\\.\\COM%u", uart->portNum);
#elif(defined __linux__)
        /* first check for any default ttyUSB "/dev/ttyUSB" port */
        snprintf(uart->portName, MAX_DEV_NAME_LEN, "%s%u", ttyNames[0], uart->portNum);
        ttyports[currPortHdl] = open(uart->portName, O_RDWR | O_NOCTTY | O_NDELAY);
        if (ttyports[currPortHdl] < 0)
        {
            errorNumber = errno;
            /* then check for any ttyUSB in /usr/ "/usr/dev/ttyUSB" */
            snprintf(uart->portName, MAX_DEV_NAME_LEN, "%s%u", ttyNames[1], uart->portNum);
            ttyports[currPortHdl] = open(uart->portName, O_RDWR | O_NOCTTY | O_NDELAY);
            if (ttyports[currPortHdl] < 0)
            {
                errorNumbers[0] = errno;
                /* then check for any /dev/ttyUSB" port */
                snprintf(uart->portName, MAX_DEV_NAME_LEN, "%s%u", ttyNames[2], uart->portNum);
                ttyports[currPortHdl] = open(uart->portName, O_RDWR | O_NOCTTY | O_NDELAY);
                if (ttyports[currPortHdl] < 0)
                {
                    errorNumbers[1] = errno;
                    /* then check for "/usr/dev/ttyS (socat)" persistent defined ttyUSB port */
                    snprintf(uart->portName, MAX_DEV_NAME_LEN, "%s%u", ttyNames[3], uart->portNum);
                    ttyports[currPortHdl] = open(uart->portName, O_RDWR | O_NOCTTY | O_NDELAY);
                    if (ttyports[currPortHdl] < 0)
                    {
                        errorNumbers[2] = errno;
                        /* finally check for "/dev/pts/" simulated port */
                        snprintf(uart->portName, MAX_DEV_NAME_LEN, "%s%u", ttyNames[4], uart->portNum);
                    }
                }
            }
        }
#endif
    }
    
#if(defined _WIN32 || __CYGWIN__)
    else if(strncmp(uart->portName, "COM", 3)== 0)
    {
        /* fix port name for win api*/
        char temp[MAX_DEV_NAME_LEN];
        snprintf(temp, MAX_DEV_NAME_LEN, "%s", uart->portName);
        snprintf(uart->portName, MAX_DEV_NAME_LEN, "\\\\.\\%s", temp);
    }
    
    DCB dcbSerialParams;
    memset(&dcbSerialParams, 0, sizeof(dcbSerialParams));
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    dcbSerialParams.fBinary = TRUE; //Windows doesn't support non-binary

#ifdef UNICODE
    wchar_t uartPortName_w[MAX_DEV_NAME_LEN];
    mbstowcs(uartPortName_w, uart->portName, MAX_DEV_NAME_LEN);
    comports[currPortHdl] = CreateFile(uartPortName_w,
            GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
#else
    comports[currPortHdl] = CreateFile(uart->portName,
            GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
#endif
    
    if(comports[currPortHdl] == INVALID_HANDLE_VALUE)
    {
        return INVALID_HANDLE;
    }
    
    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = dataBits;
    dcbSerialParams.Parity = parity;

    if(control == RTS_CTS)
    {
        snprintf(flowControlStr, 11, ", RTS/CTS");
        dcbSerialParams.fOutxCtsFlow = TRUE;
        dcbSerialParams.fRtsControl = RTS_CONTROL_HANDSHAKE;
    }
    else if(control == XON_OFF)
    {
        snprintf(flowControlStr, 11, ", XON/XOFF");
        dcbSerialParams.fOutX = TRUE;
        dcbSerialParams.fInX = TRUE;
    }
    else if(control == DTR_DSR)
    {
        snprintf(flowControlStr, 11, ", DTR/DSR");
        dcbSerialParams.fOutxDsrFlow = TRUE;
        dcbSerialParams.fDsrSensitivity = TRUE;
        dcbSerialParams.fDtrControl = DTR_CONTROL_HANDSHAKE;
    }

    switch(stopBits)
    {
    	case 1: default:
            dcbSerialParams.StopBits = ONESTOPBIT;
            break;
			
    	case 2:
            dcbSerialParams.StopBits = TWOSTOPBITS;
            break;
    }
    if(!SetCommState(comports[currPortHdl], &dcbSerialParams))
    {
       	return GetLastError();
    }
    SetSerialTimeout(uart->portNum, 0.5);
#elif(defined __linux__)
    int cBaudrate, cDataBits, cStopBits, cParity;
    struct termios serialParams;
    
    switch(baudRate)
    {
	case 50:
            cBaudrate= B50;
            break;
	case 75:
            cBaudrate= B75;
            break;
	case 110:
            cBaudrate= B110;
            break;
	case 134:
            cBaudrate= B134;
            break;
	case 150:
            cBaudrate= B150;
            break;
	case 200:
            cBaudrate= B200;
            break;
	case 300:
            cBaudrate= B300;
            break;
	case 600:
            cBaudrate= B600;
            break;
	case 1200:
            cBaudrate= B1200;
            break;
	case 1800:
            cBaudrate= B1800;
            break;
	case 2400:
            cBaudrate= B2400;
            break;
	case 4800:
            cBaudrate= B4800;
            break;
	case 9600:
            cBaudrate= B9600;
            break;
	case 19200:
            cBaudrate= B19200;
            break;
	case 38400:
            cBaudrate= B38400;
            break;
	case 57600:
            cBaudrate= B57600;
            break;
	case 115200:
            cBaudrate= B115200;
            break;
	case 230400:
            cBaudrate= B230400;
            break;
	case 460800:
            cBaudrate= B460800;
            break;
	case 500000:
            cBaudrate= B500000;
            break;
	case 576000:
            cBaudrate= B576000;
            break;
	case 921600:
            cBaudrate= B921600;
            break;
	case 1000000:
            cBaudrate= B1000000;
            break;
	case 1152000:
            cBaudrate= B1152000;
            break;
	case 1500000:
            cBaudrate= B1500000;
            break;
	case 2000000:
            cBaudrate= B2000000;
            break;
	case 2500000:
            cBaudrate= B2500000;
            break;
	case 3000000:
            cBaudrate= B3000000;
            break;
	case 3500000:
            cBaudrate= B3500000;
            break;
	case 4000000:
            cBaudrate= B4000000;
            break;
	default:
            errorNumber = baudRate;
            return INVALID_BAUD_RATE;
    }
    switch(dataBits)
    {
        case 5:
            cDataBits = CS5;
            break;
        case 6:
            cDataBits = CS6;
            break;
        case 7:
            cDataBits = CS7;
            break;
        case 8:
            cDataBits = CS8;
            break;
        default:
            errorNumber = dataBits;
            return INVALID_DATA_BITS;
    }
    switch(stopBits)
    {
        case 1:
            cStopBits = 0;
            break;
        case 2:
            cStopBits = CSTOPB;
            break;
        default :
            errorNumber = stopBits;
            return INVALID_STOP_BITS;
    }
    switch(parity)
    {
        case 0://no parity
            cParity = 0;
            break;
            
        case 1://odd
            cParity = PARENB|PARODD;
            break;
            
        case 2://even
            cParity = PARENB;
            break;
            
        case 3://mark
            cParity = PARENB|PARODD|CMSPAR;
            break;
            
        case 4://space
            cParity = PARENB|CMSPAR;
            break;
        
        default:
            errorNumber = parity;
            return INVALID_PARITY;
    }

    /* if portNum used, check for "/dev/pts/" simulated port */
    ttyports[currPortHdl] = open(uart->portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (ttyports[currPortHdl] < 0)
    {
        errorNumbers[3] = errno;
        errorPort = uart->portNum;
        if(uart->portNum != -1)
            /* copy again first port name for printing error */
            snprintf(uart->portName, MAX_DEV_NAME_LEN, "%s%u", ttyNames[0], uart->portNum);
        return CANT_OPEN_PORT;
    }
    struct termios getParams;
    tcgetattr(ttyports[currPortHdl], &getParams);
    
    memset(&serialParams, 0, sizeof(serialParams));
    if(cfsetospeed(&serialParams, (speed_t)cBaudrate) != 0)
    {
        errorNumber = errno;
        return CANT_SET_OUT_BAUDRATE;
    }
    if(cfsetispeed(&serialParams, (speed_t)cBaudrate) != 0)
    {
        errorNumber = errno;
        return CANT_SET_IN_BAUDRATE;
    }
    
    /*
     * CRTSCTS: Enable RTS/CTS (hardware) flow control
     * CLOCAL : Ignore modem control lines (CD signal)
     * CREAD  : Enable receiver
     * HUPCL  : Hang up after last closing
     */
    
    serialParams.c_iflag = IGNBRK;
    serialParams.c_cflag = cBaudrate | cDataBits | cStopBits | cParity | CLOCAL | CREAD | HUPCL;
    if(control == RTS_CTS)
    {
        snprintf(flowControlStr, 11, ", RTS/CTS");
        serialParams.c_cflag |= CRTSCTS;
    }
    else if(control == XON_OFF)
    {
        snprintf(flowControlStr, 11, ", XON/XOFF");
        serialParams.c_iflag |= IXON | IXOFF;
    }
#if 0
    //flags only valid in BSD
    else if(control == DTR_DSR)
    {
        snprintf(flowControlStr, 11, ", DTR/DSR");
        serialParams.c_cflag |= CDTR_IFLOW | CDSR_OFLOW;
    }
#endif
        
    /* fetch bytes as they become available */
    serialParams.c_cc[VTIME] = 1;
    serialParams.c_cc[VMIN] = 1;
    
    if (tcsetattr(ttyports[currPortHdl], TCSANOW, &serialParams) != 0)
    {
        errorNumber = errno;
        return CANT_SET_CONFIG;
    }
#endif
        
    //create device info string
    char parityChar = '?';
    switch(parity)
    {
        case 0:
           parityChar = 'N';
           break;
        case 1:
           parityChar = 'O';
           break;
        case 2:
           parityChar = 'E';
           break;
        case 3:
           parityChar = 'M';
           break;
        case 4:
           parityChar = 'S';
           break;
        default:
            break;
    }
    snprintf(deviceInfo, MAX_DEV_INFO_LEN, "%ld/%d-%c-%d%s",
            baudRate, dataBits, parityChar, stopBits, flowControlStr);
    
    /* use portNum as Handle */
    uart->portNum = currPortHdl++;
    return 0;
}

/**
 * \brief Function which parses the last serial port error error and copies an
 * \brief error message into a string
 * \param[out] msg	The string to copy the error message in
 * \param msgSize	Maximum size of the string to copy the error message in
 */
void DisplaySerialError(int error, char * msg, int msgSize)
{
#if(defined _WIN32 || __CYGWIN__)
    DWORD dwRC;
    LPVOID lpMsgBuf;
#elif(defined __linux__)
    unsigned int msgPrint = 0;
#endif
    
    switch (error)
    {
        case COMPORT_NUMBER_OUT_OF_RANGE:
            snprintf(msg, msgSize, "COM port number (%d) out of range (%d)",
#if(defined _WIN32 || __CYGWIN__)
                    errorNumber, MAX_SERIAL_PORTS);
#elif(defined __linux__)
                    errorNumber, MAX_SERIAL_PORTS-1);
#endif
            break;
        
#if(defined _WIN32 || __CYGWIN__)
        case INVALID_HANDLE:
            snprintf(msg, msgSize, "Invalid handle");
            break;
            
#elif(defined __linux__)
        case INVALID_BAUD_RATE:
            snprintf(msg, msgSize, "Invalid baudrate %d", errorNumber);
            break;
            
        case INVALID_DATA_BITS:
            snprintf(msg, msgSize, "Invalid data bits %d", errorNumber);
            break;
            
        case INVALID_STOP_BITS:
            snprintf(msg, msgSize, "Invalid stop bits %d", errorNumber);
            break;
            
        case INVALID_PARITY:
            snprintf(msg, msgSize, "Invalid parity %d", errorNumber);
            break;
            
        case CANT_OPEN_PORT:
            msgPrint = snprintf(msg, msgSize, "Can't open port: %s",
                    strerror(errorNumber));
            if(errorPort != -1)
            {
                for(unsigned int i=0; i<4; ++i)
                {
                    if(errorNumbers[i] == 0)
                    {
                        break;
                    }
                    else
                    {
                        /* print from second tty, as first was already printed */
                        msgPrint += snprintf(&msg[msgPrint], msgSize-msgPrint,
                                "\n(%s%u): Can't open port: %s", ttyNames[i+1],
                                errorPort, strerror(errorNumbers[i]));
                        errorNumbers[i] = 0;
                    }
                }
            }
            break;
            
        case CANT_SET_OUT_BAUDRATE:
            snprintf(msg, msgSize, "Can't set output baudrate: %s (%d)",
                    strerror(errorNumber), errorNumber);
            break;
            
        case CANT_SET_IN_BAUDRATE:
            snprintf(msg, msgSize, "Can't set input baudrate: %s (%d)",
                    strerror(errorNumber), errorNumber);
            break;
            
        case CANT_SET_CONFIG:
            snprintf(msg, msgSize, "Can't set config: %s (%d)", 
                    strerror(errorNumber), errorNumber);
            break;
#endif
            
        default:
#if(defined _WIN32 || __CYGWIN__)
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
            snprintf(msg, msgSize, "(%d) %s", error, strerror(error));
#endif
            break;
	}
	return;
}

/**
 * \brief Function to close a serial port
 * \param comport	The serial port to close
 * \return Returns the status (0 if correct, <0 if error)
 */
int CloseSerialPort(unsigned int portHandle)
{
#if(defined _WIN32 || __CYGWIN__)
    if(!CloseHandle(comports[portHandle]))
    {
        return GetLastError();
    }
#elif(defined __linux__)
    if(close(ttyports[portHandle]) < 0)
    {
        return errno;
    }
#endif
    return 0;
}

int SetSerialTimeout (unsigned int portHandle, double timeout)
{
    int status = 0;
#if(defined _WIN32 || __CYGWIN__)
    COMMTIMEOUTS timeouts;				// all in milliseconds
    timeouts.ReadIntervalTimeout = (DWORD)timeout; 	// max timeout between RXd bytes
    timeouts.ReadTotalTimeoutMultiplier = 0;            // timeout per RXd byte
    timeouts.ReadTotalTimeoutConstant = 0;		// min constant timeout for RX
    timeouts.WriteTotalTimeoutMultiplier = 0;           // timeout per TXd byte
    timeouts.WriteTotalTimeoutConstant = 0;		// min constant timeout for TX
    if (!SetCommTimeouts(comports[portHandle], &timeouts))
    {
        status = GetLastError();
    }
#else
    UNUSED(portHandle);
    UNUSED(timeout);
#endif
    return status;
}