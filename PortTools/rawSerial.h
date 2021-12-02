/**
 * \file	rawSerial.h
 * \brief	functions for write and read data in serial port (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		05/03/2012 at 15:36:42
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __rawSerial_H__
#define __rawSerial_H__

#include "../definitions.h"					/* portConfig */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to read data from a serial port with no defined format
 * \param[out]  packet          The buffer in which read data will be stored
 * \param       maxPacketSize   The maximum size of the buffer
 * \param       cpUart		The serial port configuration structure
 * \return The length read or a negative value if error occurred
 */
int ReadRawSerial(unsigned char * packet, unsigned int * pLength,
        protocolConfig * pPtcl);

/**
 * \brief Function to write data to a serial port with no defined format
 * \param       packet          The buffer where is stored the data which will be sent
 * \param       length          The number of bytes to write
 * \param       portHandle	The serial port handle
 * \return The length written or a negative value if error occurred
 */
int WriteRawSerial(const unsigned char * packet, int length,
	int portHandle);

/**
 * \brief Function which parses the errors produced while reading / writing data
 * \brief from/to a serial port with no defined format and copies an error
 * \brief message into a string
 * \param       error		The read/write status error to be parsed
 * \param[out]  msg             Character string to copy message error into
 * \param       msgSize         Max size of the string to copy message error into
 */
void RawSerialRWError(int error, char * msg, int msgSize);

/**
 * \brief Function to flush serial port input queues
 * \param       portHandle	The serial port handle
 * \return      0 if correct, a negative value if error occurred
 */
int FlushInSerialQueues(int portHandle);

/**
 * \brief Function to flush serial port output queues
 * \param       portHandle	The serial port handle
 * \return      0 if correct, a negative value if error occurred
 */
int FlushOutSerialQueues(int portHandle);


#ifdef __cplusplus
}
#endif
#endif  /* __rawSerial_H__ */