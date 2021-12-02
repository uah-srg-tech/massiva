/**
 * \file	XMLSpecialPacketTools.h
 * \brief	functions for parsing XML special packets (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/07/2014 at 16:26:15
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLSpecialPacketTools_H__
#define __XMLSpecialPacketTools_H__

#include <libxml/xmlreader.h>
#include "../definitions.h"		/* level */

#ifdef __cplusplus
extern "C" {
#endif

int ParseSpecialPacket(xmlNodePtr spPacket_handle, output * specialPacket,
	level defaultLevels[MAX_INTERFACES][MAX_LEVELS], portConfig * ports,
	special_packet_info * specialInfo, unsigned int * levelErrorRef,
	char attrData[ATTR_SIZE], const char * relativePath);

void DisplayParseSpecialPacketError (int status, int packetError, char * msg,
        int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLSpecialPacketTools_H__ */