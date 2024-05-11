/**
 * \file	XMLProtocolPacketTools.c
 * \brief	functions for parsing XML level files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		26/07/2017 at 12:27:12
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2017, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <libxml/xmlreader.h>
#include <string.h>
#include "XMLTools.h"					/* GetXMLAttributeValueByName, ... */
#include "XMLFormatTools.h"				/* CreateFormatArray */
#include "XMLFilterTools.h"				/* CreateFilterArray */
#include "XMLExportTools.h"				/* CreateExportArray */

enum {
    PROTOCOL_PACKETS_FILE_NOT_FOUND = -101,
    PROTOCOL_PACKETS_FILE_PARSING_ERROR = -102,
    TOO_MUCH_PROTOCOL_PACKETS = -103,
    WRONG_PROTOCOL_PACKET_IFREF = -104,
    WRONG_PROTOCOL_PACKET_LEVELREF = -105,
    PROTOCOL_PACKET_NAME_TOO_LONG = -106,
};

static char filenameError[ATTR_SIZE];
static levelTypeError levelError;
static int errorNumbers = 0;
static unsigned int ppCntr = 0;

static int ParseProtocolPacket(xmlNodePtr pp_handle, protocolPacket * protocolPacket,
        unsigned int defaultNumberOfLevels[MAX_INTERFACES],
        level defaultLevels[MAX_INTERFACES][MAX_LEVELS], char attrData[ATTR_SIZE],
        const char * relativePath);

int ParseProtocolPackets(gss_config * pConfig, char attrData[ATTR_SIZE],
        const char * relativePath)
{
    int status = 0;
    unsigned int idx = 0;
    levelError = CONFIG_OTHER;
    xmlDocPtr doc = NULL;
    xmlNodePtr root, child = NULL;
    char fullPath[MAX_STR_LEN];
    
    memset(filenameError, 0, ATTR_SIZE);
    strncpy(filenameError, attrData, ATTR_SIZE);
    
    /* look for file */
    if(fileExists(filenameError, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
        return PROTOCOL_PACKETS_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return PROTOCOL_PACKETS_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return PROTOCOL_PACKETS_FILE_PARSING_ERROR;
    }

    GetXMLNumChildren(root, &pConfig->numberOfProtocolPackets);
    if(pConfig->numberOfProtocolPackets > MAX_PROTOCOL_PACKETS)
    {
        errorNumbers = pConfig->numberOfProtocolPackets;
        pConfig->numberOfProtocolPackets = 0;
        return TOO_MUCH_PROTOCOL_PACKETS;
    }

    pConfig->protocolPackets =
            calloc(pConfig->numberOfProtocolPackets, sizeof(protocolPacket));

    for(ppCntr=0; ppCntr<pConfig->numberOfProtocolPackets; ++ppCntr)
    {
        for(idx=0; idx<MAX_PROTOCOL_PACKET_LEVELS; ++idx)
        {
            pConfig->protocolPackets[ppCntr].in[idx].TCFields = NULL;
            pConfig->protocolPackets[ppCntr].in[idx].numberOfTCFields = 0;
            pConfig->protocolPackets[ppCntr].in[idx].crcTCFieldRefs = NULL;
            pConfig->protocolPackets[ppCntr].in[idx].numberOfFDICTCFields = 0;
            pConfig->protocolPackets[ppCntr].in[idx].numberOfcrcTCFields = 0;
            pConfig->protocolPackets[ppCntr].in[idx].exportFields = NULL;
            pConfig->protocolPackets[ppCntr].in[idx].numberOfExportFields = 0;
            pConfig->protocolPackets[ppCntr].in[idx].ActiveDICs = NULL;
            pConfig->protocolPackets[ppCntr].in[idx].numberOfActiveDICs = 0;
        }
        /* get ProtocolPacket handle */
        if((status = GetXMLChildElementByIndex(root, ppCntr, &child)) != 0)
        {
            return status;
        }
        if((status = ParseProtocolPacket(child,
                &pConfig->protocolPackets[ppCntr], pConfig->numberOfLevels,
                pConfig->levels, attrData, relativePath)) != 0)
        {
            return status;
        }
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}

static int ParseProtocolPacket(xmlNodePtr pp_handle, protocolPacket * protocolPacket,
        unsigned int defaultNumberOfLevels[MAX_INTERFACES],
        level defaultLevels[MAX_INTERFACES][MAX_LEVELS], char attrData[ATTR_SIZE],
        const char * relativePath)
{
    int status = 0;
    unsigned ppNameLen = 0;
    levelIn * defaultPreviousLevelIn = NULL;
    filenameError[0] = '0';
     
    /* get name (first checking name length)*/
    if((status = GetXMLAttributeLengthByName(pp_handle, "name", &ppNameLen)) != 0)
    {
        return status;
    }
    if(ppNameLen > (MAX_FIELD_NAME_LEN-1))
    {
        errorNumbers = ppNameLen;
        return PROTOCOL_PACKET_NAME_TOO_LONG;
    }
    if((status = GetXMLAttributeValueByName (pp_handle, "name", attrData, 40)) != 0)
    {
        return status;
    }
    strncpy(protocolPacket->name, attrData, MAX_FIELD_NAME_LEN);    
    
    /* get ProtocolPackets ifRef */ 
    if((status = GetXMLAttributeValueByName (pp_handle, "ifRef",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    protocolPacket->ifRef = atoi(attrData);
    if((protocolPacket->ifRef) > MAX_INTERFACES)
    {
        errorNumbers = MAX_INTERFACES;
        return WRONG_PROTOCOL_PACKET_IFREF;
    }
    /* get ProtocolPackets levelRef */ 
    if((status = GetXMLAttributeValueByName (pp_handle, "levelRef",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    protocolPacket->levelRef = atoi(attrData);
    if((protocolPacket->levelRef-1) > defaultNumberOfLevels[protocolPacket->ifRef])
    {
        errorNumbers = defaultNumberOfLevels[protocolPacket->ifRef];
        return WRONG_PROTOCOL_PACKET_LEVELREF;
    }
    defaultPreviousLevelIn = &defaultLevels[protocolPacket->ifRef][protocolPacket->levelRef-1].in;
    
    /* it is needed to parse first TCformat and then export */
    memset(filenameError, 0, ATTR_SIZE);
    /* get xml TCformat file */
    if((status = GetFileAsXMLChildOnlyAttribute (pp_handle, "format",
            filenameError, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(strncmp(filenameError, "NULL", 4))
    {
        /* parse TCformat file */
        if((status = CreateFormatArray(filenameError, relativePath,
                &protocolPacket->in[1].TCFields, &protocolPacket->in[1].numberOfTCFields,
                &protocolPacket->in[1].numberOfFDICTCFields, 
                &protocolPacket->in[1].crcTCFieldRefs,
                &protocolPacket->in[1].numberOfcrcTCFields)) != 0)
        {
            levelError = TC_FORMAT;
            return status;
        }
    }

    memset(filenameError, 0, ATTR_SIZE);
    /* get xml export file */
    if((status = GetFileAsXMLChildOnlyAttribute (pp_handle, "export",
            filenameError, ATTR_SIZE)) != 0)
    {
        return status;
    }
    /* parse export file */
    if((status = CreateExportArray(filenameError, relativePath,
            &protocolPacket->in[0].exportFields, &protocolPacket->in[0].numberOfExportFields,
            &protocolPacket->in[0].ActiveDICs, &protocolPacket->in[0].numberOfActiveDICs,
            defaultPreviousLevelIn->TCFields, defaultPreviousLevelIn->numberOfTCFields,
            protocolPacket->in[1].TCFields, protocolPacket->in[1].numberOfTCFields,
            defaultPreviousLevelIn->numberOfFDICTCFields)) != 0)
    {  
        levelError = EXPORT;
        return status;
    }
    return 0;
}

void DisplayParseProtocolPacketError (int status, char * msg,
        unsigned int maxMsgSize)
{
    unsigned int len = snprintf(msg, maxMsgSize, "Error in ProtocolPacket %d",
            ppCntr);
    switch(levelError)
    {
        case TC_FORMAT: 
            len += snprintf(&msg[len], maxMsgSize-len, " TCformat file \"%s\". ",
                    filenameError);
            DisplayCreateFormatArrayError(status, &msg[len], maxMsgSize-len);
            break;

        case EXPORT:
            len += snprintf(&msg[len], maxMsgSize-len, " export file \"%s\". ",
                    filenameError);
            DisplayCreateExportArrayError(status, &msg[len], maxMsgSize-len);
            break;

        default:
            len += snprintf(&msg[len], maxMsgSize-len, ": ");
            switch(status)
            {
                case PROTOCOL_PACKETS_FILE_NOT_FOUND:
                    snprintf(&msg[len], maxMsgSize-len,
                            "ProtocolPacketsFile \"%s\" not found",
                            filenameError);
                    break;
                    
                case PROTOCOL_PACKETS_FILE_PARSING_ERROR:
                    snprintf(&msg[len], maxMsgSize-len,
                            "ProtocolPacketsFile \"%s\" parsing error",
                            filenameError);
                    break;
                    
                case TOO_MUCH_PROTOCOL_PACKETS:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Too much ProtocolPackets (%d). Maximum are %d",
                            errorNumbers, MAX_PROTOCOL_PACKETS);
                    break;
                    
                case WRONG_PROTOCOL_PACKET_IFREF:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Wrong ifRef in ProtocolPacket %d. Maximum are %d",
                            ppCntr, errorNumbers);
                    break;
                    
                case WRONG_PROTOCOL_PACKET_LEVELREF:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Wrong levelRef in ProtocolPacket %d. Maximum are %d",
                            ppCntr, errorNumbers);
                    break;
                    
                case PROTOCOL_PACKET_NAME_TOO_LONG:
                    snprintf(&msg[len], maxMsgSize-len,
                            "ProtocolPacket %d name too big (%d). Max length is %d",
                            ppCntr, errorNumbers, MAX_FIELD_NAME_LEN-1);
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