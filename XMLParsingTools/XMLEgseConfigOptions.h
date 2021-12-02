/**
 * \file	XMLEgseConfigOptions.h
 * \brief	functions for parsing EGSE config file (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/01/2014 at 16:21:45
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLEgseConfigOptions_H__
#define __XMLEgseConfigOptions_H__

#include "../definitions.h"			/* gss_config */
#include <libxml/xmlreader.h>

#ifdef __cplusplus
extern "C" {
#endif

int GetGSSConfigOptions(xmlNodePtr root, xmlNodePtr option,
	unsigned int * SpecialPackets, unsigned int * PeriodicTCs,
        unsigned int * GlobalVarsMonitors, unsigned int configItems,
        unsigned int testsInOtherFile);
void DisplayGetGSSConfigOptionsError(int status, char * msg,
        unsigned int maxMsgSize);

int CheckParseGSSInfoHeader(xmlNodePtr root, gss_options * pOptions);
void DisplayCheckParseGSSInfoHeaderError(int status, char * msg,
        unsigned int maxMsgSize);

int CheckParseGSSProtocols(xmlNodePtr root, portProtocol * protocols,
        unsigned int * pNumberOfProtocols, char attrData[ATTR_SIZE]);
void DisplayCheckParseGSSProtocolsError(int status, char * msg,
        unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLEgseConfigOptions_H__ */