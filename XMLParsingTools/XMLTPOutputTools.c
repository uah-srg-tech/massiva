/**
 * \file	XMLTPOutputTools.c
 * \brief	functions for parsing XML test outputs (definition)
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
#include "XMLExImTools.h"		/* CreateImportArraySelectDefault */
#include "XMLFormatTools.h"		/* CreateFormatArray */
#include "XMLFilterTools.h"		/* CreateFilterArray */

enum {
    WRONG_IFREF = -1001,
    WRONG_OUTPUT_ID = -1002,
    WRONG_OUTPUT_LEVEL = -1003,
    WRONG_DEFAULT_FILTER_0 = -1004,
    WRONG_DEFAULT_FILTER_1 = -1005,
    WRONG_DEFAULT_FILTER_2 = -1006,
    WRONG_DEFAULT_FILTER_3 = -1007,
    WRONG_OPTIONAL = -1008,
    WRONG_EXTRA_FILTER_0 = -1009,
    WRONG_EXTRA_FILTER_1 = -1010,
    WRONG_EXTRA_FILTER_2 = -1011,
    WRONG_EXTRA_FILTER_3 = -1012,
};

int ParseTestOutput(xmlNodePtr output_handle, unsigned int io,
	level defaultLevels[MAX_INTERFACES][MAX_LEVELS], output * currentOutput,
	levelTypeError * levelError, portConfig * ports, char filename[130],
	int * levelErrorRef, char attrData[ATTR_SIZE], int outputsCheckmode,
        portProtocol * protocols)
{
    int status = 0;
    formatField * levelTMFields;
    unsigned int aux = 0, lvl = 0;
    xmlAttrPtr attr;
    char relativePath[130];
    unsigned int numberOfTMFields[MAX_LEVELS];
    
    getRelativePath(filename, relativePath, 130);

    /* NOTE: "name" is not used */

    /* get id */
    if((status = GetXMLAttributeValueByName(output_handle, "id", attrData, ATTR_SIZE)) != 0) 
    {
        return status;
    }
    if(atoi(attrData) != ((int)io))
    {
        return WRONG_OUTPUT_ID;
    }

    /* get ifRef */
    if((status = GetXMLAttributeValueByName (output_handle, "ifRef", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    currentOutput->ifRef = atoi(attrData);
    /* check ifRef has been configured as output */
    if((currentOutput->ifRef >= MAX_INTERFACES) ||
            (ports[currentOutput->ifRef].portType == NO_PORT) ||
            (ports[currentOutput->ifRef].ioType == IN_PORT))
    {
        return WRONG_IFREF;
    }
	
    /* check if is is an optional output */
    GetXMLNumAttributes(output_handle, &aux);
    if(aux == 4)
    {
        char optionalTag[9];
        if((status = GetXMLAttributeByIndex (output_handle, 3, &attr)) != 0)
        {
            return status;
        }
        if((status = GetXMLAttributeName(attr, optionalTag, 9)) != 0)
        {
            return status;
        }
        if(strncmp(optionalTag, "optional", 8) == 0)
        {
            if(outputsCheckmode == ALL)
                return WRONG_OPTIONAL;
            currentOutput->optional = 1;
        }
    }

    /* check output level number */
    if((status = SearchXMLAttributeValueByName (output_handle, "type",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(attrData[0] == '\0')
    {
        currentOutput->numberOfLevels = atoi((char*)&output_handle->name[13]) + 1;//output_level_X
    }
    else
    {
        currentOutput->numberOfLevels = atoi(&attrData[26]) + 1;//gss:GSSTestProcOutputLevelX
    }
    if(currentOutput->numberOfLevels > 4)
    {
        return WRONG_OUTPUT_LEVEL;
    }
    currentOutput->level = calloc(currentOutput->numberOfLevels, sizeof(levelOut));

    for(lvl=0; lvl<currentOutput->numberOfLevels; ++lvl)
    {
        currentOutput->level[lvl].TMFields = NULL;
        currentOutput->level[lvl].numberOfTMFields = 0;
        currentOutput->level[lvl].numberOfFDICTMFields = 0;
        currentOutput->level[lvl].crcTMFieldRefs = NULL;
        currentOutput->level[lvl].numberOfcrcTMFields = 0;
        currentOutput->level[lvl].boolVars = NULL;
        currentOutput->level[lvl].numberOfBoolVars = 0;
        currentOutput->level[lvl].filters = NULL;
        currentOutput->level[lvl].numberOfFilters = 0;
        currentOutput->level[lvl].importInBytes = 0;
        currentOutput->level[lvl].virtualFields = NULL;
        currentOutput->level[lvl].numberOfVirtualFields = 0;
        numberOfTMFields[lvl] = 0;
    }
    
    if(currentOutput->numberOfLevels >= 1)
    {
        /* for any number of levels parse level 0 format and filter */
        memset(filename, 0, 130);
        *levelErrorRef = 0;
        /* check if level0 default format or another one */
        if((status = TryGetXMLOnlyChildAttributeValueByName (output_handle,
                "level0", "format", filename, 130)) != 0)
        {
            return status;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0)
        {
            /* parse level0 TMformat file */
            if((status = CreateFormatArray(filename,  relativePath,
                    &currentOutput->level[0].TMFields,
                    &currentOutput->level[0].numberOfTMFields,
                    &currentOutput->level[0].numberOfFDICTMFields,
                    &currentOutput->level[0].crcTMFieldRefs,
                    &currentOutput->level[0].numberOfcrcTMFields)) != 0)
            {
                *levelError = TC_FORMAT;
                return status;
            }
            levelTMFields = currentOutput->level[0].TMFields;
            numberOfTMFields[0] = currentOutput->level[0].numberOfTMFields;
        }
        else /* defaultFormat output level 0 */
        {
            levelTMFields = defaultLevels[currentOutput->ifRef][0].out.TMFields;
            numberOfTMFields[0] = defaultLevels[currentOutput->ifRef][0].out.numberOfTMFields;
        }

        /* check if exist level0 default filter */
        if((status = GetXMLChildAttributeValueByName (output_handle,
                "level0_filter", "apply_def_filter", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* defaultFilter in output level 0 */
        if(strncmp(attrData, "yes", 3) == 0) 
        {
            if(defaultLevels[currentOutput->ifRef][0].out.numberOfFilters == 0)
            {
                return WRONG_DEFAULT_FILTER_0;
            }
            currentOutput->level[0].defaultFilter = 1;
        }
        else //if(strncmp(attrData, "no", 2) == 0) 
        {
            currentOutput->level[0].defaultFilter = 0;
        }

        memset(filename, 0, 130);
        /* get level0_filter extra_filter */
        if((status = TryGetXMLValueFromChildAttributeOrHRefTag (output_handle,
                "level0_filter", "extra_filter", filename, 130, "NULL", 4)) != 0)
        {
            return status;
        }
        /* parse level0_filter extra_filter (if exists) */
        if(strncmp(filename, "NULL", 4) != 0)
        {
            if(numberOfTMFields[0] == 0)
            {
                return WRONG_EXTRA_FILTER_0;
            }
            /* parse level0_filter extra_filter */
            if((status = CreateFilterArray(filename, relativePath,
                    &currentOutput->level[0].boolVars,
                    &currentOutput->level[0].numberOfBoolVars,
                    &currentOutput->level[0].filters,
                    &currentOutput->level[0].numberOfFilters,
                    &currentOutput->level[0].typeOfFilters,
                    levelTMFields, numberOfTMFields[0])) != 0)
            {
                *levelError = FILTER;
                return status;
            }
        }
        /* defaultImport in output level 0 */
    }
    if(currentOutput->numberOfLevels >= 2)
    {
        /* for any 1 to 3 levels parse level1 format and filters */
        /* and level1 from level0 import */
        memset(filename, 0, 130);
        *levelErrorRef = 1;
        /* check if level1 default format or another one */
        if((status = TryGetXMLOnlyChildAttributeValueByName (output_handle,
                "level1", "format", filename, 130)) != 0)
        {
            return status;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0) 
        {
            /* parse level1 TMformat file */
            if((status = CreateFormatArray(filename, relativePath,
                    &currentOutput->level[1].TMFields,
                    &currentOutput->level[1].numberOfTMFields,
                    &currentOutput->level[1].numberOfFDICTMFields,
                    &currentOutput->level[1].crcTMFieldRefs,
                    &currentOutput->level[1].numberOfcrcTMFields)) != 0)
            {
                *levelError = TC_FORMAT;
                return status;
            }
            levelTMFields = currentOutput->level[1].TMFields;
            numberOfTMFields[1] = currentOutput->level[1].numberOfTMFields;
        }
        else /* defaultFormat output level 1 */
        {
            levelTMFields = defaultLevels[currentOutput->ifRef][1].out.TMFields;
            numberOfTMFields[1] = defaultLevels[currentOutput->ifRef][1].out.numberOfTMFields;
        }

        /* check if level1 default filter */
        if((status = GetXMLChildAttributeValueByName (output_handle,
                "level1_filter", "apply_def_filter", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* defaultFilter in output level 1 */
        if(strncmp(attrData, "yes", 3) == 0) 
        {
            if(defaultLevels[currentOutput->ifRef][1].out.numberOfFilters == 0)
            {
                return WRONG_DEFAULT_FILTER_1;
            }
            currentOutput->level[1].defaultFilter = 1;
        }
        else //if(strncmp(attrData, "no", 2) == 0) 
        {
            currentOutput->level[1].defaultFilter = 0;
        }

        memset(filename, 0, 130);
        /* get level1_filter extra_filter */
        if((status = TryGetXMLValueFromChildAttributeOrHRefTag (output_handle,
                "level1_filter", "extra_filter", filename, 130, "NULL", 4)) != 0)
        {
            return status;
        }
        /* parse level1_filter extra_filter (if exists) */
        if(strncmp(filename, "NULL", 4) != 0)
        {
            if(numberOfTMFields[1] == 0)
            {
                return WRONG_EXTRA_FILTER_1;
            }
            /* parse level1_filter extra_filter */
            if((status = CreateFilterArray(filename, relativePath,
                    &currentOutput->level[1].boolVars,
                    &currentOutput->level[1].numberOfBoolVars,
                    &currentOutput->level[1].filters,
                    &currentOutput->level[1].numberOfFilters,
                    &currentOutput->level[1].typeOfFilters,
                    levelTMFields, numberOfTMFields[1])) != 0)
            {
                *levelError = FILTER;
                return status;
            }
        }

        memset(filename, 0, 130);
        /* get level1_from_level0 import */
        if((status = TryGetXMLOnlyChildAttributeValueByName (output_handle,
                "level1_from_level0", "import", filename, 130)) != 0)
        {
            return status;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0)
        {
            /* parse level1_from_level0 import file */
            if((status = CreateImportArraySelectDefault(filename, relativePath,
                    currentOutput->level[0], &currentOutput->level[1],
                    defaultLevels[currentOutput->ifRef][0].out,
                    defaultLevels[currentOutput->ifRef][1].out)) != 0)
            {
                *levelError = IMPORT;
                return status;
            }
        }
        /* else defaultImport level1_from_level0 import */
    }
    if(currentOutput->numberOfLevels >= 3)
    {
        /* for 2 or 3 levels parse level2 format and filters */
        /* and level2 from level1 import */
        memset(filename, 0, 130);
        *levelErrorRef = 2;
        /* get level2 TMformat file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (output_handle,
                "level2", "format", filename, 130)) != 0)
        {
            return status;
        }
        /* parse level2 TMformat file */
        if((status = CreateFormatArray(filename, relativePath,
                &currentOutput->level[2].TMFields,
                &currentOutput->level[2].numberOfTMFields,
                &currentOutput->level[2].numberOfFDICTMFields,
                &currentOutput->level[2].crcTMFieldRefs,
                &currentOutput->level[2].numberOfcrcTMFields)) != 0)
        {
            *levelError = TM_FORMAT;
            return status;
        }
        levelTMFields = currentOutput->level[2].TMFields;
        numberOfTMFields[2] = currentOutput->level[2].numberOfTMFields;

        /* check if level2 default filter */
        if((status = GetXMLChildAttributeValueByName (output_handle,
                "level2_filter", "apply_def_filter", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* defaultFilter in output level 2 */
        if(strncmp(attrData, "yes", 3) == 0)
        {
            if(defaultLevels[currentOutput->ifRef][2].out.numberOfFilters == 0)
            {
                return WRONG_DEFAULT_FILTER_2;
            }
            currentOutput->level[2].defaultFilter = 1;
        }
        else //if(strncmp(attrData, "no", 2) == 0) 
        {
            currentOutput->level[2].defaultFilter = 0;
        }

        memset(filename, 0, 130);
        /* get level2_filter extra_filter */
        if((status = TryGetXMLValueFromChildAttributeOrHRefTag (output_handle,
                "level2_filter", "extra_filter", filename, 130, "NULL", 4)) != 0)
        {
            return status;
        }
        /* parse level2_filter extra_filter (if exists) */
        if(strncmp(filename, "NULL", 4) != 0)
        {
            if(numberOfTMFields[2] == 0)
            {
                return WRONG_EXTRA_FILTER_2;
            }
            /* parse level2_filter extra_filter */
            if((status = CreateFilterArray(filename, relativePath,
                    &currentOutput->level[2].boolVars,
                    &currentOutput->level[2].numberOfBoolVars,
                    &currentOutput->level[2].filters,
                    &currentOutput->level[2].numberOfFilters,
                    &currentOutput->level[2].typeOfFilters,
                    levelTMFields, numberOfTMFields[2])) != 0)
            {
                *levelError = FILTER;
                return status;
            }
        }

        memset(filename, 0, 130);
        /* get level2_from_level1 import */
        if((status = TryGetXMLOnlyChildAttributeValueByName (output_handle,
                "level2_from_level1", "import", filename, 130)) != 0)
        {
            return status;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0)
        {
            /* parse level2_from_level1 import file */
            if((status = CreateImportArraySelectDefault(filename, relativePath,
                    currentOutput->level[1], &currentOutput->level[2],
                    defaultLevels[currentOutput->ifRef][1].out,
                    defaultLevels[currentOutput->ifRef][2].out)) != 0)
            {
                *levelError = IMPORT;
                return status;
            }
        }
        /* else defaultImport level2_from_level1 import */
    }
    if(currentOutput->numberOfLevels >= 4)
    {
        /* for 3 levels parse level3 format and filters */
        /* and level3 from level2 import */
        memset(filename, 0, 130);
        *levelErrorRef = 3;
        /* get level3 TMformat file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (output_handle,
                "level3", "format", filename, 130)) != 0)
        {
            return status;
        }
        /* parse level3 TMformat file */
        if((status = CreateFormatArray(filename, relativePath,
                &currentOutput->level[3].TMFields,
                &currentOutput->level[3].numberOfTMFields,
                &currentOutput->level[3].numberOfFDICTMFields,
                &currentOutput->level[3].crcTMFieldRefs,
                &currentOutput->level[3].numberOfcrcTMFields)) != 0)
        {
            *levelError = TM_FORMAT;
            return status;
        }
        levelTMFields = currentOutput->level[3].TMFields;
        numberOfTMFields[3] = currentOutput->level[3].numberOfTMFields;

        /* check if level3 default filter */
        if((status = GetXMLChildAttributeValueByName (output_handle,
                "level3_filter", "apply_def_filter", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* defaultFilter in output level 2 */
        if(strncmp(attrData, "yes", 3) == 0)
        {
            if(numberOfTMFields[3] == 0)
            {
                return WRONG_DEFAULT_FILTER_3;
            }
            currentOutput->level[3].defaultFilter = 1;
        }
        else //if(strncmp(attrData, "no", 2) == 0) 
        {
            currentOutput->level[3].defaultFilter = 0;
        }

        memset(filename, 0, 130);
        /* get level3_filter extra_filter */
        if((status = TryGetXMLValueFromChildAttributeOrHRefTag (output_handle,
                "level3_filter", "extra_filter", filename, 130, "NULL", 4)) != 0)
        {
            return status;
        }
        /* parse level2_filter extra_filter (if exists) */
        if(strncmp(filename, "NULL", 4) != 0)
        {
            if(defaultLevels[currentOutput->ifRef][3].out.numberOfTMFields == 0)
            {
                return WRONG_EXTRA_FILTER_3;
            }
            /* parse level2_filter extra_filter */
            if((status = CreateFilterArray(filename, relativePath,
                    &currentOutput->level[3].boolVars,
                    &currentOutput->level[3].numberOfBoolVars,
                    &currentOutput->level[3].filters,
                    &currentOutput->level[3].numberOfFilters,
                    &currentOutput->level[3].typeOfFilters,
                    levelTMFields, numberOfTMFields[3])) != 0)
            {
                *levelError = FILTER;
                return status;
            }
        }
        memset(filename, 0, 130);
        /* get level3_from_level2 import */
        if((status = TryGetXMLOnlyChildAttributeValueByName (output_handle,
                "level3_from_level2", "import", filename, 130)) != 0)
        {
            return status;
        }
        if(strncmp(filename, "DEFAULT", 7) != 0)
        {
            /* parse level3_from_level2 import file */
            if((status = CreateImportArraySelectDefault(filename, relativePath,
                    currentOutput->level[2], &currentOutput->level[3],
                    defaultLevels[currentOutput->ifRef][2].out,
                    defaultLevels[currentOutput->ifRef][3].out)) != 0)
            {
                *levelError = IMPORT;
                return status;
            }
        }
        /* else defaultImport level3_from_level2 import */
    }
    
    /* get expected type and subtype */
    if(ports[currentOutput->ifRef].protocolID != -1)
    {
        for(lvl=0; lvl<currentOutput->numberOfLevels; ++lvl)
        {
            if(lvl == protocols[ports[currentOutput->ifRef].protocolID].typeLevel)
            {
                for(aux=0; aux<currentOutput->level[lvl].numberOfBoolVars; ++aux)
                {
                    if(currentOutput->level[lvl].boolVars[aux].operation == EQUAL)
                    {
                        if(strncmp(protocols[ports[currentOutput->ifRef].protocolID].typeName,
                                currentOutput->level[lvl].boolVars[aux].field, MAX_FIELD_NAME_LEN) == 0)
                        {
                            currentOutput->type = (unsigned int)currentOutput->level[lvl].boolVars[aux].number;
                        }
                        else if((protocols[ports[currentOutput->ifRef].protocolID].subtypeOffset != -1) &&
                                (strncmp(protocols[ports[currentOutput->ifRef].protocolID].subtypeName,
                                currentOutput->level[lvl].boolVars[aux].field, MAX_FIELD_NAME_LEN) == 0))
                        {
                            currentOutput->subtype = (unsigned int)currentOutput->level[lvl].boolVars[aux].number;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void DisplayParseTestOutputError(int status, levelTypeError levelError,
        char * msg, unsigned int maxMsgSize)
{
    switch(levelError)
    {
        case TC_FORMAT: 
            DisplayCreateFormatArrayError (status, msg, maxMsgSize);
            break;

        case TM_FORMAT: 
            DisplayCreateFormatArrayError (status, msg, maxMsgSize);
            break;

        case FILTER:
            DisplayCreateFilterArrayError (status, msg, maxMsgSize);
            break;

        case EXPORT: case IMPORT:
            DisplayParseTestProcImExError (status, levelError, msg, maxMsgSize);
            break;

        default:
            switch(status)
            {
                case WRONG_IFREF:
                    snprintf(msg, maxMsgSize, "Referenced port not configured");
                    break;
					
                case WRONG_OUTPUT_ID:
                    snprintf(msg, maxMsgSize, "Wrong output id");
                    break;

                case WRONG_OUTPUT_LEVEL:
                    snprintf(msg, maxMsgSize, "Out level not recognized");
                    break;

                case WRONG_DEFAULT_FILTER_0: case WRONG_DEFAULT_FILTER_1:
                case WRONG_DEFAULT_FILTER_2: case WRONG_DEFAULT_FILTER_3:
                    snprintf(msg, maxMsgSize,
                            "Configured DEFAULT filter at level %d, but "
                            "default filters haven't been defined at this level",
                            -(status-WRONG_DEFAULT_FILTER_0));
                    break;

                case WRONG_EXTRA_FILTER_0: case WRONG_EXTRA_FILTER_1:
                case WRONG_EXTRA_FILTER_2: case WRONG_EXTRA_FILTER_3:
                    snprintf(msg, maxMsgSize,
                            "Defined \"extra_filter\" at level %d, but "
                            "\"format\" not defined at this level",
                            -(status-WRONG_EXTRA_FILTER_0));
                    break;

                case WRONG_OPTIONAL:
                    snprintf(msg, maxMsgSize, "Cannot define an "
                            "optional output when \"checkmode\" is \"all\"");
                    break;

                default:
                    XMLerrorInfo(status, msg, maxMsgSize);
                    break;
            }
            break;
    }
    return;
}
