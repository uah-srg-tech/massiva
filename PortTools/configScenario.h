/**
 * \file	configScenario.h
 * \brief	
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		05/03/2012 at 17:48:22
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __configScenario__
#define __configScenario__

#include "../definitions.h"					/* portConfig */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to configure a port (serial / spw)
 * \param	portIndex	The number of port intended to be configured
 * \param[out]	ports		Pointer to ports handle structure
 * \param	pSerialConf	Pointer to serial ports config structure
 * \return	0 if correct, a negative value if error occurred
 */
int ConfigPort(unsigned int portIndex, portConfig * ports,
        serialConfig * pSerialConf);

/**
 * \brief Function which parses the errors produced when configuring a port
 * \brief and copies an error message into a string
 * \param	error		The status error to be parsed
 * \param	pPort		Port (interface) handle
 * \param[out]	msg		Character string to copy message error into
 * \param	msgSize 	Max size of the string to copy message error into
 */
void DisplayConfigPortError(int error, portConfig * pPort,
	char * msg, unsigned int msgSize);

/**
 * \brief Function to unconfigure port (serial / spw)
 * \param	portIndex	The port intended to be configured
 * \param	pPort		Port (interface) handle
 * \return	0 if correct, a negative value if error occurred
 */
int UnconfigPort(unsigned int portIndex, portConfig * pPort);
int UnconfigServer();

/**
 * \brief Function which parses the errors produced when unconfiguring a port
 * \brief and copies an error message into a string
 * \param	status		The status error to be parsed
 * \param	port		Port (interface) handle
 * \param[out]	msg		Character string to copy message error into
 * \param	msgSize 	Max size of the string to copy message error into
 */
void DisplayUnconfigPortError(int status, portConfig * pPort, char * msg,
        unsigned int msgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __configScenario__ */