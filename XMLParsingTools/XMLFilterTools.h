/**
 * \file	XMLFilterTools.h
 * \brief	functions for parsing XML filter files (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/01/2013 at 12:52:35
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLFilterTools_H__
#define __XMLFilterTools_H__

#include "../definitions.h"		/* boolVar, filter */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to parse a file where a XML filter is defined 
 * \param	filename				File where the XML filter is defined
 * \param[out]	boolVars			Pointer to boolVars array to be filled
 * \param[out]	numberOfBoolVars	Pointer to number of boolVars
 * \param[out]	filters				Pointer to filters array to be filled
 * \param[out]	numberOfFilters		Pointer to number of filters
 * \param[out]	typeOfFilter		Type of filter of the XML
 * \return	0 if correct, a negative value if error occurred
 */
int CreateFilterArray (const char * filename, const char * relativePath,
        boolVar ** boolVars, unsigned int * pNumberOfBoolVars,
	filter ** filters, unsigned int * pNumberOfFilters,
	filterTypes * typeOfFilter, formatField * LevelFields,
	unsigned int numberOfLevelFields);
/**
 * \brief Function which parses errors produced while parsing a XML filter
 * \brief file and copies an error message into a string
 * \param	status		The XML filter parsing status error to be parsed
 * \param[out]	msg		Character string to copy message error into
 * \param	beginAt		The position in which write in the msg string,
 * \param	beginAt		to avoid writing over the string size
 */
void DisplayCreateFilterArrayError (int status, char * msg,
        unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLFilterTools_H__ */