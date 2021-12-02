/**
 * \file	XMLLevelTools.h
 * \brief	functions for parsing XML level files (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		21/03/2013 at 10:44:12
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLLevelTools_H__
#define __XMLLevelTools_H__

#include <libxml/xmlreader.h>
#include "../definitions.h"		/* level */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to parse a level defined in a XML format file
 * \param	interf				Handle to the XML level element 
 * \param[out]	currentLevel	Array to fill with the current level information
 * \param	previousLevel		Array with the previous level information
 * \param	levelNumber			Current level number
 * \param	XML_DIR				Auxiliar file string
 * \return	0 if correct, a negative value if error occurred
 */
int ParseLevel(xmlNodePtr interf, level * currentLevel, level * previousLevel,
        unsigned int levelNumber, unsigned int elementNumber, ioTypes portType,
        const char * relativePath);

/**
 * \brief Function which parses the errors produced while parsing
 * \brief EGSE config file and copies an error message into a string
 * \param	status		The XML EGSE config parsing status error to be parsed
 * \param[out]	msg		Character string to copy message error into
 * \param	levelError	Current level when error produced
 */
void DisplayParseLevelError (int status, int port, int level, 
	char * msg, unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLLevelTools_H__ */