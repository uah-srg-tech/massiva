/**
 * \file	configSocket.h
 * \brief	
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
#ifndef __configSocket_H__
#define __configSocket_H__

#if(defined _WIN32 || __CYGWIN__)
#include <psdk_inc/_ip_types.h>
#elif(defined __linux__)
#include <sys/socket.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
    ACCEPT_NOT_POSSIBLE_SERVER_CLOSED = -100
};
    
int ConfigureSocketServer(unsigned int portNum, struct sockaddr_in * pAddress);
int AcceptSocketServer(struct sockaddr_in * pAddress, int * pAcceptedSocket);

int ConfigureSocketClient(unsigned int portNum, const char ipAddress[16], 
        int * pConnectedSocket);

void DisplaySocketError(int error, char * msg, int msgSize);

int CloseSocket(int * pConnectedSocket);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __configSocket_H__ */