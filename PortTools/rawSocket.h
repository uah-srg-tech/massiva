/**
 * \file	rawSocket.h
 * \brief	functions for write and read data in socket port (declaration)
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
#ifndef __rawSocket_H__
#define __rawSocket_H__

#include "../definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

int ReadRawSocket(unsigned char * packet, unsigned int * pLength, 
        int * pSocket, protocolConfig * pPtcl);

int WriteRawSocket(const unsigned char * packet, int length, int socket);

void RawSocketRWError(int error, char * msg, int msgSize);


#ifdef __cplusplus
}
#endif
#endif  /* __rawSocket_H__ */