/**
 * \file	configSpWpci.h
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		30/07/2013 at 16:09:38
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __configSpWpci_H__
#define __configSpWpci_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NOT_PCI_DEV
extern unsigned char pciClosed;

int ListSpWpciDevices(void);

int OpenConfigureSpWpciDevice (void ** phDevice, int transmitLink);

void DisplaySpWpciError(int error, char * msg, int msgSize);

int UnconfigCloseSpWpciDevice(void * hDevice, int transmitLink);

int ForceCloseSpWpciDevice(void * hDevice);

/**
 * \brief Function to wait get the timeout of a SpW device
 * \param hDevice	The SpW device to get the timeout
 * \return Returns the timeout
 */
double GetSpWpciTimeout (void * hDevice);

void SetSpWpciTimeout (void * hDevice, double sTimeout);

#endif  /* NOT_PCI_DEV */
#ifdef __cplusplus
}
#endif
#endif  /* ndef __configSpWpci_H__ */