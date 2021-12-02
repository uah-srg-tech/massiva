/**
 * \file	crc16.c created for crc16 project
 * \brief
 *	
 * \author	Alberto Carrasco Gallardo, <acarrasco@srg.aut.uah.es>
 * \author	Oscar Rodriguez Polo, <opolo@aut.uah.es>
 * \author  Pablo Parra Espada, <pablo.parra@aut.uah.es>
 * 
 * \internal
 * Created: 	30/04/2012
 * Compiler: 	gcc/g++
 * Company:  Space Research Group, Universidad de Alcal‡.
 * Copyright: Copyright (c) 2012, Alberto Carrasco Gallardo
 *			  Copyright (c) 2012, Oscar Rodriguez Polo
 * 			  Copyright (c) 2012, Pablo Parra Espada
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 * This software is provided under the terms of the GNU General Public v2
 * Licence. A full copy of the GNU GPL is provided in the file COPYING
 * found in the development root of the project.
 * 	
 */
#include <stdint.h>				/* uint16_t */

/*!
 * CRC table
 */
uint16_t CRCWord16Table[256];

/**
 * initialize the CRC table
 */
void init_CRC_word16_table(void)
{
	uint16_t key16=0x1021;
	unsigned int i = 0;
	int j=0;
	
	for (i = 0; i < 256; ++i)
	{
		uint16_t reg = i << 8;
		
		/* for all bits in a byte */
		for (j = 0; j < 8; ++j)
		{
			reg = reg & 0x8000 ? (reg << 1) ^ key16 : reg << 1;
		}
		
		CRCWord16Table[i] = reg;
	}
	
	return;
}

/**
 * function to generate the CRC setting the seed
 */
uint16_t set_seed_and_calculate_CRC_word16(uint16_t _numBytes,
	const uint8_t *_pData, uint16_t seed)
{
	uint16_t index;
	register uint16_t tempCRC;
	uint8_t top;
	
	/* Init with 0xFFFF */
	tempCRC=seed;
	
	/* Calculate CRC */
	for(index=0; index< _numBytes;index++)
	{
		top = tempCRC >> 8;
		top ^= *(_pData+index);
		tempCRC = (tempCRC << 8)^CRCWord16Table[top];
	}
	return tempCRC;
}

/**
 * function to generate the CRC
 */
uint16_t calculate_CRC_word16(uint16_t _numBytes, const uint8_t *_pData)
{
	return set_seed_and_calculate_CRC_word16(_numBytes, _pData, 0xFFFF);
}

/**
 * function to check the CRC
 */
int check_CRC_word16(uint16_t _numBytes, const uint8_t * _pData,
	uint16_t crc)
{
	if(crc == calculate_CRC_word16(_numBytes,_pData))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint16_t set_seed_and_calculate_checksum_word16(uint16_t _numBytes,
	const uint8_t * _pData, uint16_t seed)
{
	uint16_t checksum = 0, index = 0;
	
	if(_numBytes%2)
	{
	    checksum = _pData[0];
	    _numBytes -= 1;
	    index += 1;
	}
	while(_numBytes>0)
	{
	    checksum = (((_pData[index]<<8) + _pData[index+1])) ^ checksum;
		//get two bytes at a time and add previous calculated checksum value
	    _numBytes -= 2; //decrease by 2 for 2 byte boundaries
	    index += 2;
	}
	checksum ^= seed;
	return (checksum);
}

/**
 * function to generate the CRC
 */
uint16_t calculate_checksum_word16(uint16_t _numBytes, const uint8_t *_pData)
{
	return set_seed_and_calculate_checksum_word16(_numBytes, _pData, 0);
}
