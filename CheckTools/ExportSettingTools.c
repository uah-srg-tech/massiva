/**
 * \file	ExportSettingTools.c
 * \brief	functions for exporting info from export struct (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		04/03/2013 at 09:33:21
 * Compiler:	LabWindows / CVI 10.0.0.0
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */ 
#include <stdio.h>				/* FILE, fopen, sprintf */
#include <string.h>				/* memcpy */
#include "ExportSettingTools.h"                 /* typeOfExport */
						/* formatField, boolVar, filter,... */
#include "../CommonTools/GetSetFieldTools.h"	/* GetFieldAsUint */
#include "../CommonTools/crc16.h"		/* set_seed_and_calculate_CRC_word16, stdint.h */

static int exportError;

enum {
    SETTING_ERROR = -1,
    GETTING_ERROR = -2,
    SOURCE_FIELDS_NULL = -3,
};

int exportInformation(typeOfExport type, levelExport * exportFields,
        unsigned int numberOfExportFields, formatField * targetFields,
        unsigned int numberOfTargetFields, unsigned char * targetBuffer,
        formatField * sourceFields, unsigned char * sourceBuffer)
{
    int status, localOffset = 0;
    unsigned int exp = 0, fld = 0;
    exportError = 0;
    
    for(fld=0; fld<numberOfTargetFields; ++fld)
    {
        targetFields[fld].exported = 0;
    }
    
    for(exp=0; exp<numberOfExportFields; ++exp)
    {
        fld = exportFields[exp].targetFieldRef;

        /* WARNING> if it's a sending packet export only CONST fields cans be exported */
        /* and fields from file / file line */
        if((type == EXPORT_CONST) && (targetFields[fld].type == VSFIELD) &&
                (exportFields[exp].type != EX_FILE) && (exportFields[exp].type != EX_FILE_LINE))
            continue;

        /* VRFields must be exported differently */
        if(targetFields[fld].type == VRFIELDSIZE)
        {
            switch(exportFields[exp].type)
            {
                case EX_CONSTANT: 
                    targetFields[fld].info.virtualSizeValue =
                            exportFields[exp].data.constOrSizeInBits;
                    break;

                 case EX_CONST_SIZE_BITS:
                    /* virtualSize in bytes, size in bits */
                    targetFields[fld].info.virtualSizeValue =
                            exportFields[exp].data.constOrSizeInBits / 8;
                    break;

                case EX_VBLE_SIZE_BITS:
                    if(sourceFields == NULL)
                    {
                        return SOURCE_FIELDS_NULL;   
                    }
                    else
                    {
                        targetFields[fld].info.virtualSizeValue =
                                (sourceFields[exportFields[exp].data.vbleSizeInBits.sizeRef].totalSizeInBits +
                                exportFields[exp].data.vbleSizeInBits.addSize) / 8;
                    }
                    /* stored in bits in a variable field */
                    /* virtualSize in bytes */ 
                    break;

                case EX_FIELD:
                    /* assumed sizeof(int) for size, checked previously */
                    if((status = GetFieldFromBufferAsUllong(sourceBuffer, 0, MAX_PACKET_SIZE,
                            sourceFields[exportFields[exp].data.sourceRef].offsetInBits,
                            sizeof(int) * 8, 
                            &targetFields[fld].info.virtualSizeValue)) < 0)
                    {
                            exportError = status;
                            return GETTING_ERROR;
                    }
                    break;

                case EX_FILE: case EX_FILE_LINE: case EX_ARRAY_FILE: case EX_CONSTANT_HEX:
                    memcpy(targetBuffer, exportFields[exp].data.file.data, 
                            exportFields[exp].data.file.size);
                    break;

                case EX_ARRAY_CONST: 
                    targetFields[fld].info.virtualSizeValue =
                            exportFields[exp].data.constOrSizeInBits;
                    break;
            }
        }
        else //not VRFIELDSIZE
        {
            switch(exportFields[exp].type)
            {
                case EX_CONSTANT:
                    if((status = SettingNumber(
                            exportFields[exp].data.constOrSizeInBits,
                            targetBuffer, targetFields[fld].totalSizeInBits,
                            targetFields[fld].offsetInBits)) < 0)
                    {
                        exportError = status;
                        return SETTING_ERROR;
                    }
                    break;

                case EX_CONST_SIZE_BITS:
                    /* size in bits, must be added in bytes */ 
                    if((status = SettingNumber(
                            (exportFields[exp].data.constOrSizeInBits/8),
                            targetBuffer, targetFields[fld].totalSizeInBits,
                            targetFields[fld].offsetInBits)) < 0)
                    {
                        exportError = status;
                        return SETTING_ERROR;
                    }
                    break;

                case EX_VBLE_SIZE_BITS:
                    /* stored a reference to variable field */
                    /* size in bits */
                    if((status = SettingNumber((sourceFields[exportFields[exp].data.vbleSizeInBits.sizeRef].totalSizeInBits +
                            exportFields[exp].data.vbleSizeInBits.addSize)/8, targetBuffer,
                            targetFields[fld].totalSizeInBits,
                            targetFields[fld].offsetInBits)) < 0)
                    {
                        exportError = status;
                        return SETTING_ERROR;
                    }
                    break;

                case EX_FIELD:
                    /* if export done after checking, always same size */
                    if(sourceFields == NULL) //exporting from no data, filling with 0
                    {
                        memset(&targetBuffer[targetFields[fld].offsetInBits/8], 0,
                                targetFields[fld].totalSizeInBits/8);
                    }
                    else
                    {
                        memcpy(&targetBuffer[targetFields[fld].offsetInBits/8],
                                &sourceBuffer[sourceFields[exportFields[exp].data.sourceRef].offsetInBits/8],
                                targetFields[fld].totalSizeInBits/8);
                    }
                    break;

                case EX_FILE: case EX_FILE_LINE: case EX_ARRAY_FILE: case EX_CONSTANT_HEX:
                    if(exportFields[exp].type == EX_ARRAY_FILE)
                    {
                        /* first we have to re-write information about global offset */
                        /* plus local offset in item format field */
                        localOffset = targetFields[fld].offsetInBits;
                        targetFields[fld].offsetInBits =
                                targetFields[targetFields[fld].info.item.arrayRef].offsetInBits +
                                targetFields[targetFields[fld].info.item.arrayRef].info.array.sizeOfItem *
                                exportFields[exp].index + localOffset;
                        /* offset from array + size of each item * index + local item offset */
                    }
                    memcpy(&targetBuffer[targetFields[fld].offsetInBits/8],
                            exportFields[exp].data.file.data, exportFields[exp].data.file.size);
                    if(exportFields[exp].type == EX_ARRAY_FILE)
                    {
                        targetFields[fld].offsetInBits = localOffset;
                    }
                    break;

                case EX_ARRAY_CONST:
                    /* first we have to re-write information about global offset */
                    /* plus local offset in item format field */
                    localOffset = targetFields[fld].offsetInBits;
                    targetFields[fld].offsetInBits =
                            targetFields[targetFields[fld].info.item.arrayRef].offsetInBits +
                            targetFields[targetFields[fld].info.item.arrayRef].info.array.sizeOfItem *
                            exportFields[exp].index + localOffset;
                    /* offset from array + size of each item * index + local item offset */
                    if((status = SettingNumber(exportFields[exp].data.constOrSizeInBits,
                            targetBuffer, targetFields[fld].totalSizeInBits,
                            targetFields[fld].offsetInBits)) < 0)
                    {
                        targetFields[fld].offsetInBits = localOffset;
                        exportError = status;
                        return SETTING_ERROR;
                    }
                    targetFields[fld].offsetInBits = localOffset;
                    break;
            }
        }
        /* check field as exported */
        targetFields[fld].exported = 1;
    }
    /* check if fields exported are parent of another ones */
    for(fld=0; fld<numberOfTargetFields; ++fld)
    {
        if(targetFields[targetFields[fld].pfid].exported == 1)
            targetFields[fld].exported = 1;
    }
    return 0;
}

void DisplayExportInformationError(int status, char * msg, int msgSize)
{
    switch (status)
    {
        case SETTING_ERROR:
            DisplaySettingNumberError(exportError, msg, msgSize);
            break;

        case GETTING_ERROR:
            ParseGetFieldError(exportError, msg, msgSize);
            break;

        case SOURCE_FIELDS_NULL:
            snprintf(msg, msgSize, "Source fields are not defined");
            break;

        default:
            snprintf(msg, msgSize, "Unknown %d exportInformation error",
                    status);
            break;
    }
    return;
}

int performDICs(unsigned int numberOfActiveDICs, unsigned int * ActiveDICs, 
	unsigned int ** crcFieldRefs, unsigned int * numberOfcrcFields,
	unsigned char * targetBuffer, formatField * targetFields)
{
    int status, DICvalue;
    unsigned int dic, crcfld;

    for(dic=0;dic<numberOfActiveDICs;++dic)
    {
        if(targetFields[ActiveDICs[dic]].info.check.type == CRC_16)
        {
            DICvalue = 0xFFFF;
            for(crcfld=0; crcfld<numberOfcrcFields[dic]; ++crcfld)
            {
                DICvalue = set_seed_and_calculate_CRC_word16(
                        targetFields[crcFieldRefs[dic][crcfld]].totalSizeInBits/8,
                        &targetBuffer[targetFields[crcFieldRefs[dic][crcfld]].offsetInBits/8],
                        DICvalue);
            }
        }
        else //if(targetFields[ActiveDICs[dic]].info.check.type == CHECKSUM_16)
        {
            DICvalue = 0;
            for(crcfld=0; crcfld<numberOfcrcFields[dic]; ++crcfld)
            {
                DICvalue = set_seed_and_calculate_checksum_word16(
                        targetFields[crcFieldRefs[dic][crcfld]].totalSizeInBits/8,
                        &targetBuffer[targetFields[crcFieldRefs[dic][crcfld]].offsetInBits/8],
                        DICvalue);
            }
        }
        if((status = SettingNumber(DICvalue, targetBuffer,
                targetFields[ActiveDICs[dic]].totalSizeInBits,
                targetFields[ActiveDICs[dic]].offsetInBits)) != 0)
        {
            return status;
        }
    }
    return 0;
}

void DisplayPerformDICsError(int status, char * msg, int msgSize)
{
    DisplaySettingNumberError(status, msg, msgSize);
    return;
}
