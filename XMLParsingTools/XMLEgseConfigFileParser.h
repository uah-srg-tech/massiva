/**
 * \file	XMLEgseConfigFileParser.h
 * \brief	functions for parsing EGSE config file (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		06/06/2016 at 12:52:35
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLParseEgseConfigFile_H__
#define __XMLParseEgseConfigFile_H__

#include "../definitions.h"					/* gss_config, portConfig */

#ifdef __cplusplus
extern "C" {
#endif

int EgseConfigFileParser(xmlNodePtr root, gss_config * pConfig,
        portConfig * ports, serialConfig * serials, gss_options * pOptions,
        portProtocol * protocols, int configItems, char attrData[ATTR_SIZE],
        const char * relativePath, unsigned int testsInOtherFile);
void DisplayEgseConfigFileParserError (int status, portConfig * ports,
	char * msg, unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLParseEgseConfigFile_H__ */
