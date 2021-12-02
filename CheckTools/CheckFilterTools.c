/**
 * \file	CheckFilterTools.c
 * \brief	functions for checking boolvars and filters (definition)
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
#include <stdio.h>				/* snprintf */
#include <string.h>				/* strncmp */
#include <stdlib.h>				/* free */
#include "../definitions.h"                     /* formatField, boolVar, filter */
#include "../CommonTools/crc16.h"		/* set_seed_and_calculate_CRC_word16 */
#include "../CommonTools/GetSetFieldTools.h"	/* GetFieldAsUint */

enum {
    WRONG_FIELD_NAME = -1,
    WRONG_OPERATOR_IN_CHECK = -2,
    DATA_TOO_BIG = -3,
    WRONG_VALUE_TYPE = -4,
    ODD_OPERATOR_FOR_STRING = -5,
    CANT_GET_FIELD = -6,
    WRONG_ARRAY_FILTER = -7
};

static char errorString[MAX_ERR_LEN];
static int errorNumber, indexError;
static void getErrorStringOperator(unsigned int operator);

static int CheckSingleBoolVar (const uint8_t * buffer,
	boolVar * currentBV, formatField * currentF, int idx,
	uint64_t * numbers, uint8_t * strings[MAX_STR_OPERAND_LEN]);

int CheckBoolVars(const uint8_t * buffer, uint64_t * numbers,
	uint8_t * strings[MAX_STR_OPERAND_LEN],
	boolVar * boolVars, unsigned int numberOfBoolVars,
	formatField * LevelFields, unsigned int numberOfLevelFields, 
	unsigned int ** crcFieldRefs, const unsigned int * numberOfcrcFields)
{
    int status = 0, localOffset = 0, arrayIndex[MAX_BOOL_VARS];
    unsigned int bv, fld, elem;

    for(bv=0; bv<MAX_BOOL_VARS; ++bv)
        arrayIndex[bv] = -1;

    for(bv=0; bv<numberOfBoolVars; ++bv)
    {
        for(fld=0; fld<numberOfLevelFields; ++fld)
        {
            if(strncmp(LevelFields[fld].name, boolVars[bv].field,
                    MAX_FIELD_NAME_LEN) == 0)
            {
                break;
            }
        }
        if(fld== numberOfLevelFields)
        {
            memset(errorString, 0, MAX_ERR_LEN);
            strncpy(errorString, boolVars[bv].field, MAX_ERR_LEN);
            indexError = bv;
            return WRONG_FIELD_NAME;
        }
        localOffset = LevelFields[fld].offsetInBits;

        if(LevelFields[fld].type == AIFIELD)
        {
            int arrayRef = LevelFields[fld].info.item.arrayRef;
            if(boolVars[bv].type == FROM_ARRAY)
            {
                /* index is defined */
                LevelFields[fld].offsetInBits =
                        LevelFields[arrayRef].offsetInBits + 
                        LevelFields[arrayRef].info.array.sizeOfItem *
                        boolVars[bv].indexGroup + localOffset;
                /* offset from array + size of each item * index + */
                /* local item offset */
                status = CheckSingleBoolVar (buffer, &boolVars[bv],
                        &LevelFields[fld], bv, numbers, strings);
                LevelFields[fld].offsetInBits = localOffset;
                if(status < 0)
                {
                    indexError = bv;
                    return status;
                }
            }
            else if(boolVars[bv].type == FROM_GROUP_ARRAY)
            {
                /* we must check ALL arrays to look for the value */
                unsigned int numberOfElements;

                status = GetFieldFromBufferAsUint(buffer, 0, MAX_PACKET_SIZE,
                        LevelFields[LevelFields[arrayRef].info.array.fieldRef].offsetInBits,
                        LevelFields[LevelFields[arrayRef].info.array.fieldRef].totalSizeInBits,
                        &numberOfElements);
                if(status < 0)
                {
                    errorNumber = status;
                    return CANT_GET_FIELD;
                }

                LevelFields[fld].offsetInBits = 
                        LevelFields[arrayRef].offsetInBits + localOffset;
                /* offset from array + local item offset */

                for(elem=0; elem<numberOfElements; ++elem)
                {
                    status = CheckSingleBoolVar (buffer, &boolVars[bv],
                            &LevelFields[fld], bv, numbers, strings);
                    if(status < 0)
                    {
                        LevelFields[fld].offsetInBits = localOffset;
                        indexError = bv;
                        return status;
                    }
                    if(boolVars[bv].result == 1)
                    {
                        if(arrayIndex[elem] == -1)
                        {
                            /* the first one is always correct */
                            arrayIndex[elem] = boolVars[bv].indexGroup;
                            break;
                        }
                        else if(arrayIndex[elem] == boolVars[bv].indexGroup)
                        {
                            break;
                        }
                        else 
                        {
                            boolVars[bv].result = 0;
                        }
                    }
                    LevelFields[fld].offsetInBits +=
                            LevelFields[arrayRef].info.array.sizeOfItem;
                    /* search in other (next) item */
                }
                LevelFields[fld].offsetInBits = localOffset;
            }
            else
            {
                indexError = bv;
                return WRONG_ARRAY_FILTER;
            }
        }
        else
        {
            if(boolVars[bv].type == FDIC_BV)
            {
                unsigned int DICvalue = 0;
                if(LevelFields[fld].info.check.type == CRC_16)
                {
                    DICvalue = 0xFFFF;
                    for(elem=0; elem<numberOfcrcFields[boolVars[bv].indexGroup]; ++elem)
                    {
                        DICvalue = set_seed_and_calculate_CRC_word16(
                                LevelFields[crcFieldRefs[boolVars[bv].indexGroup][elem]].totalSizeInBits/8,
                                &buffer[LevelFields[crcFieldRefs[boolVars[bv].indexGroup][elem]].offsetInBits/8],
                                DICvalue);
                    }
                }
                else //if(targetFields[ActiveDICs[bv]].info.check.type == CHECKSUM_16)
                {
                    DICvalue = 0;
                    for(elem=0; elem<numberOfcrcFields[boolVars[bv].indexGroup]; ++elem)
                    {
                        DICvalue = set_seed_and_calculate_checksum_word16(
                                LevelFields[crcFieldRefs[boolVars[bv].indexGroup][elem]].totalSizeInBits/8,
                                &buffer[LevelFields[crcFieldRefs[boolVars[bv].indexGroup][elem]].offsetInBits/8],
                                DICvalue);
                    }
                }
                boolVars[bv].number = DICvalue;
            }

            status = CheckSingleBoolVar (buffer, &boolVars[bv], &LevelFields[fld],
                    bv, numbers, strings);
            if(status < 0)
            {
                indexError = bv;
                return status;
            }
        }
    }
    return 0;
}

static int CheckSingleBoolVar (const uint8_t * buffer,
	boolVar * currentBV, formatField * currentF, int idx,
	uint64_t * numbers, uint8_t * strings[MAX_STR_OPERAND_LEN])
{
    int status = 0, cmpStatus = 0;
    uint8_t * tempBuffer = NULL;
    unsigned long long number;
    uint8_t string[MAX_STR_OPERAND_LEN];

    switch(currentBV->valueType)
    {
        case DEC_BASE: case HEX_BASE: case BIN_BASE:
            if((currentF->totalSizeInBits/8) > (int)sizeof(unsigned long long))
            {
                currentBV->result = 0;
                break;
            }
            if((status = GetFieldFromBufferAsUllong(buffer, 0, MAX_PACKET_SIZE,
                    currentF->offsetInBits, currentF->totalSizeInBits, &number)) < 0)
            {
                break;
            }
            if(numbers != NULL)
            {
                numbers[idx] = number;
            }

            switch(currentBV->operation)
            {
                case EQUAL:
                    currentBV->result = ((number & currentBV->mask) ==
                            currentBV->number) ? 1 : 0;
                    break;

                case DIFFERENT:
                    currentBV->result = ((number & currentBV->mask) !=
                            currentBV->number) ? 1 : 0;
                    break;

                case BIGGER_THAN:
                    currentBV->result = ((number & currentBV->mask) >
                            currentBV->number) ? 1 : 0;
                    break;

                case SMALLER_THAN:
                    currentBV->result = ((number & currentBV->mask) <
                            currentBV->number) ? 1 : 0;
                    break;

                case BIGGER_OR_EQUAL:
                    currentBV->result = ((number & currentBV->mask) >=
                            currentBV->number) ? 1 : 0;
                    break;

                case SMALLER_OR_EQUAL:
                    currentBV->result = ((number & currentBV->mask) <=
                            currentBV->number) ? 1 : 0;
                    break;

                default:
                    errorNumber = currentBV->operation;
                    return WRONG_OPERATOR_IN_CHECK;
            }
            break;

        case STRING_CHAR:
            if(((currentF->totalSizeInBits/8)+1) > (int)sizeof(char))
            {
                errorNumber = (currentF->totalSizeInBits/8)+1;
                return DATA_TOO_BIG;
            }
            if((status = GetFieldFromBuffer(buffer, 0, MAX_PACKET_SIZE,
                    currentF->offsetInBits, currentF->totalSizeInBits,
                    &tempBuffer)) < 0)
            {
                break;
                //tempBuffer must be freed
            }

            string[0] = tempBuffer[0];
            free(tempBuffer);
            tempBuffer = NULL;

            if(strings != NULL)
            {
                strings[idx][0] = string[0];
            }

            switch(currentBV->operation)
            {
                case EQUAL:
                    currentBV->result = (string[0] == currentBV->string[0]) ? 1 : 0;
                    break;

                case DIFFERENT:
                    currentBV->result = (string[0] != currentBV->string[0]) ? 1 : 0;
                    break;

                case BIGGER_THAN: case SMALLER_THAN: 
                case BIGGER_OR_EQUAL: case SMALLER_OR_EQUAL:
                    /* they doesn't make any sense for text */
                    errorNumber = currentBV->operation;
                    return ODD_OPERATOR_FOR_STRING;

                default:
                    errorNumber = currentBV->operation;
                    return WRONG_OPERATOR_IN_CHECK;
            }
            break;

        case STRING:
            if((currentF->totalSizeInBits/8) > MAX_STR_OPERAND_LEN)
            {
                errorNumber = currentF->totalSizeInBits/8;
                return DATA_TOO_BIG;
            }
            if((status = GetFieldFromBuffer(buffer, 0, MAX_PACKET_SIZE,
                    currentF->offsetInBits, currentF->totalSizeInBits,
                    &tempBuffer)) < 0)
            {
                break;
                //tempBuffer must be freed
            }

            memcpy(string, tempBuffer, status);
            free(tempBuffer);
            tempBuffer = NULL;

            if(strings != NULL)
            {
                memcpy(strings[idx], string, status);
            }
            if(currentBV->mask != (unsigned long long)status)
            {
                currentBV->result = 0;
            }
            else
            {
                cmpStatus = memcmp(string, currentBV->string, status);
                switch(currentBV->operation)
                {
                    case EQUAL:
                        currentBV->result = (cmpStatus == 0) ? 1 : 0;
                        break;

                    case DIFFERENT:
                        currentBV->result = (cmpStatus != 0) ? 1 : 0;
                        break;

                    case BIGGER_THAN: case SMALLER_THAN: 
                    case BIGGER_OR_EQUAL: case SMALLER_OR_EQUAL:
                        /* they doesn't make any sense for text */
                        errorNumber = currentBV->operation;
                        return ODD_OPERATOR_FOR_STRING;

                    default:
                        errorNumber = currentBV->operation;
                        return WRONG_OPERATOR_IN_CHECK;
                }
            }
            currentBV->number = status;
            break;

        default:
            errorNumber = currentBV->valueType;
            return WRONG_VALUE_TYPE;
    }
    if(status < 0)
    {
            errorNumber = status;
            if(tempBuffer != NULL)
                free(tempBuffer);
            return CANT_GET_FIELD;
    }
    return 0;
}

void ParseCheckBoolVarError(int status, char * msg, int msgSize)
{
    snprintf(msg, 23, "Error at boolVar %3d: ", indexError);
    switch (status)
    {
        case WRONG_FIELD_NAME:
            snprintf(&msg[22], msgSize-22, "Wrong field name: %s", errorString);
            break;

        case WRONG_OPERATOR_IN_CHECK:
            getErrorStringOperator(errorNumber);
            snprintf(&msg[22], msgSize-22, "Wrong operator (%s)", errorString);
            break;

        case DATA_TOO_BIG:
            snprintf(&msg[22], msgSize-22, "Data is too big (%d / 0x%X)",
                    errorNumber, errorNumber);
            break;

        case WRONG_VALUE_TYPE:
            snprintf(&msg[22], msgSize-22, "Wrong value type (%d)", errorNumber);
            break;

        case ODD_OPERATOR_FOR_STRING:
            getErrorStringOperator(errorNumber);
            snprintf(&msg[22], msgSize-22, "Odd operator for \"string\" (%s)",
                    errorString);
            break;

        case CANT_GET_FIELD:
            ParseGetFieldError(errorNumber, &msg[22], msgSize-22);
            break;

        case WRONG_ARRAY_FILTER:
            snprintf(&msg[22], msgSize-22, "AIFields BoolVars must be"
                    "\"BoolVarFromArrayItem\" or \"BoolVarFromGroupedArrayItem\"");
            break;

        default:
            snprintf(&msg[22], msgSize-22, "Undefined error %d", status);
            break;
    }
    return;
}

unsigned short CheckFilters(filter * filters, filterTypes typeOfFilter, 
	unsigned int numberOfFilters, boolVar * boolVars, int * errorAt)
{
    unsigned int flt, bvref;
    unsigned short result;

    *errorAt = -1;
    filters[0].result = boolVars[filters[0].boolVarRef[0]].result;

    if(filters[0].numberOfBoolVarRefs != 1)
    {
        for(flt=1; flt<filters[0].numberOfBoolVarRefs; ++flt)
        {
            if(typeOfFilter == MINTERM)
            {
                filters[0].result &= boolVars[filters[0].boolVarRef[flt]].result;
            }
            else if(typeOfFilter == MAXTERM)
            {
                filters[0].result |= boolVars[filters[0].boolVarRef[flt]].result;
            }
        }
    }
    result = filters[0].result;

    if(numberOfFilters != 1)
    {
        for(flt=1; flt<numberOfFilters; ++flt)
        {	
            filters[flt].result = boolVars[filters[flt].boolVarRef[0]].result;

            if(filters[flt].numberOfBoolVarRefs != 1)
            {
                for(bvref=1; bvref<filters[flt].numberOfBoolVarRefs; ++bvref)
                {
                    if(typeOfFilter == MINTERM)
                    {
                        filters[flt].result &= boolVars[filters[flt].boolVarRef[bvref]].result;
                    }
                    else if(typeOfFilter == MAXTERM)
                    {
                        filters[flt].result |= boolVars[filters[flt].boolVarRef[bvref]].result;
                    }
                }
            }

            if(typeOfFilter == MINTERM)
            {
                result |= filters[flt].result;
            }
            else if(typeOfFilter == MAXTERM)
            {
                result &= filters[flt].result;
            }
            if((result == 0) & (filters[flt].result == 0))
            {
                *errorAt = flt;
            }
        }
    }
    return result;
}

void ParseCheckFilterError(int status, char * msg, int msgSize)
{
    switch (status)
    {
        default:
            snprintf(msg, msgSize, "Undefined error %d", status);
            break;
    }
    return;
}

static void getErrorStringOperator(unsigned int operator)
{
    switch(operator)
    {
        case EQUAL:
            snprintf(errorString, 3, "==");
            break;

        case DIFFERENT:
            snprintf(errorString, 3, "!=");
            break;

        case BIGGER_THAN:
            snprintf(errorString, 3, ">");
            break;

        case SMALLER_THAN:
            snprintf(errorString, 3, "<");
            break;

        case BIGGER_OR_EQUAL:
            snprintf(errorString, 3, ">=");
            break;

        case SMALLER_OR_EQUAL:
            snprintf(errorString, 3, "<=");
            break;

        default:
            break;
    }
}