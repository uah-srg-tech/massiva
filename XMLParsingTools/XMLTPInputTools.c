/**
 * \file	XMLTPInputTools.c
 * \brief	functions for parsing XML test inputs (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/04/2013 at 14:18:34
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdlib.h>
#include <string.h>
#include "XMLTools.h"			/* GetXMLAttributeValueByName, ... */
#include "XMLExImTools.h"		/* CreateExportArraySelectDefault */
#include "XMLFormatTools.h"		/* CreateFormatArray */

enum {
    WRONG_IFREF = -1001,
    WRONG_INPUT_LEVEL = -1002,
    DEF_EXPORT_NOT_VALID_ANYMORE = -1003,
    WRONG_EXPORT_FORMAT_NOT_DEFINED = -1004,
    DEFAULT_EXPORT_NOT_DEFINED = -1005
};

static unsigned int errorNumber = 0;

static int CheckIfDefaultExportIsValid(unsigned int numberOfExportFields,
        levelIn * pDefTgtLevel, levelIn * pSrcLevel);

int ParseTestInput(xmlNodePtr input_handle,
	level defaultLevels[MAX_INTERFACES][MAX_LEVELS], input * currentInput,
	levelTypeError * levelError, portConfig * ports, char filename[130],
	int * levelErrorRef, char attrData[ATTR_SIZE])
{
    int status = 0;
    unsigned int aux = 0;
    char relativePath[130];
    getRelativePath(filename, relativePath, 130);
    errorNumber = 0;
    unsigned int numberOfTCFields[MAX_LEVELS];

    /* get ifRef */
    if((status = GetXMLAttributeValueByName (input_handle, "ifRef", attrData,
            ATTR_SIZE)) != 0)
    {
        return status;
    }
    currentInput->ifRef = atoi(attrData);
    /* check ifRef has been configured as input */
    if((currentInput->ifRef >= MAX_INTERFACES) ||
            (ports[currentInput->ifRef].portType == NO_PORT) ||
            (ports[currentInput->ifRef].ioType == OUT_PORT))
    {
        return WRONG_IFREF;
    }
    /* NOTE: "name" is not used */

    /* get delay_value */
    if((status = GetXMLAttributeValueByName (input_handle, "delay_value",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    currentInput->delayInMs = atoi(attrData);

    /* get delay_unit */
    if((status = GetXMLAttributeValueByName (input_handle, "delay_unit",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(strcmp(attrData, "miliseconds") == 0)
    {
        currentInput->delayInMs *= 1;
    }
    else if(strcmp(attrData, "seconds") == 0)
    {
        currentInput->delayInMs *= 1000;
    }

    /* check input level number */
    if((status = SearchXMLAttributeValueByName (input_handle, "type",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(attrData[0] == '\0')
    {
        currentInput->numberOfLevels = atoi((char*)&input_handle->name[12]) + 1;//input_level_X
    }
    else
    {
        currentInput->numberOfLevels = atoi(&attrData[25]) + 1;//gss:GSSTestProcInputLevelX
    }
    if(currentInput->numberOfLevels > 4)
    {
        return WRONG_INPUT_LEVEL;
    }
    currentInput->level = calloc(currentInput->numberOfLevels, sizeof(levelIn));

    for(aux=0; aux<currentInput->numberOfLevels; ++aux)
    {
        currentInput->level[aux].TCFields = NULL;
        currentInput->level[aux].numberOfTCFields = 0;
        currentInput->level[aux].numberOfFDICTCFields = 0;
        currentInput->level[aux].crcTCFieldRefs = NULL;
        currentInput->level[aux].numberOfcrcTCFields = 0;
        currentInput->level[aux].exportFields = NULL;
        currentInput->level[aux].numberOfExportFields = 0;
        currentInput->level[aux].ActiveDICs = NULL;
        currentInput->level[aux].numberOfActiveDICs = 0;
        numberOfTCFields[aux] = 0;
    }

    if(currentInput->numberOfLevels >= 1)
    {
        /* for any number of levels parse level 0 format */
        memset(filename, 0, 130);
        *levelErrorRef = 0;
        /* check if level0 default format or another one */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "level0", "format", filename, 130)) != 0)
        {
            return status;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0)
        {
            /* parse level0 TCformat file */
            if((status = CreateFormatArray(filename, relativePath,
                    &currentInput->level[0].TCFields,
                    &currentInput->level[0].numberOfTCFields,
                    &currentInput->level[0].numberOfFDICTCFields,
                    &currentInput->level[0].crcTCFieldRefs,
                    &currentInput->level[0].numberOfcrcTCFields)) != 0)
            {
                *levelError = TC_FORMAT;
                return status;
            }
            numberOfTCFields[0] = currentInput->level[0].numberOfTCFields;
        }
        else
        {
            /* else defaultFormat input level0 */
            numberOfTCFields[0] = defaultLevels[currentInput->ifRef][0].in.numberOfTCFields;
        }
    }
    if(currentInput->numberOfLevels >= 2)
    {
        /* for any 1 to 3 levels parse level1 format */
        /* and level1 to level0 export */
        memset(filename, 0, 130);
        *levelErrorRef = 1;
        /* check if level1 default format or another one */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "level1", "format", filename, 130)) != 0)
        {
            return status;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0)
        {
            /* parse level1 TCformat file */
            if((status = CreateFormatArray(filename, relativePath,
                    &currentInput->level[1].TCFields,
                    &currentInput->level[1].numberOfTCFields,
                    &currentInput->level[1].numberOfFDICTCFields,
                    &currentInput->level[1].crcTCFieldRefs,
                    &currentInput->level[1].numberOfcrcTCFields)) != 0)
            {
                *levelError = TC_FORMAT;
                return status;
            }
            numberOfTCFields[1] = currentInput->level[1].numberOfTCFields;
        }
        else
        {
            /* else defaultFormat input level1 */
            numberOfTCFields[1] = defaultLevels[currentInput->ifRef][1].in.numberOfTCFields;
        }

        memset(filename, 0, 130);
        /* get level1_to_level0 export */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "level1_to_level0", "export", filename, 130)) != 0)
        {
            return status;
        }
        if((numberOfTCFields[1] == 0) || (numberOfTCFields[0] == 0))
        {
            errorNumber = 0;
            return WRONG_EXPORT_FORMAT_NOT_DEFINED;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0)
        {
            /* parse level1_to_level0 export file */
            if((status = CreateExportArraySelectDefault(filename, relativePath,
                    &currentInput->level[0], &currentInput->level[1],
                    defaultLevels[currentInput->ifRef][0].in,
                    defaultLevels[currentInput->ifRef][1].in)) != 0)
            {
                *levelError = EXPORT;
                return status;
            }
        }
        else if(defaultLevels[currentInput->ifRef][0].in.numberOfExportFields == 0)
        {
            /* else defaultExport level1_to_level0 */
            errorNumber = 0;
            return DEFAULT_EXPORT_NOT_DEFINED;
        }
        
        if((status = CheckIfDefaultExportIsValid(currentInput->level[0].numberOfExportFields,
                &defaultLevels[currentInput->ifRef][0].in, &currentInput->level[1])) != 0)
        {
            GetXMLChildAttributeValueByName (input_handle, "level1", "format",
                    filename, 130);
            return status;
        }
    } 
    if(currentInput->numberOfLevels >= 3)
    {
        /* for 2 or 3 levels parse level2 format */
        /* and level2 to level1 export */
        memset(filename, 0, 130);
        *levelErrorRef = 2;
        /* get level2 format file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "level2", "format", filename, 130)) != 0)
        {
            return status;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0)
        {
            /* parse level2 TCformat file */
            if((status = CreateFormatArray(filename, relativePath,
                    &currentInput->level[2].TCFields,
                    &currentInput->level[2].numberOfTCFields,
                    &currentInput->level[2].numberOfFDICTCFields,
                    &currentInput->level[2].crcTCFieldRefs,
                    &currentInput->level[2].numberOfcrcTCFields)) != 0)
            {
                *levelError = TC_FORMAT;
                return status;
            }
            numberOfTCFields[2] = currentInput->level[2].numberOfTCFields;
        }
        else
        {
            /* else defaultFormat input level2 */
            numberOfTCFields[2] = defaultLevels[currentInput->ifRef][2].in.numberOfTCFields;
        }

        memset(filename, 0, 130);
        /* get level2_to_level1 export */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "level2_to_level1", "export", filename, 130)) != 0)
        {
            return status;
        }
        if((numberOfTCFields[2] == 0) || (numberOfTCFields[1] == 0))
        {
            errorNumber = 1;
            return WRONG_EXPORT_FORMAT_NOT_DEFINED;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0)
        {
            /* parse level2_to_level1 export file */
            if((status = CreateExportArraySelectDefault(filename, relativePath,
                    &currentInput->level[1], &currentInput->level[2],
                    defaultLevels[currentInput->ifRef][1].in,
                    defaultLevels[currentInput->ifRef][2].in)) != 0)
            {
                *levelError = EXPORT;
                return status;
            }
        }
        else if(defaultLevels[currentInput->ifRef][1].in.numberOfExportFields == 0)
        {
            /* else defaultExport level2_to_level1 */
            errorNumber = 1;
            return DEFAULT_EXPORT_NOT_DEFINED;
        }
        
        if((status = CheckIfDefaultExportIsValid(currentInput->level[1].numberOfExportFields,
                &defaultLevels[currentInput->ifRef][1].in, &currentInput->level[2])) != 0)
        {
            GetXMLChildAttributeValueByName (input_handle, "level2", "format",
                filename, 130);
            return status;
        }
    }

    if(currentInput->numberOfLevels == 1)
    {
        /* parse appData to level0 export */
        memset(filename, 0, 130);
        *levelErrorRef = 0;
        /* get appData_to_level0 export file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "app_to_level0", "export", filename, 130)) != 0)
        {
            return status;
        }
        if(numberOfTCFields[0] == 0)
        {
            errorNumber = 0;
            return WRONG_EXPORT_FORMAT_NOT_DEFINED;
        }
        /* parse appData_to_level0 export file */
        if((status = CreateExportArraySelectDefault(filename, relativePath,
                &currentInput->level[0], &currentInput->level[0],
                defaultLevels[currentInput->ifRef][0].in,
                defaultLevels[currentInput->ifRef][0].in)) != 0)
        {
            *levelError = EXPORT;
            return status;
        }
    }
    else if(currentInput->numberOfLevels == 2)
    {
        /* parse appData to level1 export */
        memset(filename, 0, 130);
        *levelErrorRef = 1;
        /* get appData_to_level1 export file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "app_to_level1", "export", filename, 130)) != 0)
        {
            return status;
        }
        if(numberOfTCFields[1] == 0)
        {
            errorNumber = 1;
            return WRONG_EXPORT_FORMAT_NOT_DEFINED;
        }
        /* parse appData_to_level1 export file */
        if((status = CreateExportArraySelectDefault(filename, relativePath,
                &currentInput->level[1], &currentInput->level[1],
                defaultLevels[currentInput->ifRef][1].in,
                defaultLevels[currentInput->ifRef][1].in)) != 0)
        {
            *levelError = EXPORT;
            return status;
        }
    }
    else if(currentInput->numberOfLevels == 3)
    {
        /* parse appData to level2 export */
        memset(filename, 0, 130);
        *levelErrorRef = 2;
        /* get appData_to_level2 export file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "app_to_level2", "export", filename, 130)) != 0)
        {
            return status;
        }
        if(numberOfTCFields[2] == 0)
        {
            errorNumber = 2;
            return WRONG_EXPORT_FORMAT_NOT_DEFINED;
        }
        /* parse appData_to_level2 export file */
        if((status = CreateExportArraySelectDefault(filename, relativePath,
                &currentInput->level[2], &currentInput->level[2],
                defaultLevels[currentInput->ifRef][2].in,
                defaultLevels[currentInput->ifRef][2].in)) != 0)
        {
            *levelError = EXPORT;
            return status;
        }
    }
    else if(currentInput->numberOfLevels == 4)
    {
        /* parse level3 format */
        memset(filename, 0, 130);
        *levelErrorRef = 3;
        /* get level2 format file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "level3", "format", filename, 130)) != 0)
        {
            return status;
        }
        /* parse level3 TCformat file */
        if((status = CreateFormatArray(filename, relativePath,
                &currentInput->level[3].TCFields,
                &currentInput->level[3].numberOfTCFields,
                &currentInput->level[3].numberOfFDICTCFields,
                &currentInput->level[3].crcTCFieldRefs,
                &currentInput->level[3].numberOfcrcTCFields)) != 0)
        {
            *levelError = TC_FORMAT;
            return status;
        }

        /* and appData to level3 and level3 to level2 exports */
        memset(filename, 0, 130);
        /* get appData_to_level3 export file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "app_to_level3", "export", filename, 130)) != 0)
        {
            return status;
        }
        if(numberOfTCFields[3] == 0)
        {
            errorNumber = 3;
            return WRONG_EXPORT_FORMAT_NOT_DEFINED;
        }
        /* parse appData_to_level3 export file */
        if((status = CreateExportArraySelectDefault(filename, relativePath,
                &currentInput->level[3], &currentInput->level[3],
                defaultLevels[currentInput->ifRef][3].in,
                defaultLevels[currentInput->ifRef][3].in)) != 0)
        {
            *levelError = EXPORT;
            return status;
        }

        memset(filename, 0, 130);
        /* get level3_to_level2 export */
        if((status = TryGetXMLOnlyChildAttributeValueByName (input_handle,
                "level3_to_level2", "export", filename, 130)) != 0)
        {
            return status;
        }
        if((numberOfTCFields[3] == 0) || (numberOfTCFields[2] == 0))
        {
            errorNumber = 2;
            return WRONG_EXPORT_FORMAT_NOT_DEFINED;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0)
        {
            /* parse level3_to_level2 export file */
            if((status = CreateExportArraySelectDefault(filename, relativePath,
                    &currentInput->level[2], &currentInput->level[3],
                    defaultLevels[currentInput->ifRef][2].in,
                    defaultLevels[currentInput->ifRef][3].in)) != 0)
            {
                *levelError = EXPORT;
                return status;
            }
        }
        else if(defaultLevels[currentInput->ifRef][2].in.numberOfExportFields == 0)
        {
            /* else defaultExport level3_to_level2 */
            errorNumber = 2;
            return DEFAULT_EXPORT_NOT_DEFINED;
        }
    }
    return 0;
}

void DisplayParseTestInputError (int status, levelTypeError levelError,
        char * msg, unsigned int maxMsgSize)
{
    switch(levelError)
    {
        case TC_FORMAT: 
            DisplayCreateFormatArrayError(status, msg, maxMsgSize);
            break;
            
        case EXPORT:
            DisplayParseTestProcImExError(status, levelError, msg, maxMsgSize);
            break;

        default:
            switch(status)
            {
                case WRONG_IFREF:
                    snprintf(msg, maxMsgSize, "Referenced port not configured");
                    break;

                case WRONG_INPUT_LEVEL:
                    snprintf(msg, maxMsgSize, "Input level not recognized");
                    break;

                case DEF_EXPORT_NOT_VALID_ANYMORE:
                    snprintf(msg, maxMsgSize,
                            "DEFAULT export is not compatible with this format file");
                    break;

                case WRONG_EXPORT_FORMAT_NOT_DEFINED:
                    snprintf(msg, maxMsgSize,
                            "Defined \"export\" at level %d, but "
                            "\"format\" not defined at this level", errorNumber);
                    break;

                case DEFAULT_EXPORT_NOT_DEFINED:
                    snprintf(msg, maxMsgSize,
                            "DEFAULT \"export \" not defined at level %d", errorNumber);
                    break;

                default:
                    XMLerrorInfo(status, msg, maxMsgSize);
                    break;
            }
            break;
    }
    return;
}
static int CheckIfDefaultExportIsValid(unsigned int numberOfExportFields,
        levelIn * pDefTgtLevel, levelIn * pSrcLevel)
{
    unsigned int fld = 0;
    if((pSrcLevel->numberOfTCFields != 0) && (numberOfExportFields == 0))
    {
        /* if format is not default, but export is, we must check references */
        for(fld=0; fld < pDefTgtLevel->numberOfExportFields; ++fld)
        {
            if(((pDefTgtLevel->exportFields[fld].type == EX_VBLE_SIZE_BITS) &&
                    (pSrcLevel->numberOfTCFields <= pDefTgtLevel->exportFields[fld].data.vbleSizeInBits.sizeRef)) ||
                    ((pDefTgtLevel->exportFields[fld].type == EX_FIELD) &&
                    (pSrcLevel->numberOfTCFields <= pDefTgtLevel->exportFields[fld].data.sourceRef)))
            {
                return DEF_EXPORT_NOT_VALID_ANYMORE;
            }
        }
    }
    return 0;
}
