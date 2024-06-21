/**
 * \file	XMLTPStepTools.h
 * \brief	functions for parsing XML test procedure files (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/01/2014 at 15:30:23
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLTPStepTools_H__
#define __XMLTPStepTools_H__

#include <libxml/xmlreader.h>
#include "../definitions.h"				/* gss_config, stepStruct, portConfig,... */

#ifdef __cplusplus
extern "C" {
#endif

int ParseStep (xmlDocPtr doc, xmlNodePtr element, gss_config * config,
        portProtocol * protocols, portConfig * ports, stepStruct ** steps,
        unsigned int step, char fnameError[130], levelTypeError * levelError,
        processStatus * pParsing, int stepIdNotInFile,
        unsigned int numberOfSteps, xmlNodePtr elemParent);

void DisplayParseStepError (int status, int step, char fnameError[MAX_MSG_SIZE],
        levelTypeError levelError, char * msg, unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLTPStepTools_H__ */
