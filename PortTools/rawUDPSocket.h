/**
 * \file	rawUDPSocket.h
 * \brief	functions for write and read data in UDP socket port (declaration)
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
#ifndef __rawUDPSocket_H__
#define __rawUDPSocket_H__

#include "../definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

int ReadRawUDPSocket(unsigned char * packet, unsigned int * pLength, 
        int * pSocket);

int WriteRawUDPSocket(const unsigned char * packet, int length, int socket,
        struct sockaddr_in remoteAddress);

void RawUDPSocketRWError(int error, char * msg, int msgSize);


#ifdef __cplusplus
}
#endif
#endif  /* __rawUDPSocket_H__ */