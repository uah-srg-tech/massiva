/**
 * \file	ImportFields.h
 * \brief	functions for exporting info from import struct (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		04/03/2013 at 09:33:21
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __ImportFields_H__
#define __ImportFields_H__

#include "../definitions.h"		/* importVirtual, formatField */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to import the information to the import struct
 * \brief from the buffer
 * \param	buffer					The buffer with the current info
 * \param	virtual					Pointer to virtual import fields
 * \param	numberOfImportVirtual	Number of virtual import fields
 * \param	targetFields			The format struct of the target level
 * \param	sourceFields			The format struct of the source level
 * \return	0 if correct, a negative value if error occurred
 */
int importVirtualFields (const unsigned char * buffer,
	importVirtual * virtualFld, int numberOfImportVirtualFld,
	formatField * sourceFields, formatField * targetFields);
/**
 * \brief Function to parse error while importing information
 * \param	status					Error status while importing information
 * \param[out]	msg					Character string to copy message error into
 */
void DisplayImportFieldsError (int status, char * msg, int msgSize);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __ImportFields_H__ */