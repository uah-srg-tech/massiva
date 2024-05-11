/**
 * \file	rawSpWusb.h
 * \brief	functions for write and read data in SpaceWire port (declaration)
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
#ifndef __rawSpWusb_H__
#define __rawSpWusb_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to read data from SpW-USB brick
 * \param[out] receivedPacket	The buffer in which read data will be stored
 * \param length				The number of bytes to read
 * \param hDevice				The SpW-USB brick in which read data
 * \return 0 if correct, a negative value if error occurred
 */
int ReadRawSpWusb(unsigned char * receivedPacket, int * pLength, void * hDevice);
/**
 * \brief Function to write data to a SpW-USB brick
 * \param sendPacket		The buffer where is the data which will be sent
 * \param length			The number of bytes to write
 * \param hDevice			The SpW-USB brick to which write data
 * \return 0 if correct, a negative value if error occurred
 */
int WriteRawSpWusb(const unsigned char * sendPacket, int length, void * hDevice);
/**
 * \brief Function which parses the errors produced while reading / writing data
 * \brief from/to a SpW port
 * \param error		The raw read/write status error to be parsed
 * \param[out] msg	Character string to copy message error into
 * \param hDevice	The SpW-USB brick which failed
 * \param msgSize	Max size of the string to copy message error into
 */
void RawSpWusbRWError(int error, char * msg, int msgSize);

/**
 * \brief Function to enable/disable periodical tick-ins via a SpW-USB brick
 * \param hDevice	The SpW-USB brick to send/stop sending SpW tick-ins
 * \param enable	Whether enable or disable tick-ins
 * \param hertz		Frequency of the tick-ins
 * \return 0 if correct, a negative value if error occurred
 */
int PeriodicalTickInsSpWusb (void * hDevice, int enable, unsigned int hertz);
/**
 * \brief Function to send a single tick-ins via a SpW-USB brick
 * \param hDevice	The SpW-USB brick to send the SpW tick-ins
 * \param value		Value to write in the tick-in
 * \return 0 if correct, a negative value if error occurred
 */
int SingleTickInSpWusb (void * hDevice, unsigned char value,
	unsigned char external);

/**
 * \brief Function to enable or disable EEP in SpW packet
 * \param hDevice			The SpW-USB brick to retrieve information about
 * \param enableDisable		Whether enable or disable EEP
 */
void EnableDisableEEPSpWusb(void * hDevice, char enableDisable);

int FlushSpWusb (unsigned char * rxPacket, unsigned int * pLength, void * hDevice);

#ifdef __cplusplus
}
#endif
#endif  /* __rawSpW_H__ */