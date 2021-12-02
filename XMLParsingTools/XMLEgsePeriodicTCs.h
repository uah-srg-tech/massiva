/**
 * \file	XMLEgsePeriodicTC.h
 * \brief	functions for parsing XML periodic TCs (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/04/2013 at 14:18:34
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLEgsePeriodicTC_H__
#define __XMLEgsePeriodicTC_H__

#include <libxml/xmlreader.h>
#include "../definitions.h"					/* gss_config, portConfig */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to parse a periodic TC from a XML element 
 * \param	periodicTC_handle	Handle to the XML periodic TC element
 * \param	defaultLevels		GSS config levels
 * \param[out]	periodicTC		Pointer to input to be filled
 * \param	attrData			Auxiliary array from parent function
 * \param	XML_DIR				Auxiliary file string
 * \return	0 if correct, a negative value if error occurred
 */
int ParsePeriodicTC(xmlNodePtr periodicTC_handle, input * periodicTC,
	char * periodicTCname, level defaultLevels[MAX_INTERFACES][MAX_LEVELS],
	portConfig * ports, unsigned int * levelErrorRef, char attrData[ATTR_SIZE],
        const char * relativePath);

/**
 * \brief Function which parses errors produced while parsing a XML test
 * \brief procedure file and copies an error message into a string
 * \param	status			Test procedure parsing status error to be parsed
 * \param[out]	msg			Character string to copy message error into
 * \param	lenOrError		Length of the name of file which caused the error
 * \param	lenOrError		or error if it was in test procedure XML file
 * \param	levelErrorType	Information about the type of level of the error
 */
void DisplayParsePeriodicTCError (int status, int errorPeriodicTC, char * msg,
        unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLEgsePeriodicTC_H__ */