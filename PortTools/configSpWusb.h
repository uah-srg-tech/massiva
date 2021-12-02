/**
 * \file	configSpWusb.h
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		22/03/2012 at 16:06:42
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
//#define NOT_USB_DEV

#ifndef __configSpWusb_H__
#define __configSpWusb_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NOT_USB_DEV  
int ListSpWusbDevices(void);

int OpenConfigureSpWusbDevice(void ** phDevice, int link);

void DisplaySpWusbError(int error, char * msg, int msgSize);

int UnconfigCloseSpWusbDevice(void * hDevice);
/**
 * \brief Function to wait get the timeout of a SpW-USB brick
 * \param hDevice	The SpW-USB brick to get the timeout
 * \return Returns the timeout in seconds
 */
double GetSpWusbTimeout (void * hDevice);

void SetSpWusbTimeout (void * hDevice, double sTimeout);

#endif  /* NOT_USB_DEV */
#ifdef __cplusplus
}
#endif
#endif  /* ndef __configSpWusb_H__ */