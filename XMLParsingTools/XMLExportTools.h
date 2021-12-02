/**
 * \file	XMLExportTools.h
 * \brief	functions for parsing XML export files (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		01/03/2013 at 17:24:02
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLExportTools_H__
#define __XMLExportTools_H__

#include "../definitions.h"		/* sizesType, exportStruct, formatField */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function which to parse a file where a XML export is defined
 * \param	filename					File where the XML export is defined
 * \param[out]	exportField				Pointer to export array to be filled
 * \param[out]	numberOfExportFields	Pointer to number of export fields
 * \param	targetFields				Fields of target level
 * \param	numberOfTargetFields		Number of fields of target level
 * \param	sourceFields				Fields of source level
 * \param	numberOfSourceFields		Number of fields of source level
 * \return	0 if correct, a negative value if error occurred
 */
int CreateExportArray (const char * filename, const char * relativePath,
        levelExport ** exportField, unsigned int * numberOfExportFields,
	unsigned int ** DICs, unsigned int * numberOfDICs,
	formatField * targetFields, unsigned int numberOfTargetFields,
	formatField * sourceFields, unsigned int numberOfSourceFields,
	unsigned int numberOfTargetFDIC);
/**
 * \brief Function which parses the errors produced while parsing a XML export
 * \brief file and copies an error message into a string
 * \param	status		The XML export parsing status error to be parsed
 * \param[out]	msg		Character string to copy message error into
 * \param	beginAt		The position in which write in the msg string,
 * \param	beginAt		to avoid writing over the string size
 */
void DisplayCreateExportArrayError (int status, char * msg,
        unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLExportTools_H__ */