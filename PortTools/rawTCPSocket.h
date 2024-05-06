/**
 * \file	rawTCPSocket.h
 * \brief	functions for write and read data in TCP socket port (declaration)
 *
 * \author	Aaron Montalvo, <aaron.montalvo@uah.es>
 * 
 * \internal
 * Created:		03/03/2021 at 13:35:17
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2021, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __rawTCPSocket_H__
#define __rawTCPSocket_H__

#include "../definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

int ReadRawTCPSocket(unsigned char * packet, unsigned int * pLength, 
        int * pSocket, protocolConfig * pPtcl);

int WriteRawTCPSocket(const unsigned char * packet, int length, int socket);

void RawTCPSocketRWError(int error, char * msg, int msgSize);


#ifdef __cplusplus
}
#endif
#endif  /* __rawTCPSocket_H__ */