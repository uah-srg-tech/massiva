/**
 * \file	CheckFormatTools.c
 * \brief	functions for checking formats (definition)
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
#include <stdint.h>                         /* uint8_t */
#include <stdio.h>                          /* snprintf */
#include <math.h>                           /* pow */
#include <string.h>                         /* strlen */
#include "CheckFormatTools.h"               /* PRINT_COMPLEX, PRINT_SIMPLE */
                                            /* formatField, boolVar, filter */
#include "../CommonTools/GetSetFieldTools.h"/* GetFieldFromBufferAsUint */

enum {
    WRONG_FIELD_GETTING = -1,
    REF_FIELD_NOT_FOUND = -2,
    REF_FIELD_NOT_YET_EXPORTED = -3,
    PACKET_NOT_FILLED = -4,
    POWER_OUT_OF_RANGE = -5,
    PACKET_TOO_BIG = -6
};

static int errorNumber = 0;
static int flds[MAX_INTERFACES] = {0, 0, 0, 0, 0, 0, 0, 0, };

int CheckFormat(const uint8_t * buffer, formatField * LevelFields,
	const unsigned int numberOfLevelFields, unsigned int * length, int port)
{
    int status = 0, lengthInBits = 0;
    unsigned int fld = 0;
    
    /* only some field types must be checked */
    for(fld=0; fld<numberOfLevelFields; ++fld)
    {
        /* VFIELDs' and AFIELDs' actual total size must be obtained */
        if((LevelFields[fld].type == VSFIELD) || (LevelFields[fld].type == AFIELD))
        {
            unsigned int ref, variableSize;
            if(LevelFields[fld].type == VSFIELD)
                ref = LevelFields[fld].info.variable.fieldRef;
            else //if (LevelFields[fld].type == AFIELD)
                ref = LevelFields[fld].info.array.fieldRef;

            if(ref > numberOfLevelFields)
            {
                errorNumber = ref;
                flds[port] = fld;
                return REF_FIELD_NOT_FOUND;
            }
            /* check if REF has been previously exported */
            /* (only for sending packets) */
            if(LevelFields[ref].exported == 0)
            {
                errorNumber = ref;
                flds[port] = fld;
                return REF_FIELD_NOT_YET_EXPORTED;
            }

            /* get size */
            switch(LevelFields[ref].type)
            {
                case CSFIELD:
                    if((status = GetFieldFromBufferAsUint(buffer, 0, MAX_PACKET_SIZE, 
                            LevelFields[ref].offsetInBits,
                            LevelFields[ref].info.sizeInBits, &variableSize)) < 0)
                    {
                        flds[port] = fld;
                        return WRONG_FIELD_GETTING;
                    }
                    break;

                case CSFORMULAFIELD:
                    if((status = GetFieldFromBufferAsUint(buffer, 0, MAX_PACKET_SIZE,
                            LevelFields[ref].offsetInBits,
                            LevelFields[ref].info.formula.sizeInBits, &variableSize)) < 0)
                    {
                        flds[port] = fld;
                        return WRONG_FIELD_GETTING;
                    }
                    break;

                case VSFIELD:
                    /* only can be done if REF has been previously exported */
                    if((status = GetFieldFromBufferAsUint(buffer, 0, MAX_PACKET_SIZE,
                            LevelFields[ref].offsetInBits,
                            LevelFields[ref].totalSizeInBits, &variableSize)) < 0)
                    {
                        flds[port] = fld;
                        return WRONG_FIELD_GETTING;
                    }
                    break;

            case FDICFIELD:
                    if((status = GetFieldFromBufferAsUint(buffer, 0, MAX_PACKET_SIZE,
                            LevelFields[ref].offsetInBits,
                            LevelFields[ref].info.check.sizeInBits,	&variableSize))
                            < 0)
                    {
                        flds[port] = fld;
                        return WRONG_FIELD_GETTING;
                    }
                    break;

                case VRFIELDSIZE:
                    variableSize =
                            (unsigned int)LevelFields[ref].info.virtualSizeValue;
                    /* virtualSizeValue is always in bytes, */
                    /* so "unit" in format must be configured as "bytes" */
                    break;

                case AFIELD: 
                    /* only can be done if REF has been previously exported */
                    if((status = GetFieldFromBufferAsUint(buffer, 0, MAX_PACKET_SIZE,
                            LevelFields[ref].offsetInBits,
                            LevelFields[ref].totalSizeInBits, &variableSize)) < 0)
                    {
                        flds[port] = fld;
                        return WRONG_FIELD_GETTING;
                    }
                    break;

                default:
                    break;
            }
            if(LevelFields[fld].type == VSFIELD)
            {
                switch(LevelFields[fld].info.variable.refPower)
                {
                    case NO_POWER:
                        ref = variableSize;
                        break;

                    case BASE_2:
                        if(variableSize > 1024)
                        {
                            /* can not power up to 2^1024) */
                            flds[port] = fld;
                            return POWER_OUT_OF_RANGE;
                        }
                        ref = (unsigned int) pow(2, variableSize);
                        break;

                    case BASE_2_WITH_0:
                        if(variableSize == 0)
                            ref = 0;
                        else
                        {
                            if(variableSize > 1024)
                            {
                                /* can not power up to 2^1024) */
                                flds[port] = fld;
                                return POWER_OUT_OF_RANGE;
                            }
                            ref = (unsigned int) pow(2, variableSize);
                        }
                        break;
                }
                LevelFields[fld].totalSizeInBits =
                        (ref * LevelFields[fld].info.variable.refUnit)
                        + LevelFields[fld].info.variable.constSizeInBits;
                if(LevelFields[fld].totalSizeInBits <= 0)
                        LevelFields[fld].totalSizeInBits = 0;
            }
            else //if (LevelFields[fld].type == AFIELD))
            {
                LevelFields[fld].totalSizeInBits =
                        (LevelFields[fld].info.array.sizeOfItem * variableSize);
                if(LevelFields[fld].totalSizeInBits < 0)
                        LevelFields[fld].totalSizeInBits = 0;
                /* variableSize acts as the current ref value, not as a size */
            }
        }
        /* FDICFIELDs' actual total offset must be obtained */
        else if(LevelFields[fld].type == FDICFIELD)
        {
            LevelFields[fld].offsetInBits =
                    LevelFields[LevelFields[fld].info.check.fieldRef].offsetInBits +
                    LevelFields[LevelFields[fld].info.check.fieldRef].totalSizeInBits;
        }

        /* size is obtained for every field when values are correctly checked */
        /* only for the top fields to avoid repeating sizes */
        if(fld == LevelFields[fld].pfid)
        {
            lengthInBits += LevelFields[fld].totalSizeInBits;
        }
    }

    /* we check if packet has multiple-byte size */
    *length = lengthInBits / 8;
    if((lengthInBits % 8) != 0)
    {
        flds[port] = fld;
        return PACKET_NOT_FILLED;
    }
    if(*length > MAX_PACKET_SIZE)
    {
        errorNumber = *length;
        flds[port] = fld;
        return PACKET_TOO_BIG;
    }
    return 0;
}

void ParseCheckFormatError(int status, char * msg, int msgSize, int port)
{
    switch (status)
    {
        case WRONG_FIELD_GETTING:
            ParseGetFieldError(status, msg, msgSize);
            break;

        case REF_FIELD_NOT_FOUND:
            snprintf(msg, msgSize, "At field %d, reference field (%d) "
                    "doesn't exist", flds[port], errorNumber);
            break;

        case REF_FIELD_NOT_YET_EXPORTED:
            snprintf(msg, msgSize, "At field %d, reference field (%d) "
                    "hasn't been yet exported", flds[port], errorNumber);
            break;

        case PACKET_NOT_FILLED:
            snprintf(msg, msgSize,
                    "At field %d, packet length is not multiple of 8", flds[port]);
            break;

        case PACKET_TOO_BIG:
            snprintf(msg, msgSize, "At field %d, packet length (%d) "
                    "is bigger than max length (%d)", flds[port], errorNumber,
                    MAX_PACKET_SIZE);
            break;

        case POWER_OUT_OF_RANGE:
            snprintf(msg, msgSize,
                    "At field %d, can't calculate powers bigger than 2^1024",
                    flds[port]);
            break;

        default:
            snprintf(msg, msgSize, "Undefined error %d", status);
            break;
    }
    return;
}
