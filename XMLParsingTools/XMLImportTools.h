/**
 * \file	XMLImportTools.h
 * \brief	functions for parsing XML import files (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/01/2013 at 10:46:22
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLImportTools_H__
#define __XMLImportTools_H__

#include <libxml/xmlreader.h>
#include "../definitions.h"		/* importStruct, formatField */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to parse a file where a XML import is defined 
 * \param	filename					File where the XML import is defined
 * \param[out]	import					Import index (position) to be filled
 * \param[out]	virtual					Import virtual fields array to be filled
 * \param[out]	numberOfImportVirtual	Number of import virtual fields
 * \param	sourceFields				Fields of source level
 * \param	numberOfSourceFields		Number of fields of source level
 * \param	targetFields				Fields of target level
 * \param	numberOfTargetFields		Number of fields of target level
 * \return	0 if correct, a negative value if error occurred
 */
int CreateImportArray (const char * filename, const char * relativePath,
        unsigned int * import,
        importVirtual ** virtual, int * numberOfImportVirtual,
	formatField * sourceFields, unsigned int numberOfSourceFields, 
	formatField * targetFields, unsigned int numberOfTargetFields);
/**
 * \brief Function which parses the errors produced while parsing a XML import
 * \brief file and copies an error message into a string
 * \param	status		The XML import parsing status error to be parsed
 * \param[out]	msg		Character string to copy message error into
 * \param	beginAt		The position in which write in the msg string,
 * \param	beginAt		to avoid writing over the string size
 */
void DisplayCreateImportArrayError (int status, char * msg,
        unsigned int maxMsgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLImportTools_H__ */