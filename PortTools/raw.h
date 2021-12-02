/**
 * \file	raw.h
 * \brief	functions for write and read data with interfaces (declaration)
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
#ifndef __RAW_H__
#define __RAW_H__

#include "../definitions.h"			/* portConfig */

#ifdef __cplusplus
extern "C" {
#endif

enum spwErrors{
    DISCONNECT
};

/**
 * \brief Function to read data from a port (serial / spw)
 * \param[out]	packet          The buffer in which read data will be stored
 * \param	pLength		Pointer to the number of bytes to read
 * \param	pPort		Pointer to the port (serial / spw) in which read data
 * \return	The length of the packet read
 */
int ReadRaw(unsigned char * packet, unsigned int * pLength, portConfig * pPort);

/**
 * \brief Function to write data to a port (serial / spw)
 * \param	packet		The buffer where is stored the data which will be sent
 * \param	length		The number of bytes to write
 * \param	pPort		The port (serial / spw) in which write data
 * \return	The length of the packet written
 */
int WriteRaw(const unsigned char * packet, int length, portConfig * pPort);

/**
 * \brief Function which parses the errors produced while reading / writing data
 * \brief from/to a port with no defined format and copies an error
 * \brief message into a string
 * \param	error		The raw read/write status error to be parsed
 * \param[out]	msg		Character string to copy message error into
 * \param	msgSize		Max size of the string to copy message error into
 * \param	pPort		Port (interface) handle
 */
void RawRWError(int error, char * msg, int msgSize, portConfig * pPort); 

/**
 * \brief Function to flush input and output queues
 * \param	pPort		The port (serial / spw) in which write data
 * \param       tempBuffer	A temporal buffer to be used to flush ports
 * \param       tempBufferSize	The size of the temporal buffer
 * \return      0 if correct, a negative value if error occurred
 */
int FlushPort(portConfig * pPort, unsigned char * tempBuffer,
        unsigned int tempBufferSize);

/**
 * \brief Function to be launched in other thread by main thread
 * \brief for performing an infinite data request at serial port to making
 * \brief serial read blocking and packet-oriented
 * \param	ports		Pointer to ports handle structure
 */
void *prepareReadProtocolPacketsBuffer(void * ports);

/**
 * \brief Function to enable/disable periodical tick-ins via a SpW-USB brick
 * \param       pPort           Pointer to SpW struct to send/stop sending SpW tick-ins
 * \param       enable          Whether enable or disable tick-ins
 * \param       hertz		Frequency of the tick-ins
 * \return Returns the status(0 if correct, < 0 if error)
 */
int PeriodicalTickIns(portConfig * pPort, int enable, unsigned int hertz);

/**
 * \brief Function to send a single tick-ins via a SpW-USB brick
 * \param       pPort           Pointer to SpW struct to send the SpW tick-ins
 * \param       value		Value to write in the tick-in
 * \param       external	Whether external TC or not
 * \return Returns the status(0 if correct, < 0 if error)
 */
int SingleTickIn(portConfig * pPort, unsigned char value,
	unsigned char external);

/**
 * \brief Function to enable/disable sending periodical tick-ins via a
 * \brief SpW-USB MK2 brick
 * \param       pPort           Pointer to SpW struct to enable or disable EEP in SpW packet
 * \param       enableDisable	Whether enable or disable EEP
 */
void EnableDisableEEP(portConfig * pPort, char enableDisable); 

/**
 * \brief Function to check the physical (raw) header of a packet
 * \param       packet          Packet to be checked
 * \param       packetLen       Length of the packet
 * \param	pPort		The port (serial / spw) of the packet
 * \param	portIndex	The number of port intended to be configured
 * \return      Returns the status(0 if correct, < 0 if error)
 */
int CheckRawHeader(unsigned char * packet, unsigned int packetLen,
        portConfig * pPort, unsigned int portIndex);

/**
 * \brief Function which parses the errors produced while reading / writing data
 * \brief from/to a port with no defined format and copies an error
 * \brief message into a string
 * \param	status		The checking error to be parsed
 * \param[out]	msg		Character string to copy message error into
 * \param	msgSize		Max size of the string to copy message error into
 * \param	pPort		Port (interface) handle
 * \param	portIndex	The number of port intended to be configured
 */
void GetCheckRawHeaderError(int status, char * msg, int msgSize,
        portConfig * pPort, unsigned int portIndex);

int PrepareServer(portConfig * pPort, unsigned int portIdx, int * pRetry);
void DisplayPrepareServerError(int error, char * msg, int msgSize,
        portConfig * pPort, unsigned int portIdx);
void UnprepareServer(portConfig * pPort, unsigned int portIdx);
            
#ifdef __cplusplus
}
#endif
#endif  /* __RAW_H__ */