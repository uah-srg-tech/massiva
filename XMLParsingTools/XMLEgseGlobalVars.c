/**
 * \file	XMLEgseGlobalVars.c
 * \brief	functions for parsing XML global variables (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/04/2013 at 14:18:34
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <string.h>
#include "XMLTools.h"					/* GetXMLAttributeValueByName, ... */
#include "../definitions.h"				/* monitor, globalVar, portConfig,... */

enum {
    WRONG_DATA_TYPE = -101,
    WRONG_IFREF = -102,
    WRONG_PERIODICTC_IDREF = -103,
    WRONG_SPECIALPACKET_IDREF = -104,
    WRONG_COMMAND_TYPE = -105,
    WRONG_LEVEL = -106,
    WRONG_FIELDREF = -107,
    WRONG_FORMULA_TC = -108,
    WRONG_FORMULA_OFFSET_SIZE = -109,
    WRONG_FORMULA_FIELD_TYPE = -110,
    GLOBALVAR_NAME_TOO_LONG = -111,
};

static int errorNumbers = 0;

int ParseGlobalVar (xmlNodePtr globalVar_handle, globalVar * globalVar,
	input * periodicTCs, unsigned int numberOfPeriodicTCs,
	output * specialPackets, unsigned int numberOfSpecialPackets,
	level defaultLevels[MAX_INTERFACES][MAX_LEVELS], portConfig * ports,
	char attrData[ATTR_SIZE])
{
    int status = 0;
    unsigned int nameLength = 0;
    xmlNodePtr reference_handle = NULL;
    /* get name */
    if((status = GetXMLAttributeLengthByName (globalVar_handle, "name",
            &nameLength)) != 0)
    {
        return status;
    }
    if(nameLength > MAX_FIELD_NAME_LEN)
    {
        errorNumbers = nameLength;
        return GLOBALVAR_NAME_TOO_LONG;
    }						 
    if((status = GetXMLAttributeValueByName (globalVar_handle, "name",
            globalVar->name, MAX_FIELD_NAME_LEN)) != 0)
    {
        return status;
    }

    /* get type */
    if((status = GetXMLAttributeValueByName (globalVar_handle, "type", attrData,
            70)) != 0)
    {
        return status;
    }
    if(strncmp(attrData, "uint", 4) == 0)
    {
        globalVar->dataType = GV_UINT;
    }
    else if(strncmp(attrData, "formula", 5) == 0)
    {
        globalVar->dataType = GV_FORMULA;
    }
    else
    {
        return WRONG_DATA_TYPE;
    }
    /* get initial_value */
    if((status = GetXMLAttributeValueByName (globalVar_handle, "initial_value",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    globalVar->value = atof(attrData);
    /* get type of command */
    if((status = GetXMLChildElementByIndex(globalVar_handle, 0,
            &reference_handle)) != 0)
    {
        return status;
    }
    char referenceType[25];
    unsigned int skipType = 0;
    if((status = SearchXMLAttributeValueByName(reference_handle, "type", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if((attrData[0] != '\0') && (strncmp(attrData, "gss_2:GSSScenario", 17) == 0))
    {
        strncpy(referenceType, &attrData[17], 25);//remove initial "gss_2:GSSScenario"
        referenceType[0] = 'r';//overwrite Uppercase "R"
        skipType = 1;
    }
    else
    {
        strncpy(referenceType, (char*)reference_handle->name, 25);
    }
    if(strncmp(referenceType, "referenceField", 14) == 0)
    {
        if((status = GetXMLAttributeValueByNameSkip(reference_handle, "type",
                skipType, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strncmp(attrData, "tc", 2) == 0)
        {
            globalVar->commandType = GLOBAL_TC;
        }
        else if(strncmp(attrData, "tm", 2) == 0)
        {
            globalVar->commandType = GLOBAL_TM;
        }
        else
        {
            return WRONG_COMMAND_TYPE;
        }
        /* get ifRef */
        if((status = GetXMLAttributeValueByName (reference_handle, "ifRef",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        globalVar->Interface = atoi(attrData);
        globalVar->idRef = -1;
    }
    else if(strncmp(referenceType, "referencePeriodicTC", 19) == 0)
    {
        globalVar->commandType = GLOBAL_TC_PERIOD;

        /* get periodicTC idRef */
        if((status = GetXMLAttributeValueByName (reference_handle, "idRef",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(!strncmp(attrData, "//@PeriodicTCs/@PeriodicTC_level_", 33))
            globalVar->idRef = atoi(&attrData[35]);
        else
            globalVar->idRef = atoi(attrData);
        /* check ref.idRef has been configured */
        if(numberOfPeriodicTCs < (unsigned int)(globalVar->idRef+1))
        {
            return WRONG_PERIODICTC_IDREF;
        }
        globalVar->Interface = periodicTCs[globalVar->idRef].ifRef;
    }
    else if(strncmp(referenceType, "referenceSpecialPacket", 22) == 0)
    {
        globalVar->commandType = GLOBAL_TM_SPECIAL;

        /* get referenceSpecialPacket idRef */
        if((status = GetXMLAttributeValueByName (reference_handle, "idRef",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(!strncmp(attrData, "//@SpecialPackets/@SpecialPacket.", 33))
            globalVar->idRef = atoi(&attrData[33]);
        else
            globalVar->idRef = atoi(attrData);
        /* check ref.idRef has been configured */
        if(numberOfSpecialPackets < (unsigned int)(globalVar->idRef+1))
        {
            return WRONG_SPECIALPACKET_IDREF;
        }
        globalVar->Interface = specialPackets[globalVar->idRef].ifRef;
    }
    else
    {
        return WRONG_COMMAND_TYPE;
    }
    /* check Interface has been configured  */
    if(ports[globalVar->Interface].portType == NO_PORT)
    {
        return WRONG_IFREF;
    }

    /* get level */
    if((status = GetXMLAttributeValueByName (reference_handle, "level",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    globalVar->level = atoi(attrData);
    /* check if level is too great */
    if((globalVar->level+1) > MAX_LEVELS)
    {
        return WRONG_LEVEL;
    }

    /* get fieldRef */
    if((status = GetXMLAttributeValueByName (reference_handle, "fieldRef",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    globalVar->fieldRef = atoi(attrData);
    /* check if this field has been configured for this level */

    unsigned int fields = 0;
    switch(globalVar->commandType)
    {
        case GLOBAL_TC:
            fields = defaultLevels[globalVar->Interface][globalVar->level].in.numberOfTCFields;
            break;
            
        case GLOBAL_TM:
            fields = defaultLevels[globalVar->Interface][globalVar->level].out.numberOfTMFields;
            break;
            
        case GLOBAL_TC_PERIOD:
            if(periodicTCs[globalVar->idRef].level[globalVar->level].numberOfTCFields == 0)
                fields = defaultLevels[globalVar->idRef][globalVar->level].in.numberOfTCFields;
            else
                fields = periodicTCs[globalVar->idRef].level[globalVar->level].numberOfTCFields;
            break;
            
        case GLOBAL_TM_SPECIAL:
            if(specialPackets[globalVar->idRef].level[globalVar->level].numberOfTMFields == 0)
                fields = defaultLevels[globalVar->idRef][globalVar->level].out.numberOfTMFields;
            else
                fields = specialPackets[globalVar->idRef].level[globalVar->level].numberOfTMFields;
            break;
            
        default:
            break;
    }
    if(fields < globalVar->fieldRef)
    {
        return WRONG_FIELDREF;
    }

    /* get offset */
    if((status = GetXMLAttributeValueByName (reference_handle, "offset",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    globalVar->offset = atoi(attrData);

    if((status = GetXMLAttributeValueByName (reference_handle, "size",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    globalVar->size = atoi(attrData);
    if(globalVar->dataType == GV_FORMULA)
    {
        if((globalVar->commandType == GLOBAL_TC) || (globalVar->commandType == GLOBAL_TC_PERIOD))
        {
            return WRONG_FORMULA_TC;
        }
        
        if((globalVar->offset != 0) || (globalVar->size != 0))
        {
            return WRONG_FORMULA_OFFSET_SIZE;
        }
        
        if(((globalVar->commandType == GLOBAL_TC) && (defaultLevels[globalVar->Interface][globalVar->level].in.TCFields[globalVar->fieldRef].type != CSFORMULAFIELD)) ||
                ((globalVar->commandType == GLOBAL_TM) && (defaultLevels[globalVar->Interface][globalVar->level].out.TMFields[globalVar->fieldRef].type != CSFORMULAFIELD)) ||
                ((globalVar->commandType == GLOBAL_TC_PERIOD) && (periodicTCs[globalVar->idRef].level[globalVar->level].TCFields[globalVar->fieldRef].type != CSFORMULAFIELD)) ||
                ((globalVar->commandType == GLOBAL_TM_SPECIAL) && (specialPackets[globalVar->idRef].level[globalVar->level].TMFields[globalVar->fieldRef].type != CSFORMULAFIELD)))
        {
            return WRONG_FORMULA_FIELD_TYPE;
        }
    }
    return 0;
}

void DisplayParseGlobalVarError (int status, int errorGlobalVar, char * msg,
        unsigned int maxMsgSize)
{
    unsigned int len = snprintf(msg, maxMsgSize, "Error in GlobalVar %d: ",
            errorGlobalVar);
    switch(status)
    {
        case WRONG_DATA_TYPE:
            snprintf(&msg[len], maxMsgSize-len,
                    "Wrong data type: only valid \"uint\" and \"float\"");
            break;

        case WRONG_IFREF:
            snprintf(&msg[len], maxMsgSize-len,
                    "Referenced port not configured");
            break;

        case WRONG_PERIODICTC_IDREF:
            snprintf(&msg[len], maxMsgSize-len,
                    "Referenced periodic TC not configured");
            break;

        case WRONG_SPECIALPACKET_IDREF:
            snprintf(&msg[len], maxMsgSize-len,
                    "Referenced Special Packet not configured");
            break;

        case WRONG_COMMAND_TYPE:
            snprintf(&msg[len], maxMsgSize-len,
                    "Wrong command type: only valid \"tc\" and \"tm\"");
            break;

        case WRONG_LEVEL:
            snprintf(&msg[len], maxMsgSize-len,
                    "Referenced level not configured");
            break;

        case WRONG_FIELDREF:
            snprintf(&msg[len], maxMsgSize-len,
                    "Referenced field not configured");
            break;

        case WRONG_FORMULA_TC:
            snprintf(&msg[len], maxMsgSize-len,
                    "If type=\"formula\", only valid tags are"
                    "\"referenceField\" with type=\"tm\" and"
                     "\"referenceSpecialPacket\"");
            break;
            
        case WRONG_FORMULA_OFFSET_SIZE:
            snprintf(&msg[len], maxMsgSize-len,
                    "If type=\"formula\", size and offset must be 0");
            break;

        case WRONG_FORMULA_FIELD_TYPE:
            snprintf(&msg[len], maxMsgSize-len,
                    "If type=\"formula\", fieldRef tag \"CSFormulaField\"");
            break;

        case GLOBALVAR_NAME_TOO_LONG:
            snprintf(&msg[len], maxMsgSize-len, "Name too long. "
                    "Maximum name length is %d", MAX_FIELD_NAME_LEN-1);
            break;

        default:
            len += snprintf(&msg[len], maxMsgSize-len, ": ");
            XMLerrorInfo(status, &msg[len], maxMsgSize-len);
            break;
    }
    return;
}