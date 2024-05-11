/**
 * \file	ExportSettingTools.h
 * \brief	functions for exporting info from export struct (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		04/03/2013 at 09:33:21
 * Compiler:	LabWindows / CVI 10.0.0.0
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __ExportSettingTools_H__
#define __ExportSettingTools_H__

#include "../definitions.h"		/* formatField, export, ... */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * The enumeration to know what type of export perform: (const or all fields) 
 */
typedef enum {
	EXPORT_CONST,
	EXPORT_ALL
} typeOfExport;

/**
 * \brief Function to export the information from the export struct
 * \brief to the buffer
 * \param	numberOfExportFields	The number of fields to export at this level
 * \param	exportField				The export struct
 * \param	targetFields			The format struct of the target level
 * \param	targetBuffer			The target buffer to copy data to
 * \param	sourceFields			The format struct of the source level
 * \param	sourceBuffer			The source buffer to copy data from
 * \param	type					Type of export (only const or all fields) 
 * \return	0 if correct, a negative value if error occurred
 */
int exportInformation(typeOfExport type, levelExport * exportFields,
        unsigned int numberOfExportFields, formatField * targetFields,
        unsigned int numberOfTargetFields, unsigned char * targetBuffer,
        formatField * sourceFields, unsigned char * sourceBuffer);

/**
 * \brief Function to parse error while exporting information
 * \param	status					Error status while exporting information
 * \param[out]	msg					Character string to copy message error into
 */
void DisplayExportInformationError(int status, char * msg, int msgSize);

int performDICs(unsigned int numberOfActiveDICs, unsigned int * ActiveDICs,
	unsigned int ** crcFieldRefs, unsigned int * numberOfcrcFields,
	unsigned char * targetBuffer, formatField * targetFields);

void DisplayPerformDICsError(int status, char * msg, int msgSize);
	
#ifdef __cplusplus
}
#endif
#endif  /* ndef __ExportSettingTools_H__ */