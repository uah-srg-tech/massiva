/**
 * \file	XMLEgsePeriodicTC.c
 * \brief	functions for parsing XML periodic TCs (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/04/2013 at 14:18:34
 * Company:		Space Research Group, Universidad de Alcalï¿½.
 * Copyright:	Copyright (c) 2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdlib.h>
#include <string.h>
#include "XMLTools.h"			/* GetXMLAttributeValueByName, ... */
#include "XMLExImTools.h"		/* CreateExportArraySelectDefault */
#include "XMLFormatTools.h"		/* CreateFormatArray */
#include "XMLFilterTools.h"		/* CreateFilterArray */

enum {
    WRONG_IFREF = -101,
    WRONG_PERIODIC_TC_TAG = -102,
    WRONG_PERIODIC_TC_LEVEL = -103,
};

static levelTypeError levelError;
static char filenameError[ATTR_SIZE];
static int errorNumbers = 0;

int ParsePeriodicTC(xmlNodePtr periodicTC_handle, input * periodicTC,
	char * periodicTCname, level defaultLevels[MAX_INTERFACES][MAX_LEVELS],
	portConfig * ports, unsigned int * levelErrorRef, char attrData[ATTR_SIZE],
        const char * relativePath)
{
    int status = 0;
    unsigned int lvl = 0;
    
    memset(filenameError, 0, ATTR_SIZE);
    levelError = CONFIG_OTHER;
    
    /* get name */
    if((status = GetXMLAttributeValueByName (periodicTC_handle, "name", attrData,
            80)) != 0)
    {
        return status;
    }
    strncpy(periodicTCname, attrData, MAX_FIELD_NAME_LEN);
    /* get ifRef */
    if((status = GetXMLAttributeValueByName (periodicTC_handle, "ifRef", attrData,
            70)) != 0)
    {
        return status;
    }
    periodicTC->ifRef = atoi(attrData);
    /* check ifRef has been configured as output */
    if((ports[periodicTC->ifRef].portType == NO_PORT) ||
            (ports[periodicTC->ifRef].ioType == OUT_PORT))
    {
        return WRONG_IFREF;
    }
    /* get period_value */
    if((status = GetXMLAttributeValueByName (periodicTC_handle, "period_value",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    periodicTC->delayInMs = atoi(attrData);
    /* get period_unit */
    if((status = GetXMLAttributeValueByName (periodicTC_handle, "period_unit",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(strcmp(attrData, "miliseconds") == 0)
    {
        periodicTC->delayInMs *= 1;
    }
    else if(strcmp(attrData, "seconds") == 0)
    {
        periodicTC->delayInMs *= 1000;
    }
    /* get PeriodicTC_level_X */
    int periodicTCLevel = 0;
    if((status = SearchXMLAttributeValueByName(periodicTC_handle, "type", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(attrData[0] != '\0')
    {
        if(strncmp(&attrData[17], "PeriodicTCLevel", 15) == 0)
            periodicTCLevel = atoi(&attrData[32]);//remove initial "gss_2:GSSScenario"
        else
            return WRONG_PERIODIC_TC_TAG;
    }
    else
    {
        if(strncmp((char*)periodicTC_handle->name, "PeriodicTC_level_", 17) == 0)
            periodicTCLevel = atoi((char*)&periodicTC_handle->name[17]);
        else
            return WRONG_PERIODIC_TC_TAG;
    }
    if((periodicTCLevel < 0) || (periodicTCLevel > 2))
    {
        errorNumbers = periodicTCLevel;
        return WRONG_PERIODIC_TC_LEVEL;
    }
    periodicTC->numberOfLevels = periodicTCLevel+1;
    periodicTC->level = calloc(periodicTC->numberOfLevels, sizeof(levelIn));

    for(lvl=0; lvl<periodicTC->numberOfLevels; ++lvl)
    {
        periodicTC->level[lvl].TCFields = NULL;
        periodicTC->level[lvl].numberOfTCFields = 0;
        periodicTC->level[lvl].numberOfFDICTCFields = 0;
        periodicTC->level[lvl].crcTCFieldRefs = NULL;
        periodicTC->level[lvl].numberOfcrcTCFields = 0;
        periodicTC->level[lvl].exportFields = NULL;
        periodicTC->level[lvl].numberOfExportFields = 0;
        periodicTC->level[lvl].ActiveDICs = NULL;
        periodicTC->level[lvl].numberOfActiveDICs = 0;
    }

    if((periodicTCLevel >= 0) && (periodicTCLevel <= 2))
    {
        memset(filenameError, 0, ATTR_SIZE);
        *levelErrorRef = 0;
        /* check if level0 default format or another one */
        if((status = TryGetXMLOnlyChildAttributeValueByName (periodicTC_handle,
                "level0", "format", filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strncmp(filenameError, "DEFAULT", 7) != 0)
        {
            /* parse level0 TCformat file */
            if((status = CreateFormatArray(filenameError, relativePath,
                    &periodicTC->level[0].TCFields,
                    &periodicTC->level[0].numberOfTCFields,
                    &periodicTC->level[0].numberOfFDICTCFields,
                    &periodicTC->level[0].crcTCFieldRefs,
                    &periodicTC->level[0].numberOfcrcTCFields)) != 0)
            {
                levelError = TC_FORMAT;
                return status;
            }
        }
        /*else defaultFormat input level 0 */
    }

    if((periodicTCLevel >= 1) && (periodicTCLevel <= 2))
    {
        memset(filenameError, 0, ATTR_SIZE);
        *levelErrorRef = 1;
        /* check if level1 default format or another one */
        if((status = TryGetXMLOnlyChildAttributeValueByName (periodicTC_handle,
                "level1", "format", filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strncmp(filenameError, "DEFAULT", 7) != 0)
        {
            /* parse level1 TCformat file */
            if((status = CreateFormatArray(filenameError, relativePath,
                    &periodicTC->level[1].TCFields,
                    &periodicTC->level[1].numberOfTCFields,
                    &periodicTC->level[0].numberOfFDICTCFields,
                    &periodicTC->level[1].crcTCFieldRefs,
                    &periodicTC->level[1].numberOfcrcTCFields)) != 0)
            {
                levelError = TC_FORMAT;
                return status;
            }
        }
        /* else defaultFormat input level 0 */

        memset(filenameError, 0, ATTR_SIZE);
        /* get level1_to_level0 export */
        if((status = TryGetXMLOnlyChildAttributeValueByName (periodicTC_handle,
                "level1_to_level0", "export", filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strncmp(filenameError, "DEFAULT", 7) != 0)
        {
            /* parse level1_to_level0 export file */
            if((status = CreateExportArraySelectDefault(filenameError, relativePath,
                    &periodicTC->level[0], &periodicTC->level[1],
                    defaultLevels[periodicTC->ifRef][0].in,
                    defaultLevels[periodicTC->ifRef][1].in)) != 0)
            {
                levelError = EXPORT;
                return status;
            }
        }
        /* else defaultExport level1_to_level0 */
    }

    if(periodicTCLevel == 0)
    {
        memset(filenameError, 0, ATTR_SIZE);
        *levelErrorRef = 0;
        /* get appData_to_level0 export file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (periodicTC_handle,
                "app_to_level0", "export", filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* parse appData_to_level0 export file */
        if((status = CreateExportArraySelectDefault(filenameError, relativePath,
                &periodicTC->level[0], &periodicTC->level[0],
                defaultLevels[periodicTC->ifRef][0].in,
                defaultLevels[periodicTC->ifRef][0].in)) != 0)
        {
            levelError = EXPORT;
            return status;
        }
    }
    else if(periodicTCLevel == 1)
    {
        memset(filenameError, 0, ATTR_SIZE);
        *levelErrorRef = 1;
        /* get appData_to_level1 export file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (periodicTC_handle,
                "app_to_level1", "export", filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* parse appData_to_level1 export file */
        if((status = CreateExportArraySelectDefault(filenameError, relativePath,
                &periodicTC->level[1], &periodicTC->level[1],
                defaultLevels[periodicTC->ifRef][1].in,
                defaultLevels[periodicTC->ifRef][1].in)) != 0)
        {
            levelError = EXPORT;
            return status;
        }
    }
    else if(periodicTCLevel == 2)
    {
        memset(filenameError, 0, ATTR_SIZE);
        *levelErrorRef = 2;
        /* get level2 format file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (periodicTC_handle,
                "level2", "format", filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* parse level2 TCformat file */
        if((status = CreateFormatArray(filenameError, relativePath,
                &periodicTC->level[2].TCFields,
                &periodicTC->level[2].numberOfTCFields,
                &periodicTC->level[0].numberOfFDICTCFields,
                &periodicTC->level[2].crcTCFieldRefs,
                &periodicTC->level[2].numberOfcrcTCFields)) != 0)
        {
            levelError = TC_FORMAT;
            return status;
        }

        memset(filenameError, 0, ATTR_SIZE);
        /* get appData_to_level2 export file */
        if((status = TryGetXMLOnlyChildAttributeValueByName (periodicTC_handle,
                "app_to_level2", "export", filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* parse appData_to_level2 export file */
        if((status = CreateExportArraySelectDefault(filenameError, relativePath,
                &periodicTC->level[2], &periodicTC->level[2],
                defaultLevels[periodicTC->ifRef][2].in,
                defaultLevels[periodicTC->ifRef][2].in)) != 0)
        {
            levelError = EXPORT;
            return status;
        }

        memset(filenameError, 0, ATTR_SIZE);
        /* get level2_to_level1 export */
        if((status = TryGetXMLOnlyChildAttributeValueByName (periodicTC_handle,
                "level2_to_level1", "export", filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strncmp(filenameError, "DEFAULT", 7) != 0)
        {
            /* parse level2_to_level1 export file */
            if((status = CreateExportArraySelectDefault(filenameError, relativePath,
                    &periodicTC->level[1], &periodicTC->level[2],
                    defaultLevels[periodicTC->ifRef][1].in,
                    defaultLevels[periodicTC->ifRef][2].in)) != 0)
            {
                levelError = EXPORT;
                return status;
            }
        }
        /* else defaultExport level2_to_level1 */
    }
    return 0;
}

void DisplayParsePeriodicTCError (int status, int errorPeriodicTC, char * msg,
        unsigned int maxMsgSize)
{
    unsigned int len = snprintf(msg, maxMsgSize, "Error in periodic TC %d",
            errorPeriodicTC);
    switch(levelError)
    {
        case TC_FORMAT: 
            len += snprintf(&msg[len], maxMsgSize-len, " format file \"%s\". ",
                    filenameError);
            DisplayCreateFormatArrayError(status, &msg[len], maxMsgSize-len);
            break;

        case EXPORT: 
            len += snprintf(&msg[len], maxMsgSize-len, " export file \"%s\". ",
                    filenameError);
            DisplayParsePeriodicTCExError(status, levelError, &msg[len],
                    maxMsgSize-len);
            break;

        default:
            len += snprintf(&msg[len], maxMsgSize-len, ": ");
            switch(status)
            {
                case WRONG_IFREF:
                    snprintf(&msg[len], maxMsgSize-len, "Error in periodic "
                            "TC %d: Referenced port not configured",
                            errorPeriodicTC);
                    break;
                    
                case WRONG_PERIODIC_TC_TAG:
                    snprintf(&msg[len], maxMsgSize-len, "Error in periodic "
                            "TC %d: Wrong tag", errorPeriodicTC);
                    break;
                    
                case WRONG_PERIODIC_TC_LEVEL:
                    snprintf(&msg[len], maxMsgSize-len, "Error in periodic "
                            "TC %d: Wrong level %d", errorPeriodicTC, 
                            errorNumbers);
                    break;

                default:
                    XMLerrorInfo(status, &msg[len], maxMsgSize-len);
                    break;
            }
            break;
    }
    return;
}
