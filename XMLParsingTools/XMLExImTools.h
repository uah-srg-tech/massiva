/**
 * \file	XMLExImTools.h
 * \brief	functions for parsing XML test procs import / export (declaration)
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
#ifndef __XMLExImTools_H__
#define __XMLExImTools_H__

#include "../definitions.h"		/* levelIn, levelOut */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to create an export array in a defined step, input and level
 * \brief selecting default fields or not default
 * \param	filename			File where the XML export is defined
 * \param[out]	Previous		Previous level (export to) in current input
 * \param[out]	Current			Current level (export from) in current input
 * \param	DefaultPrevious		Previous default level (export to)
 * \param	DefaultCurrent		Current default level (export from)
 * \return	0 if correct, a negative value if error occurred
 */
int CreateExportArraySelectDefault(const char * filename, const char * relativePath,
        levelIn * Previous, levelIn * Current,
        levelIn DefaultPrevious, levelIn DefaultCurrent);

/**
 * \brief Function to create an import array in a defined step, output and level
 * \brief selecting default fields or not default
 * \param	filename			File where the XML import is defined
 * \param	Previous			Previous level (import from) in current output
 * \param[out]	Current			Current level (import to) in current output
 * \param	DefaultPrevious		Previous default level (import from)
 * \param	DefaultCurrent		Current default level (import to)
 * \return	0 if correct, a negative value if error occurred
 */
int CreateImportArraySelectDefault(char * filename, const char * relativePath,
        levelOut Previous, levelOut * Current,
        levelOut DefaultPrevious, levelOut DefaultCurrent);

/**
 * \brief Function which parses errors produced while parsing a XML test
 * \brief procedure file and copies an error message into a string
 * \param	status			Test procedure parsing status error to be parsed
 * \param[out]	msg			Character string to copy message error into
 * \param	nameLen			Length of the name of file which caused the error
 * \param	levelErrorType	Information about the type of level of the error
 */
void DisplayParseTestProcImExError(int status, levelTypeError levelError,
        char * msg, unsigned int maxMsgSize);

void DisplayParsePeriodicTCExError(int status, levelTypeError levelError,
        char * msg, unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLExImTools_H__ */