/**
 * \file	configTCPSocket.h
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
#ifndef __configTCPSocket_H__
#define __configTCPSocket_H__

#if(defined _WIN32 || __CYGWIN__)
#include <psdk_inc/_ip_types.h>
#elif(defined __linux__)
#include <sys/socket.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
    ACCEPT_NOT_POSSIBLE_TCP_SERVER_CLOSED = -100
};

int ConfigureAddr(const char ipAddress[16], unsigned int port,
        struct sockaddr_in * pAddress);
	
int ConfigureTCPSocketServer(const char localIp[16], unsigned int localPort,
		struct sockaddr_in * pLocalAddr);
int AcceptTCPSocketServer(struct sockaddr_in * pLocalAddr, int * pAcceptedSocket);

int ConfigureTCPSocketClient(const char remoteIp[16], unsigned int remotePort,
		int * pConnectedSocket);

void DisplayTCPSocketError(int error, char * msg, int msgSize);

int CloseTCPSocket(int * pConnectedSocket);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __configTCPSocket_H__ */