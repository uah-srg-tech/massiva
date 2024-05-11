/**
 * \file	rawProtocol.h
 * \brief	functions for write and read data packets with a raw protocol
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		10/03/2021 at 15:39:42
 * Company:		Space Research Group, Universidad de Alcalᮍ
 * Copyright:	Copyright (c) 2021, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __rawProtocol_H__
#define __rawProtocol_H__

#include <pthread.h>
#include "../definitions.h"	/* protocolConfig, portConfig */

#ifdef __cplusplus
extern "C" {
#endif
extern pthread_mutex_t ptclMutex[MAX_INTERFACES];

int ReadRawProtocol(unsigned char * packet, unsigned int * pLength,
        protocolConfig * pPtcl);
/**
 * \brief Function to be launched by raw polling thread
 * \brief for performing an infinite data request at serial port to making
 * \brief serial read blocking and packet-oriented
 * \param       ports		Pointer to ports structure
 */
int readProtocolPacketsBuffer(portConfig * ports);

#ifdef __cplusplus
}
#endif
#endif  /* __rawProtocol_H__ */