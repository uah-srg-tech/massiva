/**
 * \file	rawSpWmk2.h
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		09/05/2013 at 17:24:55
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __rawSpWmk2_H__
#define __rawSpWmk2_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to read data from SpW-USB MK2 brick
 * \param[out] receivedPacket	The buffer in which read data will be stored
 * \param length				The number of bytes to read
 * \return Returns the status (0 if correct, < 0 if error)
 */
int ReadRawSpWmk2(unsigned char * receivedPacket, unsigned int * pLength,
	unsigned int * pChannel);

/**
 * \brief Function to write data to a SpW-USB MK2 brick
 * \param sendPacket		The buffer where is stored the sending data
 * \param length			The number of bytes to write
 * \param pChannel			Pointer to the channel of the MK2 to write data
 * \return Returns the status (0 if correct, < 0 if error)
 */
int WriteRawSpWmk2(const unsigned char * sendPacket, int length,
	unsigned int * pChannel);
/**
 * \brief Function which parses the errors produced while reading / writing data
 * \brief from/to a SpW port
 * \param error		The raw read/write status error to be parsed
 * \param[out] msg	The string to copy the error message in
 * \param msgSize	Maximum size of the string to copy the error message in
 */
void RawSpWmk2RWError(int error, char * msg, int msgSize,
	unsigned int * pDeviceId);

/**
 * \brief Function to enable/disable sending periodical tick-ins via a
 * \brief SpW-USB MK2 brick
 * \param pChannel	Pointer to the channel of the MK2 to send/stop SpW tick-ins
 * \param enable	Whether enable or disable tick-ins
 * \param hertz		Frequency of the tick-ins
 * \return Returns the status (0 if correct, < 0 if error)
 */
int PeriodicalTickInsSpWmk2 (unsigned int * pDeviceId, int enable,
	unsigned int hertz);
/**
 * \brief Function to send a single tick-ins via a SpW-USB MK2 brick
 * \param pChannel	Pointer to the channel of the MK2 to send the SpW tick-ins
 * \param value		Value to write in the tick-in
 * \return Returns the status (0 if correct, < 0 if error)
 */
int SingleTickInSpWmk2 (unsigned int * pDeviceId, unsigned char value,
	unsigned char external);

/**
 * \brief Function to enable or disable EEP in SpW packet
 * \param enableDisable	Whether enable or disable EEP
 */
void EnableDisableEEPSpWmk2(char enableDisable);

int FlushSpWmk2 (unsigned char * rxPacket, unsigned int * pLength,
	unsigned int * pChannel);

int InjectErrorSpWMk2(unsigned int * pDeviceId, unsigned char port,
	unsigned char error);

#ifdef __cplusplus
}
#endif
#endif  /* __rawSpWmk2_H__ */
