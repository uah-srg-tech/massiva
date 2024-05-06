/**
 * \file	configUDPSocket.h
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
#ifndef __configUDPSocket_H__
#define __configUDPSocket_H__

#if(defined _WIN32 || __CYGWIN__)
#include <psdk_inc/_ip_types.h>
#elif(defined __linux__)
#include <sys/socket.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

int ConfigureUDPSocket(const char localIp[16], unsigned int localPort, 
        struct sockaddr_in * pLocalAddr, const char remoteIp[16],
        unsigned int remotePort, struct sockaddr_in * pRemoteAddr,
        int * pSocket);

void DisplayUDPSocketError(int error, char * msg, int msgSize);

int CloseUDPSocket(int * pSocket);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __configUDPSocket_H__ */