/**
 * \file	configSpWusbmk2.h
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		10/07/2013 at 17:35:37
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
//#define NOT_MK2_DEV

#ifndef __configSpWusbmk2_H__
#define __configSpWusbmk2_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NOT_MK2_DEV

int ListSpWMk2Devices(void);

int OpenConfigureSpWMk2Device(unsigned int * pChannelId,
	unsigned int * pDeviceId);

void DisplaySpWMk2Error(int error, char * msg, int msgSize);

int UnconfigCloseSpWMk2Device(unsigned int * pChannelId);

/**
 * \brief Function to wait get the timeout of a SpW-USB MK2 brick
 * \param pChannel	Pointer to the channel of the MK2 to get the timeout
 * \return Returns the timeout in seconds
 */
double GetSpWMk2Timeout (void);
void SetSpWMk2Timeout (double sTimeout);

#endif  /* NOT_MK2_DEV */
#ifdef __cplusplus
}
#endif
#endif  /* ndef __configSpWusbmk2_H__ */