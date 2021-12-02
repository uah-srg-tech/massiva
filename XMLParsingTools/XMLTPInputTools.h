/**
 * \file	XMLTPInputTools.h
 * \brief	functions for parsing XML test inputs (declaration)
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
#ifndef __XMLTPInputTools_H__
#define __XMLTPInputTools_H__

#include <libxml/xmlreader.h>
#include "../definitions.h"				/* stepStruct, levelType, portConfig */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to parse a test procedure input from a XML element 
 * \param	input_handle		Handle to the XML test procedure input element
 * \param	defaultLevels		GSS config levels
 * \param[out]	currentInput            Pointer to input to be filled
 * \param[out]	levelErrorType          Error level info to be filled if error
 * \param[out]	filename                File info to be parsed
 * \param	attrData		Auxiliary array from parent function
 * \param	XML_DIR			Auxiliary file string
 * \return	0 if correct, a negative value if error occurred
 */
int ParseTestInput(xmlNodePtr input_handle,
	level defaultLevels[MAX_INTERFACES][MAX_LEVELS], input * currentInput,
	levelTypeError * levelError, portConfig * ports, char filename[130],
	int * levelErrorRef, char attrData[ATTR_SIZE]);

void DisplayParseTestInputError(int status, levelTypeError levelError,
        char * msg, unsigned int maxMsgSize);
#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLTPInputTools_H__ */