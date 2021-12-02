/**
 * \file	CheckFilterTools.h
 * \brief	functions for checking boolvars and filters (declaration)
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		15/02/2013 at 15:57:35
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __CheckFilterTools_H__
#define __CheckFilterTools_H__

#include <stdint.h>			/* uint8_t */
#include "../definitions.h"		/* formatField, boolVar, filter */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Function to check boolVars of packets in a given level
 * \param	buffer					The packet to check boolVars 
 * \param   prevMsg					The previous msg to be written (step info)
 * \param	boolVars				The boolVars of the desired level
 * \param	numberOfBoolVars		The number of boolVars of the desired level
 * \param	LevelFields				The format struct of the desired level
 * \param	numberOfLevelFields		The number of fields of the desired level
 * \return	0 if correct, a negative value if error occurred
 */
int CheckBoolVars(const uint8_t * buffer, uint64_t * numbers,
	uint8_t * strings[MAX_STR_OPERAND_LEN],
	boolVar * boolVars, unsigned int numberOfBoolVars,
	formatField * LevelFields, unsigned int numberOfLevelFields, 
	unsigned int ** crcFieldRefs, const unsigned int * numberOfcrcFields);
/**
 * \brief Function to parse error while checking boolVars 
 * \param	status					Error status while checking boolVars 
 * \param[out]	msg					Character string to copy message error into
 */
void ParseCheckBoolVarError(int status, char * msg, int msgSize);

/**
 * \brief Function to check filters of packets in a given level
 * \param	filters					The filters of the desired level
 * \param	typeOfFilter			The type of filter of the desired level
 * \param	numberOfFilters			The number of filters of the desired level
 * \param	boolVars				The boolVars of the desired level
 * \return	result of all filters (1 if passed, 0 if not),
 * \return	or a negative value if error occurred
 */
unsigned short CheckFilters(filter * filters, int typeOfFilter, 
	unsigned int numberOfFilters, boolVar * boolVars, int * errorAt);

/**
 * \brief Function to parse error while checking filters
 * \param	status					Error status while checking filters
 * \param[out]	msg					Character string to copy message error into
 */
void ParseCheckFilterError(int status, char * msg, int msgSize);

#ifdef __cplusplus
}
#endif
#endif /* ndef __CheckFilterTools_H__ */