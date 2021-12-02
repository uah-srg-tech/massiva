/**
 * \file	XMLProcedureListTools.h
 * \brief	functions for procedure files list (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/01/2013 at 12:52:35
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLProcedureListTools_H__
#define __XMLProcedureListTools_H__

#include <libxml/xmlreader.h>
#include "../definitions.h"		/* config */

#ifdef __cplusplus
extern "C" {
#endif

int ParseProcedureList(xmlNodePtr root, gss_config * pConfig);

void DisplayParseProcedureListError(int status, char * msg, int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLProcedureListTools_H__ */