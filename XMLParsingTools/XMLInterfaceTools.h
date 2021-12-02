/**
 * \file	XMLInterfaceTools.h
 * \brief	functions for parsing XML export settings (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		25/04/2013 at 17:48:06
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLInterfaceTools_H__
#define __XMLInterfaceTools_H__

#include "../definitions.h"					/* portConfig */

#ifdef __cplusplus
extern "C" {
#endif

int ParseInterface(xmlNodePtr element, portConfig * ports,
        serialConfig * serials, char attrData[ATTR_SIZE], int index,
        const char * relativePath);

void DisplayParseInterfaceError (int status, portConfig * port, char * msg,
	unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLInterfaceTools_H__ */
