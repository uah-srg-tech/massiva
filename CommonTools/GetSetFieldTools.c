/**
 * \file	GetSetFieldTools.h
 * \brief	functions for get fields from fields (definition)
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
#include <stdio.h>			 	/* snprintf */
#include <stdlib.h>			 	/* calloc, free */
#include <string.h>			 	/* memcpy */
#include <math.h>			 	/* pow */

enum {
    ZERO_BUFFER_SIZE = -1,
    ERROR_ALLOCATING_MEMORY = -2,
    NOT_AN_UINT = -3,
    NOT_AN_ULLONG = -4
};

enum {
    NUMBER_TOO_BIG = -1,
};

static int leftMask[9] = {0xFF, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0x00};
static int rightMask[9] = {0x00, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0xFF};

/**
 * \brief Function to get the value of a buffer a char *
 * \param	buffer					The packet to check boolVars
 * \param	offsetRefInBits			The offset (in bits) of the desired field
 * \param	sizeRefInBits			The size (in bits) of the desired field
 * \param[out]	tempBuffer			The variable to save the value into
 * \return	0 if correct, a negative value if error occurred
 */

int GetFieldFromBuffer(const unsigned char * buffer,
	unsigned int initialPointer, unsigned int bufferMaxSize,
	int offsetRefInBits, int sizeRefInBits, unsigned char ** field)
{
    int offsetRefInBytes = 0, offsetSizeRefInBytes = 0, bitsInLastByte = 0;
    int oneByteBigger = 0, tempBufferSize = 0, idx = 0;
    unsigned char * tempBuffer = NULL;

    bitsInLastByte = offsetRefInBits + sizeRefInBits;
    offsetSizeRefInBytes = bitsInLastByte/8;
    bitsInLastByte %= 8;

    offsetRefInBytes = offsetRefInBits/8;
    offsetRefInBits %= 8;

    oneByteBigger = (bitsInLastByte == 0) ? 0 : 1; //whether last byte if full
    tempBufferSize = offsetSizeRefInBytes-offsetRefInBytes+oneByteBigger;
    if(tempBufferSize == 0)
    {
        if(sizeRefInBits == 0)
        {
            // for any reason, size parameter is 0
            // assume datasize = 1 byte, data = 0
            *field = calloc(1, sizeof(unsigned char));
            **field = 0;
            return 1;
        }
        else
        {
            return ZERO_BUFFER_SIZE;
        }
    }

    /* 1) allocate an appropriate temp buffer and copy data */
    tempBuffer = calloc(tempBufferSize, sizeof(unsigned char));
    if(tempBuffer == NULL)
    {
        return ERROR_ALLOCATING_MEMORY;
    }

    if((initialPointer+offsetRefInBytes+tempBufferSize) > bufferMaxSize)
    {
        /* A) circular array whose end has been reached */
        if((initialPointer+offsetRefInBytes+tempBufferSize) > bufferMaxSize)
        {
            /* A1) Offset is after the beginning of circular array */
            memcpy(tempBuffer,
                    &buffer[offsetRefInBytes-(bufferMaxSize-initialPointer)],
                    tempBufferSize);
        }
        else
        {
            /* A2) Offset is before the beginning of circular array */
            memcpy(tempBuffer, &buffer[initialPointer+offsetRefInBytes],
                    bufferMaxSize-initialPointer-offsetRefInBytes);
            memcpy(&tempBuffer[bufferMaxSize-initialPointer-offsetRefInBytes], buffer,
                    tempBufferSize - (bufferMaxSize-initialPointer-offsetRefInBytes));
        }        
    }
    else
    {
        /* B) linear array or circular array whose end has not been reached */
        memcpy(tempBuffer, &buffer[initialPointer+offsetRefInBytes], tempBufferSize);
    }
    /* 2) mask the offset part to get a clean MSB for shifting - only if offset%8 is not 0 */
    if(offsetRefInBits)
        tempBuffer[0] &= rightMask[offsetRefInBits];

    /* 3) shift all information to the right if needed */
    if(oneByteBigger)
    {
        for(idx = tempBufferSize-oneByteBigger; idx >= 0; --idx)
        {
            /* loop goes backwards */
            tempBuffer[idx] >>= 8 - bitsInLastByte;
            if(idx != 0) /* last*/
            {
                tempBuffer[idx] |=
                        (tempBuffer[idx-1] & rightMask[bitsInLastByte]) << bitsInLastByte;
            }
        }
    }
    /* 4) copy data to real functiion argument */
    *field = calloc(tempBufferSize, sizeof(unsigned char));
    memcpy(*field, tempBuffer, tempBufferSize);
    free (tempBuffer);
    return tempBufferSize;
}

int GetFieldFromBufferAsUint(const unsigned char * buffer,
	unsigned int initialPointer, unsigned int bufferMaxSize,
	int offsetRefInBits, int sizeRefInBits, unsigned int * value)
{
    int tempBufferSize = 0, idx = 0;
    unsigned char * tempBuffer = NULL;
    unsigned int tempUint = 0;

    if((sizeRefInBits/8) > (int)sizeof(unsigned int))
    {   
        return NOT_AN_UINT;
    }

    tempBufferSize = GetFieldFromBuffer(buffer, initialPointer, bufferMaxSize,
            offsetRefInBits, sizeRefInBits, &tempBuffer);
    if(tempBufferSize < 0)
    {
        if(tempBuffer != NULL)
            free(tempBuffer);
        return tempBufferSize;
    }
    else if(tempBufferSize > (int)sizeof(unsigned int))
    {
        if(tempBuffer != NULL)
        free(tempBuffer);
        return NOT_AN_UINT;
    }
    
    /* FIX ENDIANESS */
    for(idx=0; idx<tempBufferSize; ++idx)
    {
        tempUint += (unsigned int)(tempBuffer[tempBufferSize-1-idx]) << 8*idx;
    }
    free(tempBuffer);
    *value = tempUint;
    return tempBufferSize;
}

int GetFieldFromBufferAsUllong(const unsigned char * buffer,
	unsigned int initialPointer, unsigned int bufferMaxSize,
	int offsetRefInBits, int sizeRefInBits, unsigned long long * value)
{
    int tempBufferSize = 0, idx = 0;
    unsigned char * tempBuffer = NULL;
    unsigned long long tempUllong = 0;

    if((sizeRefInBits/8) > (int)sizeof(unsigned long long))
    {   
        return NOT_AN_ULLONG;
    }
    tempBufferSize = GetFieldFromBuffer(buffer, initialPointer, bufferMaxSize,
            offsetRefInBits, sizeRefInBits, &tempBuffer);
    if(tempBufferSize < 0)
    {
        if(tempBuffer != NULL)
            free(tempBuffer);
        return tempBufferSize;
    }
    else if(tempBufferSize > (int)sizeof(unsigned long long))
    {
        free(tempBuffer);
        return NOT_AN_ULLONG;
    }
    
    /* FIX ENDIANESS */
    for(idx=0; idx<tempBufferSize; ++idx)
    {
        //tempChar = tempBuffer[tempBufferSize-1-idx];
        //tempUllong += (tempChar << 8*idx);
        tempUllong += (unsigned long long)(tempBuffer[tempBufferSize-1-idx]) << 8*idx;
    }
    free(tempBuffer);
    *value = tempUllong;
    return tempBufferSize;
}

void ParseGetFieldError(int status, char * msg, int msgSize)
{
    switch (status)
    {
        case ZERO_BUFFER_SIZE:
            snprintf(msg, msgSize, "Size of auxiliary buffer is 0");
            break;

        case ERROR_ALLOCATING_MEMORY:
            snprintf(msg, msgSize, "Error Allocating Memory");
            break;

        case NOT_AN_UINT:
            snprintf(msg, msgSize, "Value is bigger than an unsigned integer");
            break;

        case NOT_AN_ULLONG:
            snprintf(msg, msgSize, "Value is bigger than an unsigned long");
            break;

        default:
            snprintf(msg, msgSize, "Undefined error %d when getting field", 
                    status);
            break;
    }
    return;
}
int SettingNumber(unsigned long long data, unsigned char * targetBuffer,
        int totalSizeInBits, int offsetInBits)
{
    unsigned int offsetModInBits = offsetInBits % 8;
    unsigned int offsetSizeModInBits = (offsetInBits + totalSizeInBits) % 8;
    
    /* data must be smaller than field size */
    if(totalSizeInBits != 64)
    {
        /* due to floating point, for 64 bits can't do it well */
        if(((data > (unsigned long long)(pow(2, totalSizeInBits)-1)) &&
                (totalSizeInBits != 64)) || (data > 0xFFFFFFFFFFFFFFFF))
        {
            return NUMBER_TOO_BIG;
        }
    }

    if((offsetModInBits + totalSizeInBits) <= 8)
    {
        /* data requires one byte or less */
        unsigned char tempData = (unsigned char) data;
        
        if(totalSizeInBits != 8)
        {
            /* shift data to the leftmost */
            tempData <<= (8 - (totalSizeInBits % 8));
        }

        if((offsetInBits % 8) == 0)
        {
            /* data begin at MSB (may be masked) */
            /* mask previous data (right) and set the remain (left) */
            targetBuffer[(offsetInBits/8)] &=
                    rightMask[offsetSizeModInBits];
            targetBuffer[(offsetInBits/8)] |=
                    tempData & leftMask[offsetSizeModInBits];
        }
        else
        {
            /* data don't begin at MSB (must be shifted and may be masked) */
            /* shift data to offset */
            tempData >>= offsetModInBits;
            
            /* mask previous data (left and right) and set the remain (center) */
            targetBuffer[(offsetInBits/8)] &=
                    leftMask[offsetModInBits] | rightMask[offsetSizeModInBits];
            targetBuffer[(offsetInBits/8)] |=
                    tempData & leftMask[offsetSizeModInBits] & rightMask[offsetModInBits];
        }
    }
    else
    {
        /* data requires two bytes or more */
        int numberOfRealBytes, i;
        unsigned char * tempBuffer, * invTempBuffer;
        numberOfRealBytes = (offsetModInBits + totalSizeInBits) / 8;
        /* size requirement can be 1 byte longer change depending on size */
        if(((offsetModInBits + totalSizeInBits) % 8) != 0)
            numberOfRealBytes++;
        
        if(offsetModInBits == 0)
        {
            /* data begin at MSB (may be masked) */
            invTempBuffer = calloc(numberOfRealBytes, sizeof(unsigned char));
            memcpy(invTempBuffer, (unsigned char*) &data, numberOfRealBytes);
            /* array byte endianess must be changed later */

            for(i=0; i<numberOfRealBytes; ++i)
            {
                /* remember changing byte endianess -> invTempBuffer[i] is wrong */
                if(i == 0) /* last is first in invTempBuffer */
                {
                    /* last: mask previous data (right) and set the remain (left) */
                    targetBuffer[(offsetInBits/8)+i] &=
                            rightMask[offsetSizeModInBits];
                    targetBuffer[(offsetInBits/8)+i] |=
                            invTempBuffer[numberOfRealBytes-1-i] & leftMask[offsetSizeModInBits];
                }
                else
                {
                    targetBuffer[(offsetInBits/8)+i] = invTempBuffer[numberOfRealBytes-1-i];
                }
            }
            free(invTempBuffer);
        }
        else
        {
            /* data don't begin at MSB (may be shifted and masked) */
            invTempBuffer = calloc(numberOfRealBytes, sizeof(unsigned char));
            tempBuffer = calloc(numberOfRealBytes, sizeof(unsigned char));
            memcpy(invTempBuffer, (unsigned char*) &data, numberOfRealBytes);
            
            /* to shift data, array byte endianess is changed */
            for(i=0; i<numberOfRealBytes; ++i)
            {
                tempBuffer[i] = invTempBuffer[numberOfRealBytes-1-i];
            }
            free(invTempBuffer);

            if(offsetSizeModInBits != 0)
            {
                /* shifting data is needed only if last byte is not full */
                for(i=0; i<numberOfRealBytes; ++i)
                {
                    /* first shift to the left data to start at the right position */
                    tempBuffer[i] <<= 8 - offsetSizeModInBits;
                    /* if no last byte, then add data shifted from next byte */
                    if(i != numberOfRealBytes-1)
                    {
                        tempBuffer[i] |= tempBuffer[i+1] >> offsetSizeModInBits;
                    }
                }
            }

            /* copy data */
            for(i=0; i<numberOfRealBytes; ++i)
            {
                if(i == 0)
                {
                    /* first: filter (mask) previous data (left) and set the remain (right) */
                    targetBuffer[(offsetInBits/8)] &=
                            leftMask[offsetInBits % 8];
                    targetBuffer[(offsetInBits/8)] |=
                            tempBuffer[0] & rightMask[offsetInBits % 8];
                }
                else if(i == numberOfRealBytes-1)
                {
                    /* last: mask previous data (right) and set the remain (left) */
                    targetBuffer[(offsetInBits/8)+i] &=
                            rightMask[(offsetInBits + totalSizeInBits) % 8];
                    targetBuffer[(offsetInBits/8)+i] |=
                            tempBuffer[i] & leftMask[(offsetInBits + totalSizeInBits) % 8];
                }
                else
                    targetBuffer[(offsetInBits/8)+i] = tempBuffer[i];
            }
            free(tempBuffer);
        }
    }
    return 0;
}

void DisplaySettingNumberError (int status, char * msg, int msgSize)
{
    switch (status)
    {
        case NUMBER_TOO_BIG:
            snprintf(msg, msgSize, "Data is bigger than size");
            break;

        default:
            snprintf(msg, msgSize, "Unknown %d settingNumber error", status);
            break;
    }
    return;
}