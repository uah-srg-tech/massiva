/**
 * \file	rawSpWpci.h
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		30/07/2013 at 16:09:38
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __rawSpWpci_H__
#define __rawSpWpci_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to read data from a SpW PCI-2 board
 * \param[out] receivedPacket	The buffer in which read data will be stored
 * \param length				The number of bytes to read
 * \param hDevice				The SpW PCI-2 board in which read data
 * \return 0 if correct, a negative value if error occurred
 */
int ReadRawSpWpci(unsigned char * receivedPacket, int * pLength, void * hDevice,
	unsigned char transmitLink);
/**
 * \brief Function to write data to a SpW PCI-2 board
 * \param sendPacket			The buffer where is stored the sending data
 * \param length				The number of bytes to write
 * \param hDevice				The SpW PCI-2 board to which write data
 * \return Returns the status (0 if correct, < 0 if error)
 */
int WriteRawSpWpci(const unsigned char * sendPacket, int length, void * hDevice,
	unsigned char transmitLink);
/**
 * \brief Function which parses the errors produced while reading or writing data
 * \brief from/to a SpW port
 * \param error		The raw read/write status error to be parsed
 * \param[out] msg	The string to copy the error message in
 */
void RawSpWpciRWError(int error, char * msg, int msgSize, void * hDevice,
	unsigned char transmitLink);

/**
 * \brief Function to enable/disable sending periodical tick-ins via a SpW device
 * \param hDevice	The SpW device to send/stop sending SpW tick-ins
 * \param enable	Whether enable or disable tick-ins
 * \param hertz		Frequency of the tick-ins
 * \return Returns the status (0 if correct, < 0 if error)
 */
int PeriodicalTickInsSpWpci (void * hDevice, int enable, unsigned int hertz);
/**
 * \brief Function to send a single tick-ins via a SpW device
 * \param hDevice	The SpW device to send the SpW tick-ins
 * \return Returns the status (0 if correct, < 0 if error)
 */
int SingleTickInSpWpci (void * hDevice, unsigned char value,
	unsigned char external);

/**
 * \brief Function to enable or disable EEP in SpW packet
 * \param hDevice			The SpW PCI board to retrieve information about
 * \param enableDisable		Whether enable or disable EEP
 */
void EnableDisableEEPSpWpci(void * hDevice, char enableDisable,
	unsigned char transmitLink);

int FlushSpWpci (unsigned char * rxPacket, unsigned int * pLength,
	void * hDevice, unsigned char transmitLink);

#ifdef __cplusplus
}
#endif
#endif  /* __rawSpWpci_H__ */