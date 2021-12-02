/**
 * \file	XMLExportSettingTools.c
 * \brief	functions for parsing XML export settings (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		25/04/2013 at 17:48:06
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#pragma GCC diagnostic ignored "-Wunused-result"

#include <stdlib.h>			/* strtoull */
#include <string.h>
#include "XMLTools.h"			/* GetXMLValueByTag, ... */
#include "XMLFormatTools.h"		/* getFormatFromXtext */
#include "../definitions.h"		/* sizesType, exportStruct, formatField */
#include "XMLExportSettingTools.h"	/* ATTR_EXPORT_SIZE */

enum {
    WRONG_TARGET_FIELD_REF = -11,
    WRONG_TARGET_TO_AFIELD_OR_AIFIELD = -12,
    WRONG_SOURCE_FIELD_REF = -13,
    NOT_THE_SAME_SIZE = -14,
    FIELD_TO_FIELD_TOO_BIG = -15,
    WRONG_FIELD_EXPORT = -16,
    FILENAME_TOO_BIG = -17,
    FILE_NOT_FOUND = -18,
    LINE_NOT_FOUND = -19,
    CONSTANT_TO_FIELD_TOO_BIG = -20,
    CONSTANT_TOO_BIG = -21,
    WRONG_FILE_LINE_0 = -22,
    WRONG_ARGUMENT_IN_FILE = -23,
    WRONG_SETTING_AIFIELD = -24,
    WRONG_SIZE_REF = -25,
    FILE_OR_LINE_TOO_BIG = -26,
    NO_TARGET_FIELDS = -27,
    DATA_IN_FILE_TOO_BIG = -28,
    WRONG_SETTING = -29
}; 

enum {
    NOT_FULL_BYTE = -1,
    NOT_FULL_OFFSET = -2
};

static FILE * fp = NULL;
static unsigned char fileOpened = 0;
static char fileOpenedName[MAX_EXPORT_FILENAME_LEN];
static int file_line = 0;
static unsigned int errSize = 0, maxSize = 0;

int CreateExportSetting(xmlNodePtr setting, levelExport ** exportField,
	formatField * targetFields, unsigned int numberOfTargetFields,
	formatField * sourceFields, unsigned int numberOfSourceFields,
	char attrData[ATTR_EXPORT_SIZE], char errorNames[MAX_ERR_LEN],
        sizesType * sizes, int numberOfSizes, int index)
{
    int status;
    unsigned int tgt = 0, src = 0;
    char lastByte;
    unsigned int isArray = 0;
    file_line = 0;

    if(0 == numberOfTargetFields)
    {
        return NO_TARGET_FIELDS;
    }

    /* get the target field name and name length */
    if((status = GetXMLValueFromAttributeOrHRefTag (setting, "toFieldRef",
            attrData, ATTR_EXPORT_SIZE)) != 0)
    {
        return status;
    }

    GetXMLNumChildren(setting, &tgt);
    if(tgt == 0)//toFieldRef is an attribute
    {
        for(tgt=0; tgt<numberOfTargetFields; ++tgt)
        {
            if((strncmp(targetFields[tgt].name, attrData, 26) == 0) &&
                    (strlen(targetFields[tgt].name) == strlen(attrData)))
            {
                break;
            }
        }
        if(tgt == numberOfTargetFields)
        {
            memset(errorNames, 0, MAX_ERR_LEN);
            strncpy(errorNames, attrData, MAX_ERR_LEN);
            return WRONG_TARGET_FIELD_REF;
        }
    }
    else//toFieldRef is a tag 
    {
        if((status = getFormatFromXtext(attrData, strlen(attrData),
                targetFields, numberOfTargetFields, &tgt)) != 0)
        {
            memset(errorNames, 0, MAX_ERR_LEN);
            strncpy(errorNames, attrData, MAX_ERR_LEN);
            return WRONG_TARGET_FIELD_REF;
        }
    }
    
    exportField[0][index].targetFieldRef = tgt;

    char settingType[20];
    if((status = SearchXMLAttributeValueByName(setting, "type", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(attrData[0] != '\0')
    {
        strncpy(settingType, &attrData[13], 20);//remove initial "gss:GSSExport"
        settingType[0] = 's';//overwrite Uppercase "S"
    }
    else
    {
        strncpy(settingType, (char*)setting->name, 20);
    }
    if(targetFields[tgt].type == AFIELD)
    {
        memset(errorNames, 0, MAX_ERR_LEN);
        strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
        return WRONG_TARGET_TO_AFIELD_OR_AIFIELD;
    }
    if((targetFields[tgt].type == AIFIELD) &&
            ((strncmp(settingType, "settingAIFromConst", 18) != 0) &&
            (strncmp(settingType, "settingAIFromFile", 17) != 0)))
    {
        memset(errorNames, 0, MAX_ERR_LEN);
        strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
        return WRONG_TARGET_TO_AFIELD_OR_AIFIELD;
    }

    if((targetFields[tgt].type != AIFIELD) &&
            ((strncmp(settingType, "settingAIFromConst", 18) == 0) ||
            (strncmp(settingType, "settingAIFromFile", 17) == 0)))
    {
        memset(errorNames, 0, MAX_ERR_LEN);
        strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
        return WRONG_SETTING_AIFIELD;
    }
    if(strncmp(settingType, "settingFromSize", 15) == 0)
    {
        if(fileOpened)
        {
            fclose(fp); 
            fp = NULL; 
            fileOpened = 0;
        }
        /* get size if ref */
        if((status = GetXMLAttributeValueByName (setting, "sizeRef", attrData,
                ATTR_EXPORT_SIZE)) != 0)
        {
            return status;
        }
        if(atoi(attrData) >= numberOfSizes)
            return WRONG_SIZE_REF;

        if(sizes[atoi(attrData)].type == C_SIZE)
        {
            exportField[0][index].type = EX_CONST_SIZE_BITS;/* size was in bits */
            exportField[0][index].data.constOrSizeInBits = 
                    sizes[atoi(attrData)].sizeOrRef;
        }
        else //if(sizes[atoi(attrData)].type == V_SIZE)
        {
            /* if size is from variable field is not yet defined */
            /* in sizes[] is stored the variable field ref */
            exportField[0][index].type = EX_VBLE_SIZE_BITS;/* size not yet def. */
            exportField[0][index].data.vbleSizeInBits.sizeRef =
                    sizes[atoi(attrData)].sizeOrRef;
            exportField[0][index].data.vbleSizeInBits.addSize =
                    sizes[atoi(attrData)].addSizeInBits;
        }
    }
    else if(strncmp(settingType, "settingFromField", 16) == 0)
    {
        if(fileOpened)
        {
            fclose(fp); 
            fp = NULL; 
            fileOpened = 0;
        }
        /* field to copy in must occupy byte-multiple positions */
        int size = 0;

        switch(targetFields[tgt].type)
        {
            case CSFIELD:
                size = targetFields[tgt].info.sizeInBits;
                break;

            case CSFORMULAFIELD:
                size = targetFields[tgt].info.formula.sizeInBits;
                break;

            case VSFIELD:
                size = targetFields[tgt].info.variable.maxSizeInBits;
                break;

            case FDICFIELD:
                size = targetFields[tgt].info.check.sizeInBits;
                break;

            default:
                break;
        }

        if((size % 8) != 0)
        {
            return NOT_FULL_BYTE;
        }
        /* field to copy in must have a byte-multiple offset */
        if((targetFields[tgt].offsetInBits % 8) != 0)
        {
            return NOT_FULL_OFFSET;
        }
        
        if((status = GetXMLValueFromAttributeOrHRefTag(setting, "fieldRef",
                attrData, ATTR_EXPORT_SIZE)) != 0)
        {
            return status;
        }
        GetXMLNumChildren(setting, &src);

        if(src < 2)//FieldRef is an attribute
        {
            for(src=0; src<numberOfSourceFields; ++src)
            {
                if(strncmp(sourceFields[src].name, attrData, MAX_FIELD_NAME_LEN) == 0)
                    break;
            }
            if(src == numberOfSourceFields)
            {  
                memset(errorNames, 0, MAX_ERR_LEN);
                strncpy(errorNames, attrData, MAX_ERR_LEN);
                return WRONG_SOURCE_FIELD_REF;
            }
        }
        else//FieldRef is a tag 
        {
            if((status = getFormatFromXtext(attrData, strlen(attrData),
                    sourceFields, numberOfSourceFields, &src)) != 0)
            {
                memset(errorNames, 0, MAX_ERR_LEN);
                strncpy(errorNames, attrData, MAX_ERR_LEN);
                return WRONG_SOURCE_FIELD_REF;
            }
        }

        /* source field must have same (max) size as target field */
        switch(targetFields[tgt].type)
        {
            case CSFIELD: /* target */
                switch(sourceFields[src].type)
                {
                    case CSFIELD: /* source */
                        if(sourceFields[src].info.sizeInBits !=
                                targetFields[tgt].info.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case CSFORMULAFIELD: /* source */
                        if(sourceFields[src].info.formula.sizeInBits !=
                                targetFields[tgt].info.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case VSFIELD: /* source */
                        /* can't export from variable to constant */
                        /* can't check size of variable fields */
                        memset(errorNames, 0, MAX_ERR_LEN);
                        strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                        return WRONG_FIELD_EXPORT;

                    case FDICFIELD: /* source */
                        if(sourceFields[src].info.check.sizeInBits !=
                                targetFields[tgt].info.sizeInBits)
                        {
                                memset(errorNames, 0, MAX_ERR_LEN);
                                strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                                return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case VRFIELDSIZE: /* source */
                        if((sizeof(int) * 8) !=	targetFields[tgt].info.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    default:
                        break;
                }
                break;

            case CSFORMULAFIELD: /* target */
                switch(sourceFields[src].type)
                {
                    case CSFIELD: /* source */
                        if(sourceFields[src].info.sizeInBits !=
                                targetFields[tgt].info.formula.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case CSFORMULAFIELD: /* source */
                        if(sourceFields[src].info.formula.sizeInBits !=
                                targetFields[tgt].info.formula.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case VSFIELD: /* source */
                        /* can't export from variable to constant */
                        /* can't check size of variable fields */
                        memset(errorNames, 0, MAX_ERR_LEN);
                        strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                        return WRONG_FIELD_EXPORT;

                    case FDICFIELD: /* source */
                        if(sourceFields[src].info.check.sizeInBits !=
                                targetFields[tgt].info.formula.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case VRFIELDSIZE: /* source */
                        if((sizeof(int) * 8) !=	targetFields[tgt].info.formula.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    default:
                        break;
                }
                break;

            case VSFIELD: /* target */
                switch(sourceFields[src].type)
                {
                    case CSFIELD: /* source */
                        if(sourceFields[src].info.sizeInBits >
                                targetFields[tgt].info.variable.maxSizeInBits)
                        {
                                memset(errorNames, 0, MAX_ERR_LEN);
                                strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                                return FIELD_TO_FIELD_TOO_BIG;
                        }
                        break;

                    case CSFORMULAFIELD: /* source */
                        if(sourceFields[src].info.formula.sizeInBits >
                                targetFields[tgt].info.variable.maxSizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return FIELD_TO_FIELD_TOO_BIG;
                        }
                        break;

                    case VSFIELD: /* source */
                        if(sourceFields[src].info.variable.maxSizeInBits >
                                targetFields[tgt].info.variable.maxSizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case FDICFIELD: /* source */
                        if(sourceFields[src].info.check.sizeInBits >
                                targetFields[tgt].info.variable.maxSizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return FIELD_TO_FIELD_TOO_BIG;
                        }
                        break;

                    case VRFIELDSIZE: /* source */
                        if((sizeof(int) * 8) > 
                                targetFields[tgt].info.variable.maxSizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return FIELD_TO_FIELD_TOO_BIG;
                        }
                        break;

                    default:
                        break;
                }
                break;

            case FDICFIELD: /* target */
                switch(sourceFields[src].type)
                {
                    case CSFIELD: /* source */
                        if(sourceFields[src].info.sizeInBits !=
                                targetFields[tgt].info.check.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case CSFORMULAFIELD: /* source */
                        if(sourceFields[src].info.formula.sizeInBits !=
                                targetFields[tgt].info.check.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case VSFIELD: /* source */
                        /* can't export from variable to fdic */ 
                        /* can't check size of variable fields */
                        memset(errorNames, 0, MAX_ERR_LEN);
                        strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                        return WRONG_FIELD_EXPORT;

                    case FDICFIELD: /* source */
                        if(sourceFields[src].info.check.sizeInBits !=
                                targetFields[tgt].info.check.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case VRFIELDSIZE: /* source */
                        if((sizeof(int) * 8) != targetFields[tgt].info.check.sizeInBits)
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    default:
                        break;
                }
                break;

            case VRFIELDSIZE: /* target */
                switch(sourceFields[src].type)
                {
                    case CSFIELD: /* source */
                        if(sourceFields[src].info.sizeInBits != (sizeof(int) * 8))
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case CSFORMULAFIELD: /* source */
                            if(sourceFields[src].info.formula.sizeInBits !=
                                    (sizeof(int) * 8))
                            {
                                memset(errorNames, 0, MAX_ERR_LEN);
                                strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                                return NOT_THE_SAME_SIZE;
                            }
                            break;

                    case VSFIELD: /* source */
                        /* can't export from variable to virtual */
                        /* can't check size of variable fields */
                        memset(errorNames, 0, MAX_ERR_LEN);
                        strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                        return WRONG_FIELD_EXPORT;

                    case FDICFIELD: /* source */
                        if(sourceFields[src].info.check.sizeInBits != (sizeof(int) * 8))
                        {
                            memset(errorNames, 0, MAX_ERR_LEN);
                            strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                            return NOT_THE_SAME_SIZE;
                        }
                        break;

                    case VRFIELDSIZE: /* source */
                        /* certainly same size */
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
        exportField[0][index].type = EX_FIELD;
        exportField[0][index].data.sourceRef = src;
    }
    else if((strncmp(settingType, "settingFromConst", 16) == 0) ||
            (strncmp(settingType, "settingAIFromConst", 18) == 0))
    {
        unsigned int nameLen = 0, isConstantHex = 0;

        if(fileOpened)
        {
            fclose(fp); 
            fp = NULL; 
            fileOpened = 0;
        }
        if(strncmp(settingType, "settingAIFromConst", 18) == 0)
        {
            isArray = 1;
        }
        
        /* get value length */
        if((status = GetXMLAttributeLengthByName(setting, "value", &nameLen)) != 0)
        {
            return status;
        }
        /* get value */
        if((status = GetXMLAttributeValueByName (setting, "value", attrData,
                ATTR_EXPORT_SIZE)) != 0)
        {
            return status;
        }

        if(strncmp(attrData, "0x", 2) == 0)
        {
            /* hex format */
            if(nameLen > (2+MAX_NUM_OPERAND_LEN*2+1)) //0x + 8*2 hex bytes + '\0'
            {
                /* data defined as hex inside XML file itself */
                exportField[0][index].data.file.size = (nameLen-2)/2;
                
                unsigned int maxLenBytes = 0;
                if((targetFields[tgt].type == CSFIELD) ||
                        (targetFields[tgt].type == CSFORMULAFIELD))
                {
                    maxLenBytes = targetFields[tgt].info.variable.maxSizeInBits/8;
                }
                else if(targetFields[tgt].type == VSFIELD)
                {
                    maxLenBytes = targetFields[tgt].info.variable.maxSizeInBits/8;
                }
                
                if(exportField[0][index].data.file.size > maxLenBytes)
                {
                    memset(errorNames, 0, MAX_ERR_LEN);
                    strncpy(errorNames, targetFields[tgt].name, MAX_ERR_LEN);
                    errSize = exportField[0][index].data.file.size;
                    return DATA_IN_FILE_TOO_BIG;
                }
                memset(exportField[0][index].data.file.data, 0, exportField[0][index].data.file.size);
                unsigned int idx;
                int hex;
                for(idx=0; idx<exportField[0][index].data.file.size; idx++)
                {
                    sscanf(&attrData[2+2*idx], "%02X", (int *)&hex);
                    exportField[0][index].data.file.data[idx] = hex;
                }
                isConstantHex = 1;
            }
            else
            {
                exportField[0][index].data.constOrSizeInBits =
                        strtoull(&attrData[2], NULL, 16);
            }
        }
        else if(strncmp(attrData, "b", 1) == 0)
        {
            unsigned int aux;
            if(nameLen > (MAX_NUM_OPERAND_LEN*8+1)) //b + 8*8 bits + '\0'
            {
                memset(errorNames, 0, MAX_ERR_LEN);
                strncpy(errorNames, attrData, MAX_ERR_LEN);
                return CONSTANT_TOO_BIG;
            }
            /* bin format */
            exportField[0][index].data.constOrSizeInBits = 0;
            for(aux=1; aux<nameLen; ++aux)
            {
                /* convert character for binary */
                attrData[aux] -= 48;
                if ((attrData[aux] != 0) && (attrData[aux] != 1))
                    /* invalid character */
                    break;
                /* multiply by the base */
                exportField[0][index].data.constOrSizeInBits *= 2;
                /* add current value */
                exportField[0][index].data.constOrSizeInBits += attrData[aux];
            }
        }
        else
        {
            if(nameLen > 20) //2^64 = 18446744073709551616 + '\0'
            {
                memset(errorNames, 0, MAX_ERR_LEN);
                strncpy(errorNames, attrData, MAX_ERR_LEN);
                return CONSTANT_TOO_BIG;
            }
            /* dec format */
            exportField[0][index].data.constOrSizeInBits = strtoull(attrData, NULL, 10);
        }

        if(isArray)
        {
            /* get array index field */
            if((status = GetXMLAttributeValueByName (setting, "toArrayIndex",
                    attrData, ATTR_EXPORT_SIZE)) != 0) 
            {
                return status;
            }
            exportField[0][index].index = atoi(attrData);
            exportField[0][index].type = EX_ARRAY_CONST;
        }
        else if(isConstantHex)
        {
            exportField[0][index].type = EX_CONSTANT_HEX;
        }
        else 
        {
            exportField[0][index].type = EX_CONSTANT;
        }
    }
    else if((strncmp(settingType, "settingFromFile", 15) == 0) || 
            (strncmp(settingType, "settingAIFromFile", 17) == 0))
    {
        xmlAttrPtr attrib = NULL;
        static char filename[MAX_EXPORT_FILENAME_LEN];
        unsigned int file_offset = 0;
        int auxSize = -1;
        char attribName[7];

        if(strncmp(settingType, "settingAIFromFile", 16) == 0)
        {
            isArray = 1;
        }
        if((status = GetXMLAttributeLengthByName(setting, "file", &src)) != 0)
        {
            return status;
        }
        if(src > MAX_EXPORT_FILENAME_LEN)
        {
            return FILENAME_TOO_BIG;
        }

        /* get file name and check size */
        if((status = GetXMLAttributeValueByName (setting, "file", attrData, ATTR_EXPORT_SIZE)) != 0)
        {
            return status;
        }
        strncpy(filename, attrData, MAX_EXPORT_FILENAME_LEN);

        /* field to copy in must occupy byte-multiple positions */
        switch(targetFields[tgt].type)
        {
            case CSFIELD:
                auxSize = targetFields[tgt].info.sizeInBits;
                break;

            case CSFORMULAFIELD:
                auxSize = targetFields[tgt].info.formula.sizeInBits;
                break;

            case VSFIELD:
                auxSize = targetFields[tgt].info.variable.maxSizeInBits;
                break;

            case FDICFIELD:
                auxSize = targetFields[tgt].info.check.sizeInBits;
                break;

            case AIFIELD:
                auxSize = targetFields[tgt].info.item.sizeInBits;
                break;

            default:
                break;
        }

        if((auxSize % 8) != 0)
        {
            return NOT_FULL_BYTE;
        }
        /* field to copy in must have a byte-multiple offset */
        if((targetFields[tgt].offsetInBits % 8) != 0)
        {
            return NOT_FULL_OFFSET;
        }

        if((!fileOpened) || (!memcmp(fileOpenedName, filename, MAX_EXPORT_FILENAME_LEN))) 
        {
            fp = fopen(filename, "rb");
            if(fp == NULL)
            {
                memset(errorNames, 0, MAX_ERR_LEN);
                strncpy(errorNames, attrData, MAX_ERR_LEN);
                return FILE_NOT_FOUND;
            }
            fileOpened = 1;
            memcpy(fileOpenedName, filename, MAX_EXPORT_FILENAME_LEN);
        }
        memset(errorNames, 0, MAX_ERR_LEN);
        strncpy(errorNames, attrData, MAX_ERR_LEN);

        /* look for file size */
        fseek(fp, 0, SEEK_END);/* seek to end of file */
        exportField[0][index].data.file.size = ftell(fp);

        /* get current file pointer in bits (hex) */
        if(exportField[0][index].data.file.size != 0)
        {
            fseek(fp, -1, SEEK_END); /* seek to last char of file */
            fread((char *)&lastByte, 1, 1, fp);
        }
        rewind(fp);
        /* don't fclose unless export changes or it is last export */

        if(exportField[0][index].data.file.size != 0)
        {
            if(lastByte == '\n') /* check if file has a \n as last char */
                exportField[0][index].data.file.size--;
            exportField[0][index].data.file.size /= 2;
            /* get current file pointer in bits (hex mode) */
        }

        /* check if defined offset and/or size */
        if((status = GetXMLNumAttributes(setting, (unsigned int*)&src)) != 0)
        {
            return status;
        }
        if(src == (3 + isArray))
        {
            if((status = GetXMLAttributeByIndex(setting, 2+isArray, &attrib)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeName(attrib, attribName, 7)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeValue(attrib, attrData, ATTR_EXPORT_SIZE)) != 0)
            {
                return status;
            }
            if(strncmp("offset", attribName, 6) == 0)
            {
                file_offset = atoi(attrData);
                exportField[0][index].data.file.size -= file_offset;
            }
            else if(strncmp("size", attribName, 4) == 0)
            {
                exportField[0][index].data.file.size = atoi(attrData);
            }
            else //if(strncmp("line", attribName, 4) == 0)
            {
                file_line = atoi(attrData);
                if(file_line < 1)
                {
                    return WRONG_FILE_LINE_0;
                }
                auxSize = -1;
            }
        }
        else if(src == (4 + isArray))
        {
            if((status = GetXMLAttributeByIndex(setting, 2+isArray, &attrib)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeName(attrib, attribName, 7)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeValue(attrib, attrData, ATTR_EXPORT_SIZE)) != 0)
            {
                return status;
            }
            if(strncmp("offset", attribName, 6) == 0)
            {
                file_offset = atoi(attrData);
            }
            else
            {
                return WRONG_ARGUMENT_IN_FILE;
            }
            if((status = GetXMLAttributeByIndex(setting, 3+isArray, &attrib)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeName(attrib, attribName, 7)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeValue(attrib, attrData, ATTR_EXPORT_SIZE)) != 0)
            {
                return status;
            }
            if(strncmp("size", attribName, 4) == 0)
            {
                exportField[0][index].data.file.size = atoi(attrData);
            }
            else if(strncmp("line", attribName, 4) == 0)
            {
                file_line = atoi(attrData);
                if(file_line < 1)
                {
                    return WRONG_FILE_LINE_0;
                }
                auxSize = -1;
            }
            else
            {
                return WRONG_ARGUMENT_IN_FILE;
            }
        }

        if(auxSize == -1)
        {
            int aux;
            char lineData[MAX_STR_OPERAND_LEN*2];
            for(aux=0; aux<file_line; ++aux)
            {
                if(fgets(lineData, MAX_STR_OPERAND_LEN*2, fp) == NULL)
                {
                    fclose(fp);
                    fp = NULL;
                    return LINE_NOT_FOUND;
                }
            }
            exportField[0][index].data.file.size = strlen(lineData) - file_offset*2;
            fseek (fp, -exportField[0][index].data.file.size, SEEK_CUR);
            exportField[0][index].data.file.size /= 2;
            if(lineData[strlen(lineData)-2] == '\r')
                exportField[0][index].data.file.size--;
            /* don't fclose unless export changes or it is last export */
        }
        else
        {
            fseek(fp, file_offset*2, SEEK_SET);
        }
        errSize = exportField[0][index].data.file.size;

        /* file size must be equal or smaller than target field */ 
        exportField[0][index].data.file.size *= 8; //bits
        switch(targetFields[tgt].type)
        {
            case CSFIELD:
                if(exportField[0][index].data.file.size > targetFields[tgt].info.sizeInBits)
                {
                    maxSize = targetFields[tgt].info.sizeInBits / 8;
                    return FILE_OR_LINE_TOO_BIG;
                }
                break;

            case CSFORMULAFIELD:
                if(exportField[0][index].data.file.size > targetFields[tgt].info.formula.sizeInBits)
                {
                    maxSize = targetFields[tgt].info.formula.sizeInBits / 8;
                    return FILE_OR_LINE_TOO_BIG;
                }
                break;

            case VSFIELD:
                if(exportField[0][index].data.file.size > targetFields[tgt].info.variable.maxSizeInBits)
                {
                    maxSize = targetFields[tgt].info.variable.maxSizeInBits / 8;
                    return FILE_OR_LINE_TOO_BIG;
                }
                break;

            case FDICFIELD:
                if(exportField[0][index].data.file.size > targetFields[tgt].info.check.sizeInBits)
                {
                    maxSize = targetFields[tgt].info.check.sizeInBits / 8;
                    return FILE_OR_LINE_TOO_BIG;
                }
                break;

            case VRFIELDSIZE:
                if(exportField[0][index].data.file.size > (sizeof(int) * 8))
                {
                    maxSize = sizeof(int);
                    return FILE_OR_LINE_TOO_BIG;
                }
                break;

            default:
                break;
        }
        exportField[0][index].data.file.size /= 8; //bytes

        if(exportField[0][index].data.file.size > MAX_STR_OPERAND_LEN)
        {
            maxSize = MAX_STR_OPERAND_LEN;
            return FILE_OR_LINE_TOO_BIG;
        }

        /* get data from file */
        char hex;
        unsigned int idx;
        memset(exportField[0][index].data.file.data, 0, exportField[0][index].data.file.size);
        for(idx=0; idx<exportField[0][index].data.file.size; idx++)
        {
            fscanf (fp, "%02X", (int *)&hex);
            exportField[0][index].data.file.data[idx] = hex;
        }

        if(isArray)
        {
            /* get array index field */
            if((status = GetXMLAttributeValueByName (setting, "toArrayIndex",
                    attrData, ATTR_EXPORT_SIZE)) != 0)
            {
                return status;
            }
            exportField[0][index].index = atoi(attrData);
            exportField[0][index].type = EX_ARRAY_FILE;
        }
        else if(auxSize == -1)
        {
            exportField[0][index].type = EX_FILE_LINE;
        }
        else
        {
            exportField[0][index].type = EX_FILE;
        }
    }
    else
    {
        memset(errorNames, 0, MAX_ERR_LEN);
        strncpy(errorNames, settingType, MAX_ERR_LEN);
        return WRONG_SETTING;
    }
    return 0;
}

void DisplayCreateExportSettingError (int status, char errorNames[MAX_ERR_LEN],
        char * msg, unsigned int maxMsgSize)
{
    switch (status)
    {
        case NO_TARGET_FIELDS:
            snprintf(msg, maxMsgSize, "No target fields to be exported");
            break;

        case WRONG_TARGET_FIELD_REF:
            snprintf(msg, maxMsgSize, "Wrong target \"toFieldRef\" (%s)",
                    errorNames);
            break;

        case WRONG_TARGET_TO_AFIELD_OR_AIFIELD:
            snprintf(msg, maxMsgSize, "Wrong \"toFieldRef\" (%s): only "
                    "\"Const\" or \"File\" can export to AIField", errorNames);
            break;

        case WRONG_SOURCE_FIELD_REF:
            snprintf(msg, maxMsgSize, "Wrong source \"fieldRef\" (%s) ",
                    errorNames);
            break;

        case NOT_THE_SAME_SIZE:
            snprintf(msg, maxMsgSize, "\"toFieldRef\" (%s) and "
                    "\"fieldRef\" doesn't have the same size", errorNames);
            break;

        case WRONG_FIELD_EXPORT:
            snprintf(msg, maxMsgSize, "\"toFieldRef\" (%s) "
                    "export is not allowed", errorNames);
            break;

        case FIELD_TO_FIELD_TOO_BIG:
            snprintf(msg, maxMsgSize, "\"toFieldRef\" (%s) is "
                    "bigger than \"fieldRef\"", errorNames);
            break;

        case FILENAME_TOO_BIG:
            snprintf(msg, maxMsgSize, "Export filename too big. "
                    "Max length is %d", MAX_EXPORT_FILENAME_LEN);
            break;

        case FILE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "File \"%s\" not found", errorNames);
            break;

        case LINE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "Line %d not found at file \"%s\" not found",
                file_line, errorNames);
            break;

        case CONSTANT_TO_FIELD_TOO_BIG:
            snprintf(msg, maxMsgSize, "\"toFieldRef\" of settingFromConst "
                    "\"%s\" is too big. Maximum size is %d bytes",
                    errorNames, MAX_NUM_OPERAND_LEN);
            break;

        case CONSTANT_TOO_BIG:
            snprintf(msg, maxMsgSize, "settingFromConst value \"%s\""
                    " is too big. Maximum size is %d bytes", errorNames,
                    MAX_NUM_OPERAND_LEN);
            break;
            
        case DATA_IN_FILE_TOO_BIG:
            snprintf(msg, maxMsgSize, "settingFromConst \"%s\""
                    " is too big. Maximum size is %d bytes", errorNames,
                    errSize);
            break;

        case WRONG_FILE_LINE_0:
            snprintf(msg, maxMsgSize, "settingFromFile \"line\" "
                    "can't be \"0\"");
            break;

        case NOT_FULL_BYTE:
            snprintf(msg, maxMsgSize, "Wrong \"toFieldRef\" size."
                    "It must have 8 bits or a multiple");
            break;

        case NOT_FULL_OFFSET:
            snprintf(msg, maxMsgSize, "Wrong field offset");
            break;

        case WRONG_SETTING_AIFIELD:	
            snprintf(msg, maxMsgSize, "Wrong \"settingAIFromConst\" or "
                    "\"settingAIFromFile\" (%s): \"toFieldRef\" is not "
                    "an AIField", errorNames);
            break;

        case WRONG_SIZE_REF:
            snprintf(msg, maxMsgSize, "Wrong \"settingFromSize\". "
                    "\"sizeRef\" does not exist");
            break;

        case FILE_OR_LINE_TOO_BIG:
            if(file_line)
                snprintf(msg, maxMsgSize, "Line %d at file \"%s\" is too big "
                        "(%d > %d)", file_line, errorNames, errSize, maxSize);
            else
                snprintf(msg, maxMsgSize, "File \"%s\" is too big (%d > %d)",
                        errorNames, errSize, maxSize);
            break;

        case WRONG_SETTING:	
            snprintf(msg, maxMsgSize, "Wrong \"setting\" tag %s",
                    errorNames);
            break;

        default:
            XMLerrorInfo(status, msg, maxMsgSize);
            break;
    }
    return;
}

void closeXMLExportFileOpened (void)
{
    if(fp != NULL)
    {
        fclose(fp); 
        fp = NULL; 
        fileOpened = 0;
    }
    return;
}