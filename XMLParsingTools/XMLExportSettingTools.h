/**
 * \file	XMLExportSettingTools.h
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
#ifndef __XMLExportSettingTools_H__
#define __XMLExportSettingTools_H__

#include "../definitions.h"		/* sizesType, exportStruct, formatField */
#include <libxml/xmlreader.h>	/* xmlNodePtr */

#ifdef __cplusplus
extern "C" {
#endif
#define ATTR_EXPORT_SIZE    500
    
/**
 * \brief Function which to parse the settings element from a file where a XML
 * \brief export is defined
 * \param	settingsRoot				Handle to the XML settings element 
 * \param	sizes						Sizes array defined in export file
 * \param[out]	exportField				Export array to be filled
 * \param	targetFields				Fields of source level
 * \param	numberOfTargetFields		Number of fields of source level
 * \param	sourceFields				Fields of target level
 * \param	numberOfSourceFields		Number of fields of target level
 * \param	attrData					Auxiliary array from parent function
 * \param	XML_DIR						Auxiliary file string
 * \param	errorNames					Char string to save extra info of errors
 * \return	0 if correct, a negative value if error occurred
 */
int CreateExportSetting(xmlNodePtr setting, levelExport ** exportField,
	formatField * targetFields, unsigned int numberOfTargetFields,
	formatField * sourceFields, unsigned int numberOfSourceFields,
	char attrData[ATTR_EXPORT_SIZE], char errorNames[MAX_ERR_LEN], sizesType * sizes,
	int numberOfSizes, int index);

/**
 * \brief Function which parses the errors produced while parsing a XML export
 * \brief file and copies an error message into a string
 * \param	status			The XML export parsing status error to be parsed
 * \param[out]	msg			Character string to copy message error into
 * \param	beginAt			The position in which write in the msg string,
 * \param	beginAt			to avoid writing over the string size
 * \param	errorNames		Char string with extra info of errors
 */
void DisplayCreateExportSettingError (int status, char errorNames[MAX_ERR_LEN],
        char * msg, unsigned int maxMsgSize);

void closeXMLExportFileOpened (void);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLExportSizeTools_H__ */