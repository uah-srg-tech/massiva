/**
 * \file	GetSetFieldTools.h
 * \brief	functions for get fields from fields (declaration)
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		25/04/2013 at 16:50:30
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef GETSETFIELDTOOLS_H
#define GETSETFIELDTOOLS_H

#ifdef __cplusplus
extern "C" {
#endif
int GetFieldFromBuffer(const unsigned char * buffer,
	unsigned int initialPointer, unsigned int bufferMaxSize,
	int offsetRefInBits, int sizeRefInBits, unsigned char ** field);
/**
 * \brief Function to get the value of a buffer as an ullong
 * \param	buffer					The packet to check boolVars
 * \param	offsetRefInBits			The offset (in bits) of the desired field
 * \param	sizeRefInBits			The size (in bits) of the desired field
 * \param[out]	value				The variable to save the value into
 * \return	0 if correct, a negative value if error occurred
 */
int GetFieldFromBufferAsUllong(const unsigned char * buffer,
	unsigned int initialPointer, unsigned int bufferMaxSize,
	int offsetRefInBits, int sizeRefInBits, unsigned long long * value);
/**
 * \brief Function to get the value of a buffer as an uint32
 * \param	buffer					The packet to check boolVars
 * \param	offsetRefInBits			The offset (in bits) of the desired field
 * \param	sizeRefInBits			The size (in bits) of the desired field
 * \param[out]	value				The variable to save the value into
 * \return	0 if correct, a negative value if error occurred
 */
int GetFieldFromBufferAsUint(const unsigned char * buffer,
	unsigned int initialPointer, unsigned int bufferMaxSize,
	int offsetRefInBits, int sizeRefInBits, unsigned int * value);

/**
 * \brief Function to parse error while getting the value of a packet field
 * \param	status					Error status while getting the value
 * \param[out]	msg					Character string to copy message error into
 */
void ParseGetFieldError(int status, char * msg, int msgSize);

/**
 * \brief Function to set a constant integer data into a buffer field
 * \param	data					Constant integer data to be set
 * \param	targetField				The format struct of the field to be filled
 * \param	targetBuffer			The buffer to write the data into
 * \return	0 if correct, a negative value if error occurred
 */
int SettingNumber(unsigned long long data, unsigned char * targetBuffer,
        int totalSizeInBits, int offsetInBits);
    
void DisplaySettingNumberError (int status, char * msg, int msgSize);

#ifdef __cplusplus
}
#endif
#endif /* ndef GETSETFIELDTOOLS_H */