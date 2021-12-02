/**
 * \file	XMLProtocolPacketTools.h
 * \brief	functions for parsing XML level files (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		26/07/2017 at 12:27:12
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2017, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLProtocolPacketTools_H__
#define __XMLProtocolPacketTools_H__

#include <libxml/xmlreader.h>
#include "../definitions.h"		/* level */

#ifdef __cplusplus
extern "C" {
#endif

int ParseProtocolPackets(gss_config * pConfig, char attrData[ATTR_SIZE],
        const char * relativePath);
void DisplayParseProtocolPacketError (int status, char * msg,
        unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLProtocolPacketTools_H__ */