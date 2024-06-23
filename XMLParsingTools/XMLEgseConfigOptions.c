/**
 * \file	XMLEgseConfigOptions.c
 * \brief	functions for parsing egse_config.xml file (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/01/2014 at 17:18:29
 * Company:		Space Research Group, Universidad de Alcalï¿½.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdlib.h>
#include <string.h>
#include "XMLTools.h"			/* GetXMLValueByTag, ... */
#include "../definitions.h"		/* gss_config */

enum {
    WRONG_TAG_SPECIAL_OR_PERIODIC = -101,
    WRONG_TAG_GLOBALVARS_MONITORS = -102,
    WRONG_NUMBER_OF_OPTIONS = -103,
};

enum {
    GSS_INFO_ATTRIB_TOO_LONG = -101,
    WRONG_PRINT_STATUS = -102,
};

enum {
    TOO_MUCH_PROTOCOLS = -101,
    WRONG_PROTOCOL_ID = -102,
    WRONG_TYPE_LEVEL = -103,
    WRONG_TYPE_NAME = -104,
    WRONG_TYPE_OFFSET = -105,
    WRONG_SUBTYPE_NAME = -106,
    WRONG_SUBTYPE_OFFSET = -107,
};

static char infoAttribErrorTag[16], infoAttribErrorAttrib[20];
static unsigned int errorLenValue = 0, errorMaxLenValue = 0;
static unsigned int idx = 0;

static int parseGSSInfoTag(xmlNodePtr element, const char * attribute,
        char * string, unsigned int maxLen);
static int parseGSSInfoOptionLogs(xmlNodePtr element, const char * tag,
        enabled_status pPrintStatus[NUMBER_OF_LOGS]);
static int parseGSSDiscardFlags(xmlNodePtr element, const char * tag,
        enabled_status pDiscardStatus[NUMBER_OF_LOGS]);
static int parseEnDisOption(xmlNodePtr element, const char * tag,
        const char * attribute, enabled_status * pEnDisStatus);

int GetGSSConfigOptions(xmlNodePtr root, xmlNodePtr option,
	unsigned int * SpecialPackets, unsigned int * PeriodicTCs,
        unsigned int * GlobalVarsMonitors, unsigned int configItems,
        unsigned int testsInOtherFile)
{
    int status = 0;
    char tag1[15], tag2[15];

    /* if scenario inside environment and no option, it won't be enough to check */
    if(configItems <= 3)
    {
        return status;
    }
    /* Get Tag of the 3rd element for different parsers */
    if((status = GetXMLChildElementByIndex(root, 3, &option)) != 0)
    {
        return status;
    }
    strncpy(tag1, (char*) option->name, 15);
    switch(configItems+testsInOtherFile)
    {
        case 4:
            //nothing to configure
            break;
            
        case 5:
            //look for SpecialPackets or PeriodicTCs
            if(strncmp(tag1, "SpecialPackets", 14) == 0)
            {
                *SpecialPackets = 3;
            }
            else if(strncmp(tag1, "PeriodicTCs", 11) == 0)
            {
                *PeriodicTCs = 3;
            }
            else
            {
                idx = 3;
                status = WRONG_TAG_SPECIAL_OR_PERIODIC;
            }
            break;

        case 6:
            if((status = GetXMLChildElementByIndex(root, 4, &option)) != 0)
            {
                break;
            }
            strncpy(tag2, (char*) option->name, 15);
            //look for SpecialPackets or PeriodicTCs
            if((strncmp(tag1, "SpecialPackets", 14) == 0) &&
                    (strncmp(tag2, "PeriodicTCs", 11) == 0))
            {
                *SpecialPackets = 3;
                *PeriodicTCs = 4;
            }
            //look for GlobalVars and Monitors
            else if((strncmp(tag1, "GlobalVars", 10) == 0) &&
                    (strncmp(tag2, "Monitors", 8) == 0))
            {
                *GlobalVarsMonitors = 3;
            }
            else
            {
                idx = 3;
                status = WRONG_TAG_GLOBALVARS_MONITORS;
            }
            break;
            
        case 7:
            //look for SpecialPackets or PeriodicTCs
            if(strncmp(tag1, "SpecialPackets", 14) == 0)
            {
                *SpecialPackets = 3;
            }
            else if(strncmp(tag1, "PeriodicTCs", 11) == 0)
            {
                *PeriodicTCs = 3;
            }
            else
            {
                idx = 3;
                status = WRONG_TAG_SPECIAL_OR_PERIODIC;
                break;
            }
            
            //look for GlobalVars and Monitors
            if((status = GetXMLChildElementByIndex(root, 4, &option)) != 0)
            {
                break;
            }
            strncpy(tag1, (char*) option->name, 15);
            if((status = GetXMLChildElementByIndex(root, 5, &option)) != 0)
            {
                break;
            }
            strncpy(tag2, (char*) option->name, 15);
            if((strncmp(tag1, "GlobalVars", 10) == 0) &&
                (strncmp(tag2, "Monitors", 8) == 0))
            {
                *GlobalVarsMonitors = 4;
            }
            else
            {
                idx = 4;
                status = WRONG_TAG_GLOBALVARS_MONITORS;
            } 
            break;
            
        case 8:
            *SpecialPackets = 3;
            *PeriodicTCs = 4;
            *GlobalVarsMonitors = 5;
            break;
            
        default:
            idx = configItems;
            status = WRONG_NUMBER_OF_OPTIONS;
            break;
    }
    return status;
}

void DisplayGetGSSConfigOptionsError(int status, char * msg,
        unsigned int maxMsgSize)
{
    switch (status)
    {
        case WRONG_TAG_SPECIAL_OR_PERIODIC: 
            snprintf(msg, maxMsgSize, "Wrong tag %d: "
                    "Must be \"SpecialPackets\" or\"PeriodicTCs\"", idx);
            break;

        case WRONG_TAG_GLOBALVARS_MONITORS:
            snprintf(msg, maxMsgSize, "Wrong tag %d: "
                    "\"GlobalVars\" must be followed by \"Monitors\"", idx);
            break;

        case WRONG_NUMBER_OF_OPTIONS:
            snprintf(msg, maxMsgSize, "Wrong number of options (%d): There can be"
                    "\"SpecialPackets\", \"PeriodicTCs\", and "
                    "\"GlobalVars\" followed by \"Monitors\"", idx);
            break;

        default:
            XMLerrorInfo(status, msg, maxMsgSize);
            break;
    }
    return;
}

int CheckParseGSSInfoHeader(xmlNodePtr root, gss_options * pOptions)
{
    int status = 0;
    unsigned int idx=0;
    xmlNodePtr optionsNode = NULL;
    
    memset(pOptions->test_campaign, 0, MAX_FIELD_NAME_LEN);
    memset(pOptions->version, 0, MAX_DATE_VERSION_SIZE);
    memset(pOptions->date, 0, MAX_DATE_VERSION_SIZE);
    memset(pOptions->version_control_url, 0, MAX_MSG_SIZE);
    for(idx=0; idx<3; idx++)
    {
        pOptions->gss_info_print[idx] = DISABLED;
        pOptions->phy_header_print[idx] = DISABLED;
    }
    for(idx=0; idx<TEST_ERRORS; idx++)
    {
        pOptions->discardErrorFlags[idx] = 0;
    }
    
    /* Get Tag of the 1st element  */
    if((status = GetXMLChildElementByIndex(root, 0, &optionsNode)) != 0)
    {
        return status;
    }
    /* only parse portPhyHeaderOffset - it exists only from GSS 3.1.0.0 * */
    if(!strncmp("gss_options", (char*)optionsNode->name, 8))
    {
        if((status = parseGSSInfoTag(optionsNode, "test_campaign",
                pOptions->test_campaign, MAX_FIELD_NAME_LEN)) != 0)
        {
            return status;
        }
        if((status = parseGSSInfoTag(optionsNode, "version",
                pOptions->version, MAX_DATE_VERSION_SIZE)) != 0)
        {
            return status;
        }
        if((status = parseGSSInfoTag(optionsNode, "date",
                pOptions->date, MAX_DATE_VERSION_SIZE)) != 0)
        {
            return status;
        }
        /* version_control_url is not REQUIRED but IMPLIED */
        if((status = GetXMLChildNumAttributes(optionsNode, "gss_info", &idx))
                != 0)
        {
            return status;
        }
        if(idx == 4)
        {
            if((status = parseGSSInfoTag(optionsNode, "version_control_url",
                    pOptions->version_control_url, MAX_MSG_SIZE)) != 0)
            {
                return status;
            }
        }
        if((status = parseGSSInfoOptionLogs(optionsNode, "gss_info_print",
                pOptions->gss_info_print)) != 0)
        {
            return status;
        }
        if((status = parseGSSInfoOptionLogs(optionsNode, "phy_header_print",
                pOptions->phy_header_print)) != 0)
        {
            return status;
        }
        if((status = parseEnDisOption(optionsNode, "phy_header_print",
                "gssTabs", &pOptions->phy_header_gss_tabs)) != 0)
        {
            return status;
        }
        if((status = parseGSSDiscardFlags(optionsNode, "discardErrorFlags",
                pOptions->discardErrorFlags)) != 0)
        {
            return status;
        }
    }
    return status;
}

void DisplayCheckParseGSSInfoHeaderError(int status, char * msg,
        unsigned int maxMsgSize)
{
    switch (status)
    {
        case GSS_INFO_ATTRIB_TOO_LONG:
            snprintf(msg, maxMsgSize, "\"gss_info\" attribute %s too long (%d)."
                    " Max length is %d", infoAttribErrorAttrib, errorLenValue,
                    errorMaxLenValue);
            break;
            
        case WRONG_PRINT_STATUS:
            snprintf(msg, maxMsgSize, "Wrong Tag %s attribute %s value. "
                    "It must be \"enabled\" or \"disabled\"",
                    infoAttribErrorTag, infoAttribErrorAttrib);
            break;
        default:
            XMLerrorInfo(status, msg, maxMsgSize);
            break;
    }
}

static int parseGSSInfoTag(xmlNodePtr element, const char * attribute,
        char * string, unsigned int maxLen)
{
    int status = 0;
    if((status = GetXMLChildAttributeLengthByName (element, "gss_info",
            attribute, &errorLenValue)) != 0)
    {
        return status;
    }
    if(errorLenValue > maxLen)
    {
        memset(infoAttribErrorAttrib, 0, 20);
        strncpy(infoAttribErrorAttrib, attribute, 20);
        errorMaxLenValue = maxLen;
        return GSS_INFO_ATTRIB_TOO_LONG;
    }
    status = GetXMLChildAttributeValueByName (element, "gss_info",
            attribute, string, maxLen);
    return status;
}

static int parseGSSInfoOptionLogs(xmlNodePtr element, const char * tag,
        enabled_status pPrintStatus[NUMBER_OF_LOGS])
{
    int status = 0;
    if((status = parseEnDisOption(element, tag, "mainLog",
            &pPrintStatus[MAIN_LOG])) != 0)
    {
        return status;
    }
    if((status = parseEnDisOption(element, tag, "portLogs",
            &pPrintStatus[PORT_LOGS])) != 0)
    {
        return status;
    }
    status = parseEnDisOption(element, tag, "rawLog", &pPrintStatus[RAW_LOG]);
    return status;
}

static int parseGSSDiscardFlags(xmlNodePtr element, const char * tag,
        enabled_status pDiscardStatus[NUMBER_OF_LOGS])
{
    int status = 0;
    if((status = parseEnDisOption(element, tag, "txErrors",
            &pDiscardStatus[TX_ERRORS])) != 0)
    {
        return status;
    }
    if((status = parseEnDisOption(element, tag, "notExpectedPackets",
            &pDiscardStatus[NOT_EXPECTED_PACKETS])) != 0)
    {
        return status;
    }
    if((status = parseEnDisOption(element, tag, "filtersKo",
            &pDiscardStatus[FILTERS_KO])) != 0)
    {
        return status;
    }
    status = parseEnDisOption(element, tag, "validTimesKo",
            &pDiscardStatus[VALID_TIMES_KO]);
    return status;
}

static int parseEnDisOption(xmlNodePtr element, const char * tag,
        const char * attribute, enabled_status * pEnDisStatus)
{
    int status = 0;
    char status_string[9];
    memset(status_string, 0, 9);
    if((status = GetXMLChildAttributeValueByName (element, tag, attribute,
            status_string, 9)) != 0)
    {
        return status;
    }
    if(!strncmp("enabled", status_string, 7))
    {
        *pEnDisStatus = ENABLED;
    }
    else if(!strncmp("disabled", status_string, 7))
    {
        *pEnDisStatus = DISABLED;
    }
    else
    {
        memset(infoAttribErrorTag, 0, 16);
        strncpy(infoAttribErrorTag, tag, 16);
        memset(infoAttribErrorAttrib, 0, 20);
        strncpy(infoAttribErrorAttrib, attribute, 20);
        return WRONG_PRINT_STATUS;
    }
    return status;
}

int CheckParseGSSProtocols(xmlNodePtr root, portProtocol * protocols,
        unsigned int * pNumberOfProtocols, char attrData[ATTR_SIZE])
{
    int status = 0;
    unsigned int childNumber = 0, nameLen = 0;
    xmlNodePtr protocolsNode = NULL, protocolHndl = NULL, typeHndl = NULL;
    
    for(idx=0; idx<MAX_PROTOCOLS; ++idx)
    {
        protocols[idx].typeLevel = 0;
        protocols[idx].typeOffset = 0;
        memset(protocols[idx].typeName, 0, MAX_FIELD_NAME_LEN);
        protocols[idx].subtypeOffset = -1;
        memset(protocols[idx].subtypeName, 0, MAX_FIELD_NAME_LEN);
    }
    
    /* Get Tag of the 2st element  */
    if((status = GetXMLChildElementByIndex(root, 1, &protocolsNode)) != 0)
    {
        return status;
    }
    /* only parse "Protocols" - it exists only from GSS 3.1.0.0 * */
    if(!strncmp("Protocols", (char*)protocolsNode->name, 9))
    {
        GetXMLNumChildren(protocolsNode, pNumberOfProtocols);
        if(*pNumberOfProtocols > MAX_PROTOCOLS)
        {
            errorLenValue = *pNumberOfProtocols;
            *pNumberOfProtocols = 0;
            return TOO_MUCH_PROTOCOLS;
        }
        
        for(idx=0; idx<*pNumberOfProtocols; ++idx)
        {
            /* get "Protocol" handle */
            if((status = GetXMLChildElementByIndex(protocolsNode, idx,
                    &protocolHndl)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeValueByName(protocolHndl, "id", attrData,
                    70)) != 0)
            {
                return status;
            }
            if(atoi(attrData) != (int)idx)
            {
                return WRONG_PROTOCOL_ID;
            }
            if((status = GetXMLAttributeValueByName(protocolHndl, "typeLevel",
                    attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }
            if((protocols[idx].typeLevel = atoi(attrData)) > (MAX_LEVELS-1))
            {
                errorLenValue = protocols[idx].typeLevel;
                protocols[idx].typeLevel = 0;
                return WRONG_TYPE_LEVEL;
            }
            
            /* get "type" handle */
            if((status = GetXMLChildElementByTag(protocolHndl, "type", &typeHndl)) != 0)
            {
                return status;
            }
            /* get type name (first checking length) */
            if((status = GetXMLAttributeLengthByName(typeHndl, "name",
                    &nameLen)) != 0)
            {
                return status;
            }
            if(nameLen > (MAX_FIELD_NAME_LEN-1))
            {
                return WRONG_TYPE_NAME;
            }
            if((status = GetXMLAttributeValueByName(typeHndl, "name", attrData,
                    ATTR_SIZE)) != 0)
            {
                return status;
            }
            strncpy(protocols[idx].typeName, attrData, MAX_FIELD_NAME_LEN);
            
            /* get type offset */
            if((status = GetXMLAttributeValueByName(typeHndl, "offset",
                    attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }
            if((protocols[idx].typeOffset = atoi(attrData)) > (MAX_PACKET_SIZE-1))
            {
                errorLenValue = protocols[idx].typeOffset;
                protocols[idx].typeOffset = 0;
                return WRONG_TYPE_OFFSET;
            }
            
            /* subtype is not COMPULSORY */
            if((status = GetXMLNumChildren(protocolHndl, &childNumber)) != 0)
            {
                return status;
            }
            if(childNumber == 2)
            {
                /* get "subtype" handle */
                if((status = GetXMLChildElementByTag(protocolHndl, "subtype",
                        &typeHndl)) != 0)
                {
                    return status;
                }
                /* get type name (first checking length) */
                if((status = GetXMLAttributeLengthByName(typeHndl, "name",
                        &nameLen)) != 0)
                {
                    return status;
                }
                if(nameLen > (MAX_FIELD_NAME_LEN-1))
                {
                    return WRONG_SUBTYPE_NAME;
                }
                if((status = GetXMLAttributeValueByName(typeHndl, "name",
                        attrData, ATTR_SIZE)) != 0)
                {
                    return status;
                }
                strncpy(protocols[idx].subtypeName, attrData, MAX_FIELD_NAME_LEN);
            
                /* get subtype offset */
                if((status = GetXMLAttributeValueByName(typeHndl, "offset",
                        attrData, ATTR_SIZE)) != 0)
                {
                    return status;
                }
                if((protocols[idx].subtypeOffset = atoi(attrData)) > (MAX_PACKET_SIZE-1))
                {
                    errorLenValue = protocols[idx].subtypeOffset;
                    protocols[idx].subtypeOffset = -1;
                    return WRONG_SUBTYPE_OFFSET;
                }
            }
        }
    }
    return status;
}

void DisplayCheckParseGSSProtocolsError(int status, char * msg,
        unsigned int maxMsgSize)
{
    unsigned int len = 0;
    if(status == TOO_MUCH_PROTOCOLS)
    {
        snprintf(msg, maxMsgSize, "Too much protocols (%d). "
                "Maximum are %d", errorLenValue, MAX_PROTOCOLS);
    }
    else
    {
        len = snprintf(msg, maxMsgSize, "Error in protocol %d ", idx);
        switch (status)
        {
            case WRONG_PROTOCOL_ID:
                snprintf(&msg[len], maxMsgSize-len,
                        "\"id\". They must be correlative");
                break;

            case WRONG_TYPE_LEVEL:
                snprintf(&msg[len], maxMsgSize-len,
                        "\"typeLevel\". Maximum value is %d", MAX_LEVELS-1);
                break;

            case WRONG_TYPE_NAME:
                snprintf(&msg[len], maxMsgSize-len,
                        "type \"name\" too long (%d). Max length is %d",
                        errorLenValue, MAX_FIELD_NAME_LEN-1);
                break;

            case WRONG_TYPE_OFFSET:
                snprintf(&msg[len], maxMsgSize-len,
                        "type \"offset\". Maximum value is %d",
                        MAX_PACKET_SIZE-1);
                break;

            case WRONG_SUBTYPE_NAME:
                snprintf(&msg[len], maxMsgSize-len,
                        "subtype \"name\" too long (%d). Max length is %d",
                        errorLenValue, MAX_FIELD_NAME_LEN-1);
                break;

            case WRONG_SUBTYPE_OFFSET:
                snprintf(&msg[len], maxMsgSize-len,
                        "subtype \"offset\". Maximum value is %d",
                        MAX_PACKET_SIZE-1);
                break;

            default:
                XMLerrorInfo(status, &msg[len], maxMsgSize-len);
                break;
        }
    }
}
