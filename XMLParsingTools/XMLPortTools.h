/**
 * \file	XMLPortTools.h
 * \brief	functions for parsing XML port files (declaration)
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
#ifndef __XMLPortTools_H__
#define __XMLPortTools_H__

#include "../definitions.h"					/* portConfig */

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief Function to parse the file where the XML spw port is defined
 * \param	portConfigFile	File where the XML spw port is defined
 * \param[out]	port		A struct to save the port configuration
 * \return	0 if correct, a negative value if error occurred
 */
int ParsePort(char * portConfigFile, portConfig * pPort, serialConfig * serials,
        int index, const char * relativePath);

/**
 * \brief Function which parses errors produced while parsing a XML port
 * \brief file and copies an error message into a string
 * \param	status		The XML port parsing status error to be parsed
 * \param[out]	msg		Character string to copy message error into
 */
void DisplayParsePortError (int status, portConfig * pPort, char * msg,
        unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLPortTools_H__ */
