/**
 * \file	XMLEgseMonitors.h
 * \brief	functions for parsing XML global variables (declaration)
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
#ifndef __XMLEgseMonitors_H__
#define __XMLEgseMonitors_H__

#include "../definitions.h"					/* gss_config */
#include <libxml/xmlreader.h>

#ifdef __cplusplus
extern "C" {
#endif

void ResetAlarmVals(void);

/**
 * \brief Function to parse a global variable from a XML element 
 * \param	monitor_handle		Handle to the XML global variable element
 * \param	periodicTCs			GSS periodic TCs
 * \param	attrData			Auxiliary array from parent function
 * \param	XML_DIR				Auxiliary file string
 * \return	0 if correct, a negative value if error occurred
 */
int ParseMonitor(xmlNodePtr monitor_handle, monitor * monitor,
	level defaultLevels[MAX_INTERFACES][MAX_LEVELS],
        globalVar * globalVars, unsigned int numberOfGlobalVars,
        chart * pCharts, unsigned int numberOfCharts, char attrData[ATTR_SIZE],
        const char * relativePath);

void ParseMonitorFilter(monitor * monitor, globalVar * globalVars,
	level levels[MAX_INTERFACES][MAX_LEVELS], input * periodicTCs,
        output * specialPackets);

/**
 * \brief Function which parses errors produced while parsing a XML test
 * \brief procedure file and copies an error message into a string
 * \param	status			Test procedure parsing status error to be parsed
 * \param[out]	msg			Character string to copy message error into
 * \param	errorMonitor	Error if it was in test procedure XML file
 */
void DisplayParseMonitorError (int status, int errorMonitor, char * msg,
        unsigned int maxMsgSize);

int ParseCharts(gss_config * pConfig, char attrData[ATTR_SIZE],
        const char * relativePath);

void DisplayParseChartsError (int status, char * msg, unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLEgseMonitors_H__ */