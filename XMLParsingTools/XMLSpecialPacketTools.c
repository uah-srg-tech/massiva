/**
 * \file	XMLSpecialPacketTools.c
 * \brief	functions for parsing XML special packets (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/07/2014 at 16:26:15 
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <string.h>
#include "XMLTools.h"					/* GetXMLAttributeValueByName, ... */
#include "XMLFormatTools.h"				/* CreateFormatArray */
										/* level, portConfig */
#include "XMLFilterTools.h"				/* CreateFilterArray */
#include "XMLExportTools.h"				/* CreateExportArray */
#include "XMLImportTools.h"				/* CreateImportArray */

enum {
    SPECIAL_PACKET_NAME_TOO_LONG = -101,
    WRONG_IFREF = -102,
    WRONG_LEVELREF = -103,
    LESS_LEVELS_THAN_REF = -104,
    WRONG_DATAFIELD_NAME = -105,
    EXTRA_UPPER_LEVELS = -106,
    MISSING_UPPER_LEVELS = -107,
    WRONG_UPPER_LEVEL_ID = -108
};

static char filenameError[ATTR_SIZE]; 
static levelTypeError levelError;

static int ParseSpecialPacketPeriod(xmlNodePtr period_handle,
	special_packet_info * pSpecialInfo, char attrData[ATTR_SIZE]);
static int ParseSpecialPacketInterval(xmlNodePtr period_handle,
	special_packet_info * pSpecialInfo, char attrData[ATTR_SIZE]);

int ParseSpecialPacket(xmlNodePtr spPacket_handle, output * specialPacket,
	level defaultLevels[MAX_INTERFACES][MAX_LEVELS], portConfig * ports,
	special_packet_info * pSpecialInfo, unsigned int * levelErrorRef,
    char attrData[ATTR_SIZE], const char * relativePath)
{
    int status = 0;
    unsigned int lvlRef = 0, index = 0, aux_uint = 0, numberOfElements = 0, numberOfItems = 0;
    xmlNodePtr aux_handle = NULL, ul_handle = NULL;

    /* get special packet status */
    if((status = GetXMLAttributeValueByName (spPacket_handle, "status", attrData,
            70)) != 0)
    {
        return status;
    }
    if(strncmp(attrData, "enabled", 7) == 0)
    {
        pSpecialInfo->enabled = 1;
    }
    else if(strncmp(attrData, "disabled", 8) == 0)
    {
        pSpecialInfo->enabled = 0;
    }

    /* get special packet name */
    if((status = GetXMLAttributeLengthByName (spPacket_handle, "name",
            &aux_uint)) != 0)
    {
        return status;
    }
    if(aux_uint > MAX_FIELD_NAME_LEN)
    {
        return SPECIAL_PACKET_NAME_TOO_LONG;
    }
    if((status = GetXMLAttributeValueByName (spPacket_handle, "name",
            pSpecialInfo->name, MAX_FIELD_NAME_LEN)) != 0)
    {
        return status;
    }

    /* get specialPacket->ifRef */
    if((status = GetXMLAttributeValueByName (spPacket_handle, "ifRef",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    specialPacket->ifRef = atoi(attrData);
    /* check specialPacket->ifRef has been configured */
    if((specialPacket->ifRef >= MAX_INTERFACES) ||
            (ports[specialPacket->ifRef].portType == NO_PORT))
    {
        snprintf(filenameError, 70, "%d", specialPacket->ifRef);
        return WRONG_IFREF;
    }

    /* get levels */
    if((status = GetXMLAttributeValueByName (spPacket_handle, "levels",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    specialPacket->numberOfLevels = (unsigned int)strtoul(attrData, NULL, 0);

    /* get lvlRef */
    if((status = GetXMLAttributeValueByName (spPacket_handle, "levelRef",
            attrData, 70))!= 0)
    {
        return status;
    }
    lvlRef = atoi(attrData);
    /* check lvlRef has been configured for current specialPacket->ifRef */
    if(defaultLevels[specialPacket->ifRef][lvlRef].out.numberOfTMFields == 0)
    {
        return WRONG_LEVELREF;
    }
    if(specialPacket->numberOfLevels <= lvlRef)
    {
        return LESS_LEVELS_THAN_REF;
    }

    specialPacket->level =
            calloc(specialPacket->numberOfLevels, sizeof(levelOut));

    for(index=0; index<specialPacket->numberOfLevels; ++index)
    {
        specialPacket->level[index].TMFields = NULL;
        specialPacket->level[index].numberOfTMFields = 0;
        specialPacket->level[index].numberOfFDICTMFields = 0;
        specialPacket->level[index].crcTMFieldRefs = NULL;
        specialPacket->level[index].numberOfcrcTMFields = 0;
        specialPacket->level[index].boolVars = NULL;
        specialPacket->level[index].numberOfBoolVars = 0;
        specialPacket->level[index].filters = NULL;
        specialPacket->level[index].numberOfFilters = 0;
        specialPacket->level[index].typeOfFilters = FILTER_NOT_DEF;
        specialPacket->level[index].defaultFilter = 0;
        specialPacket->level[index].importInBytes = 0;
        specialPacket->level[index].virtualFields = NULL;
        specialPacket->level[index].numberOfVirtualFields = 0;
    }
    /* needed also other fields */
    for(index=0; index<=lvlRef; ++index)
    {
        specialPacket->level[index].importInBytes = 
                defaultLevels[0][index].out.importInBytes;
    }

    /* get input filter file */
    if((status = GetFileAsXMLChildOnlyAttribute (spPacket_handle, "inputFilter",
            filenameError, ATTR_SIZE)) != 0)
    {
        return status;
    }

    if(specialPacket->level[lvlRef].numberOfTMFields == 0)
        status = CreateFilterArray(filenameError, relativePath,
                &specialPacket->level[lvlRef].boolVars,
                &specialPacket->level[lvlRef].numberOfBoolVars,
                &specialPacket->level[lvlRef].filters,
                &specialPacket->level[lvlRef].numberOfFilters,
                &specialPacket->level[lvlRef].typeOfFilters,
                defaultLevels[specialPacket->ifRef][lvlRef].out.TMFields,
                defaultLevels[specialPacket->ifRef][lvlRef].out.numberOfTMFields);
    else
        status = CreateFilterArray(filenameError, relativePath,
                &specialPacket->level[lvlRef].boolVars,
                &specialPacket->level[lvlRef].numberOfBoolVars,
                &specialPacket->level[lvlRef].filters,
                &specialPacket->level[lvlRef].numberOfFilters,
                &specialPacket->level[lvlRef].typeOfFilters,
                specialPacket->level[lvlRef].TMFields,
                specialPacket->level[lvlRef].numberOfTMFields);

    /* parse extra filter file */
    if(status != 0)
    {
        *levelErrorRef = lvlRef;
        levelError = FILTER;
        return status;
    }

    /* get number of elements */
    /* 2 if nor UpperLevel nor period/range */
    /* 3 if UpperLevel (index 1) or period/range (index 2) */
    /* 4 if UpperLevel (index 1) and period/range (index 3) */
    GetXMLNumChildren(spPacket_handle, &numberOfElements);

    /* get index 1 handle and check it is "UpperLevels" */
    if((status = GetXMLChildElementByIndex(spPacket_handle, 1, &aux_handle)) != 0)
    {
        return status;
    }

    if((lvlRef+1) == specialPacket->numberOfLevels)
    {
        aux_uint = 0;
        if(strncmp((char*)aux_handle->name, "UpperLevels", 6) == 0)
        {
            return EXTRA_UPPER_LEVELS;
        }
    }
    else //if((lvlRef+1) != specialPacket->numberOfLevels)
    {
        aux_uint = 1;
        if(strncmp((char*)aux_handle->name, "UpperLevels", 6) != 0)
        {
            return MISSING_UPPER_LEVELS;
        }

        for(index=lvlRef+1; index<specialPacket->numberOfLevels; ++index)
        {
            *levelErrorRef = index;
            /* get upper level handle */
            if((status = GetXMLChildElementByIndex(aux_handle, index-lvlRef-1,
                    &ul_handle)) != 0)
            {
                return status;
            }
            /* get upper level level id */
            if((status = GetXMLAttributeValueByName (ul_handle, "level",
                    attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }
    
            if(getUnderscoredNumber(attrData) != index)
            {
                return WRONG_UPPER_LEVEL_ID;
            }
            levelError = TM_FORMAT;
            /* get upper level format file */
            if((status = GetFileAsXMLChildOnlyAttribute (ul_handle, "TMformat",
                    filenameError, ATTR_SIZE)) != 0)
            {
                return status;
            }
            /* parse upper level format file */
            if((status = CreateFormatArray(filenameError, relativePath,
                    &specialPacket->level[index].TMFields,
                    &specialPacket->level[index].numberOfTMFields,
                    &specialPacket->level[index].numberOfFDICTMFields,
                    &specialPacket->level[index].crcTMFieldRefs,
                    &specialPacket->level[index].numberOfcrcTMFields)) != 0)
            {
                return status;
            }
            levelError = FILTER;
            /* get upper level filter file */
            if((status = GetFileAsXMLChildOnlyAttribute (ul_handle, "inputFilter",
                    filenameError, ATTR_SIZE)) != 0)
            {
                return status;
            }
            /* parse upper level filter file */
            if((status = CreateFilterArray(filenameError, relativePath,
                    &specialPacket->level[index].boolVars,
                    &specialPacket->level[index].numberOfBoolVars,
                    &specialPacket->level[index].filters,
                    &specialPacket->level[index].numberOfFilters,
                    &specialPacket->level[index].typeOfFilters,
                    specialPacket->level[index].TMFields,
                    specialPacket->level[index].numberOfTMFields)) != 0)
            {
                return status;
            }
            levelError = IMPORT;
            /* get upper level import file */
            if((status = GetFileAsXMLChildOnlyAttribute (ul_handle,
                    "import_from_prev_Level", filenameError, ATTR_SIZE)) != 0)
            {
                return status;
            }
            /* parse upper level import file */
            if(index == lvlRef+1)
            {
                if((status = CreateImportArray(filenameError, relativePath,
                        &specialPacket->level[index].importInBytes,
                        &specialPacket->level[index].virtualFields,
                        &specialPacket->level[index].numberOfVirtualFields,
                        defaultLevels[specialPacket->ifRef][lvlRef].out.TMFields,
                        defaultLevels[specialPacket->ifRef][lvlRef].out.numberOfTMFields,
                        specialPacket->level[index].TMFields,
                        specialPacket->level[index].numberOfTMFields))
                        != 0)
                {
                    return status;
                }
            }
            else
            {
                if((status = CreateImportArray(filenameError, relativePath,
                        &specialPacket->level[index].importInBytes,
                        &specialPacket->level[index].virtualFields,
                        &specialPacket->level[index].numberOfVirtualFields,
                        specialPacket->level[index-1].TMFields,
                        specialPacket->level[index-1].numberOfTMFields,
                        specialPacket->level[index].TMFields,
                        specialPacket->level[index].numberOfTMFields))
                        != 0)
                {
                    return status;
                }
            }
        }
    }

    /* get printingData handle */
    if((status = GetXMLChildElementByIndex(spPacket_handle, 1+aux_uint,
            &aux_handle)) != 0)
    {
        return status;
    }

    /* get printingData printStatus */
    if((status = GetXMLAttributeValueByName (aux_handle, "printStatus",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(strncmp(attrData, "enabled", 7) == 0)
    {
        pSpecialInfo->print_enabled = 1;
    }
    else if(strncmp(attrData, "disabled", 8) == 0)
    {
        pSpecialInfo->print_enabled = 0;
    }
    levelError = CONFIG_OTHER;

    /* check if structuredData */
    GetXMLNumChildren(aux_handle, &numberOfItems);
    if(numberOfItems)
    {
        /* get structuredData */
        if((status = GetXMLChildElementByIndex (aux_handle, 0, &ul_handle))
                != 0)
        {
            return status;
        }

        if((status = GetXMLAttributeValueByName(ul_handle, "levelRef", attrData,
                70)) != 0)
        {
            return status;
        }
        aux_uint = atoi(attrData);
        if(aux_uint >= specialPacket->numberOfLevels)
        { 
            return LESS_LEVELS_THAN_REF;	
        }
        else if(aux_uint <= lvlRef)
        {
            unsigned int idx;
            /* we must copy level format to current level ref */
            specialPacket->level[aux_uint].numberOfTMFields = 
                    defaultLevels[0][aux_uint].out.numberOfTMFields;
            specialPacket->level[aux_uint].TMFields =
                    calloc(specialPacket->level[aux_uint].numberOfTMFields,
                    sizeof(formatField));
            for(idx=0; idx<specialPacket->level[aux_uint].numberOfTMFields; ++idx)
            {
                memcpy(&specialPacket->level[aux_uint].TMFields[idx],
                        &defaultLevels[0][aux_uint].out.TMFields[idx],
                        sizeof(formatField));
            }
        }
        pSpecialInfo->struct_show = aux_uint;
        if((status = GetXMLAttributeValueByName(ul_handle, "firstField", attrData,
                70)) != 0)
        {
            return status;
        }
        pSpecialInfo->struct_show_field = atoi(attrData);
    }								 

    /* if it is a period/intervalRange special packet, get the data */
    if(numberOfElements > 2)
    {
        /* period/intervalRange will be the last tag */
        if((status = GetXMLChildElementByIndex(spPacket_handle,
                numberOfElements-1, &aux_handle)) != 0)
        {
            return status;
        }

        if(strncmp((char*)aux_handle->name, "period", 6) == 0)
        {
            pSpecialInfo->type = SPECIAL_PERIODIC;
            status = ParseSpecialPacketPeriod(aux_handle, pSpecialInfo, attrData);
            if(status != 0)
                levelError = PERIOD;
        }
        else if(strncmp((char*)aux_handle->name, "intervalRange", 13) == 0)
        {
            pSpecialInfo->type = INTERVAL;
            status = ParseSpecialPacketInterval(aux_handle, pSpecialInfo, attrData);
            if(status != 0)
                levelError = INTERVAL;
        }
    }
    return status;
}

void DisplayParseSpecialPacketError (int status, int packetError, char * msg,
        int maxMsgSize)
{
    unsigned int len = snprintf(msg, maxMsgSize, "Error in SpecialPacket %d",
            packetError);
    switch(levelError)
    {
        case TM_FORMAT:
            len += snprintf(&msg[len], maxMsgSize-len,
                    " TMformat file \"%s\". ", filenameError);
            DisplayCreateFormatArrayError(status, &msg[len], maxMsgSize-len);
            break;

        case FILTER:
            len += snprintf(&msg[len], maxMsgSize-len,
                    " filter file \"%s\". ", filenameError);
            DisplayCreateFilterArrayError(status, &msg[len], maxMsgSize-len);
            break;

        case IMPORT:
            len += snprintf(&msg[len], maxMsgSize-len,
                    " import file \"%s\". ", filenameError);
            DisplayCreateImportArrayError(status, &msg[len], maxMsgSize-len);
            break;

        default:
            if(levelError == PERIOD)
                len += snprintf(&msg[len], maxMsgSize-len, " period: ");
            else if(levelError == INTERVAL)
                len += snprintf(&msg[len], maxMsgSize-len, " interval: ");
            else
                len += snprintf(&msg[len], maxMsgSize-len, ": ");
            switch (status)
            {
                case SPECIAL_PACKET_NAME_TOO_LONG:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Name too long. Maximum name length is %d",
                            MAX_FIELD_NAME_LEN-1);
                    break;

                case WRONG_IFREF:
                    snprintf(&msg[len], maxMsgSize-len,
                            "ifRef=\"%s\" not configured", filenameError);
                    break;

                case WRONG_LEVELREF:
                    snprintf(&msg[len], maxMsgSize-len,
                            "\"levelRef\" not found");
                    break;

                case LESS_LEVELS_THAN_REF:
                    snprintf(&msg[len], maxMsgSize-len, "\"levelRef\" "
                            "is bigger than \"levels\" configured");
                    break;

                case WRONG_DATAFIELD_NAME:
                    snprintf(&msg[len], maxMsgSize-len,
                            "\"dataFieldName\" not found");
                    break;

                case EXTRA_UPPER_LEVELS:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Not needed\"UpperLevels\" tag when equal "
                            "\"levels\" configured than \"levelRef\"");
                    break;

                case MISSING_UPPER_LEVELS:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Compulsory \"UpperLevels\" tag when more "
                            "\"levels\" configured than \"levelRef\"");
                    break;

                case WRONG_UPPER_LEVEL_ID:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Wrong \"UpperLevel\" level value"); 
                    break;

                default:
                    XMLerrorInfo(status, &msg[len], maxMsgSize-len);
                    break;
            }
            break;
    }
    memset(filenameError, 0, ATTR_SIZE); 
    return;
}

static int ParseSpecialPacketPeriod(xmlNodePtr period_handle,
    special_packet_info * pSpecialInfo, char attrData[ATTR_SIZE])
{
    int status = 0;
    //0 is min period, 1 for times smaller than min period
    //2 is max period, 3 for times bigger than max period
    //4 for last time
    if((status = GetXMLAttributeValueByName (period_handle, "min_value",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    pSpecialInfo->minValueMs = atof(attrData);
    if((status = GetXMLAttributeValueByName (period_handle, "min_unit",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(strcmp(attrData, "miliseconds") == 0)
    {
        pSpecialInfo->minValueMs *= 1;
    }
    else if(strcmp(attrData, "seconds") == 0)
    {
        pSpecialInfo->minValueMs *= 1000;
    }

    if((status = GetXMLAttributeValueByName (period_handle, "max_value",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    pSpecialInfo->maxValueMs = atof(attrData);
    if((status = GetXMLAttributeValueByName (period_handle, "max_unit",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(strcmp(attrData, "miliseconds") == 0)
    {
        pSpecialInfo->maxValueMs *= 1;
    }
    else if(strcmp(attrData, "seconds") == 0)
    {
        pSpecialInfo->maxValueMs *= 1000;
    }
    return status;
}

static int ParseSpecialPacketInterval(xmlNodePtr period_handle,
    special_packet_info * pSpecialInfo, char attrData[ATTR_SIZE])
{
    int status = 0;
    //0 is min packets in interval, 1 for times smaller than min
    //2 is max packets in interval, 3 for times bigger than max
    //4 is interval time, 5 for last time
    //6 for current interval packet counter
    if((status = GetXMLAttributeValueByName (period_handle, "min", attrData,
            70)) != 0)
    {
        return status;
    }
    pSpecialInfo->minValueMs = atof(attrData);

    if((status = GetXMLAttributeValueByName (period_handle, "max", attrData,
            70)) != 0)
    {
        return status;
    }
    pSpecialInfo->maxValueMs = atof(attrData);

    if((status = GetXMLAttributeValueByName (period_handle, "interval_value",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    pSpecialInfo->intervalTimeMs = atof(attrData);
    if((status = GetXMLAttributeValueByName (period_handle, "interval_unit",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(strcmp(attrData, "miliseconds") == 0)
    {
        pSpecialInfo->intervalTimeMs *= 1;
    }
    else if(strcmp(attrData, "seconds") == 0)
    {
        pSpecialInfo->intervalTimeMs *= 1000;
    }
    return status;
}