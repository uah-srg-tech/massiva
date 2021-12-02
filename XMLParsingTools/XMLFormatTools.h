/**
 * \file	XMLFormatTools.h
 * \brief	functions for parsing XML format files (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		18/02/2013 at 16:05:55
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLFormatTools_H__
#define __XMLFormatTools_H__

#include "../definitions.h"		/* formatField */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to parse a file where a XML format is defined 
 * \param	filename				File where the XML format is defined
 * \param[out]	fields				Pointer to fields array to be filled
 * \param[out]	numberOfFields		Pointer to number of fields
 * \param[out]	crcFieldRefs		Pointer to ref fields of crc array
 * \param[out]	numberOfcrcFields	Pointer to number of crc ref fields
 * \return	0 if correct, a negative value if error occurred
 */
int CreateFormatArray(const char * filename, const char * relativePath,
        formatField ** fields,
	unsigned int * numberOfFields, unsigned int * numberOfFDICFields,
	unsigned int *** crcFieldRefs, unsigned int ** numberOfcrcFields);
/**
 * \brief Function which parses the errors produced while parsing a XML format
 * \brief file and copies an error message into a string
 * \param	status		The XML format parsing status error to be parsed
 * \param[out]	msg		Character string to copy message error into
 * \param	beginAt		The position in which write in the msg string,
 * \param	beginAt		to avoid writing over the string size
 */
void DisplayCreateFormatArrayError(int status, char * msg,
        unsigned int maxMsgSize);

unsigned int getFormatFromXtext(char * data, unsigned int dataLen,
        formatField * LevelFields, unsigned int numberOfLevelFields,
        unsigned int * pFieldIdx);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLFormatTools_H__ */