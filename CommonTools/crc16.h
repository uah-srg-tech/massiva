/**
 * \file	crc16.h created for crc16 project
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

#ifndef CRC16_H_
#define CRC16_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>				/* uint16_t */

/**
 * initialize the CRC table
 */
void init_CRC_word16_table(void);//OK

/**
 * function to set seed and generate the CRC
 */
uint16_t set_seed_and_calculate_CRC_word16(uint16_t _numBytes,
	const uint8_t *_pData, uint16_t seed);

/**
 * function to generate the CRC
 */
uint16_t calculate_CRC_word16(uint16_t _numBytes, const uint8_t * _pData);//OK

/**
 * function to check the CRC
 */
int check_CRC_word16(uint16_t _numBytes, const uint8_t * _pData,
	uint16_t crc);

uint16_t set_seed_and_calculate_checksum_word16(uint16_t _numBytes,
	const uint8_t * _pData, uint16_t seed);
	
uint16_t calculate_checksum_word16(uint16_t _numBytes, const uint8_t *_pData);

#ifdef __cplusplus
}
#endif
#endif /* CRC16_H_ */