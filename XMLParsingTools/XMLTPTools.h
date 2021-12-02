/**
 * \file	XMLTPTools.h
 * \brief	functions for parsing XML test procedure files (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		07/03/2013 at 17:32:12
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLTPTools_H__
#define __XMLTPTools_H__

#include "../definitions.h"				/* gss_config, stepStruct, portConfig */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to parse a file where a XML test procedure is defined 
 * \param	filename			File where the XML test procedure is defined
 * \param	config				GSS config struct
 * \param[out]	steps			Pointer to steps array to be filled
 * \param[out]	numberOfSteps	Pointer to number of steps
 * \return	0 if correct, a negative value if error occurred
 */
int ParseTestProcedure(const char * filename, gss_config * config,
	portProtocol * protocols, portConfig * ports, stepStruct ** steps,
        unsigned int * pNumberOfSteps, processStatus * pParsing);

/**
 * \brief Function which parses errors produced while parsing a XML test
 * \brief procedure file and copies an error message into a string
 * \param	status		Test procedure parsing status error to be parsed
 * \param[out]	msg		Character string to copy message error into
 */
void DisplayParseTestProcedureError(int status, char * msg,
        unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLTPTools_H__ */