/**
 * \file	configSpW.h
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		17/07/2013 at 11:44:32
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __configSpW_H__
#define __configSpW_H__

#include "../definitions.h"					/* portConfig */

#ifdef __cplusplus
extern "C" {
#endif

int SelectOpenSpWDevice(portConfig * pPort);

void SelectDisplaySpWError(portConfig * pPort, int status, char * msg,
	int msgSize);

int SelectCloseSpWDevice(portConfig * pPort);

unsigned char GetNumberOfSpWDevices();

double GetSpWTimeout(portConfig * pPort);
void SetSpWTimeout(portConfig * pPort, double msTimeout);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __configSpW_H__ */
