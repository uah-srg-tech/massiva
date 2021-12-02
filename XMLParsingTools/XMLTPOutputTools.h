/**
 * \file	XMLTPOutputTools.h
 * \brief	functions for parsing XML test outputs (declaration)
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
#ifndef __XMLTPOutputTools_H__
#define __XMLTPOutputTools_H__

#include <libxml/xmlreader.h>
#include "../definitions.h"				/* stepStruct, levelType, portConfig */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to parse a test procedure output from a XML element 
 * \param	output_handle		Handle to the XML test procedure output element
 * \param	defaultLevels		GSS config levels
 * \param[out]	currentOutput           Pointer to output to be filled
 * \param[out]	levelErrorType          Error level info to be filled if error
 * \param[out]	filename                File info to be parsed
 * \param	attrData		Auxiliary array from parent function
 * \param	XML_DIR			Auxiliary file string
 * \return	0 if correct, a negative value if error occurred
 */
int ParseTestOutput(xmlNodePtr output_handle, unsigned int io,
	level defaultLevels[MAX_INTERFACES][MAX_LEVELS], output * currentOutput,
	levelTypeError * levelErrorType, portConfig * ports, char filename[130],
	int * levelErrorRef, char attrData[ATTR_SIZE], int outputsCheckmode,
        portProtocol * protocols);

/**
 * \brief Function which parses errors produced while parsing a XML test
 * \brief procedure file and copies an error message into a string
 * \param	status			Test procedure parsing status error to be parsed
 * \param[out]	msg			Character string to copy message error into
 * \param	nameLen			Length of the name of file which caused the error
 * \param	levelErrorType	Information about the type of level of the error
 */
void DisplayParseTestOutputError(int status, levelTypeError levelError,
        char * msg, unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLTPOutputTools_H__ */