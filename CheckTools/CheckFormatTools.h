/**
 * \file	CheckFormatTools.h
 * \brief	functions for checking formats (declaration)
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		15/02/2013 at 15:57:35
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __CheckFormatTools_H__
#define __CheckFormatTools_H__

#include <stdint.h>                     /* uint8_t */
#include "../definitions.h"		/* formatField, boolVar */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to check format of packets with a given field format
 * \param	buffer					The packet to check format 
 * \param	LevelFields				The format struct of the desired level
 * \param	numberOfLevelFields		The number of fields of the desired level
 * \param	crcFieldRefs			The reference fields to make the crc
 * \param	numberOfcrcFields		The number of crc fields of the packet
 * \param[out]	length				The length of the format (in bytes)
 * \return	0 if correct, a negative value if error occurred
 */
int CheckFormat(const uint8_t * buffer, formatField * LevelFields,
	const unsigned int numberOfLevelFields, unsigned int * length, int port);
/**
 * \brief Function to parse error while checking format
 * \param	status					Error status while checking format 
 * \param[out]	msg					Character string to copy message error into
 */
void ParseCheckFormatError(int status, char * msg, int msgSize, int port);

#ifdef __cplusplus
}
#endif
#endif /* ndef __CheckFormatTools_H__ */