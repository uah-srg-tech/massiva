/**
 * \file	configSerial.h
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		14/05/2012 at 12:16:58
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __configSerial_H__
#define __configSerial_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SERIAL_PORTS                            10
    
#if(defined _WIN32 || __CYGWIN__)
#include <windows.h>			/* HANDLE */
extern HANDLE comports[MAX_SERIAL_PORTS];
#elif(defined __linux__)
extern int ttyports[MAX_SERIAL_PORTS];
#endif
#include <pthread.h>
#include "../definitions.h"

void resetUartPortHandles();
/**
 * \brief Function to configure a serial port
 * \param	portHandle	The serial port handle
 * \param	baudRate	baudRate value for configuring port
 * \param	parity          parity value for configuring port
 * \param	stopBits	stopBits value for configuring port
 * \param	inputQueueSize	inputQueueSize value for configuring port
 * \param	outputQueueSize	outputQueueSize value for configuring port
 * \return	0 if correct, a negative value if error occurred
 */
int ConfigureSerial(uartConfig * uart, long baudRate, int parity, int dataBits,
        int stopBits, flowControl control, char deviceInfo[MAX_DEV_INFO_LEN]);

/**
 * \brief Function which parses the errors produced when configuring a serial port
 * \brief and copies an error message into a string
 * \param	error		The status error to be parsed
 * \param[out]	msg		Character string to copy message error into
 * \param	msgSize 	Max size of the string to copy message error into
 */
void DisplaySerialError(int error, char * msg, int msgSize);

/**
 * \brief Function to unconfigure a serial port
 * \param	portHandle	The serial port handle
 * \return	0 if correct, a negative value if error occurred
 */
int CloseSerialPort(unsigned int portHandle);

/**
 * \brief Function to set the timeout of a serial port
 * \param comport	The serial port in which timeout will be set
 * \param timeout	The timeout value to set
 * \return 0 if correct, a negative value if error occurred
 */
int SetSerialTimeout(unsigned int portHandle, double timeout);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __configSerial_H__ */