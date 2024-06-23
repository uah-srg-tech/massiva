/**
 * \file	XMLEgseConfigFileParser.c
 * \brief	functions for parsing egse_config.xml file (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		06/06/2016 at 12:52:35
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdlib.h>
#include <string.h>
#include "XMLTools.h"			/* GetXMLValueByTag, ... */
#include "XMLInterfaceTools.h"		/* ParseInterface */
					/* portConfig */
#include "XMLLevelTools.h"		/* DisplayParseLevelError */
					/* formatField, boolVar, export */
#include "XMLEgseConfigOptions.h"	/* GetGSSConfigOptions */
#include "XMLSpecialPacketTools.h"	/* ParseSpecialPacket */
#include "XMLEgsePeriodicTCs.h"		/* ParsePeriodicTC */
#include "XMLEgseGlobalVars.h"		/* ParseGlobalVar */
#include "XMLEgseMonitors.h"		/* ParseMonitor */
#include "XMLProtocolPacketTools.h"	/* ParseProtocolPacket */

enum {
    TOO_MUCH_INTERFACES = -201,
    TOO_MUCH_LEVELS = -202,
    TOO_MUCH_SPECIAL_PACKETS = -203,
    WRONG_SPECIAL_PACKET_ID = -204,
    TOO_MUCH_PERIODIC_TCS = -205,
    WRONG_PERIODIC_TC_ID = -206,
    TOO_MUCH_GLOBALVARS = -207,
    WRONG_GLOBALVAR_ID = -208,
    TOO_MUCH_MONITORS = -209,
    WRONG_IF_PROTOCOL_ID = -210,
    WRONG_IF_PROTOCOL_ID_LEVELS = -211,
    PROTOCOL_PACKET_ERROR = -212,
};

static enum {
    XMLFILE,
    INTERFACE,
    LEVEL,
    SPECIAL_PACKET,
    PERIODICTC,
    GLOBALVAR,
    CHARTS,
    MONITOR,
    GSS_INFO_HEADER,
    GSS_PROTOCOLS,
    GSS_CONFIG_OPTIONS
}configFileErrorType = XMLFILE;
static int errorNumbers;
static unsigned int idx, current;

int EgseConfigFileParser(xmlNodePtr root, gss_config * pConfig,
        portConfig * ports, serialConfig * serials, gss_options * pOptions,
        portProtocol * protocols, int configItems, char attrData[ATTR_SIZE],
        const char * relativePath, unsigned int testsInOtherFile)
{
    int status = 0;
    unsigned int SpecialPackets = 0, PeriodicTCs = 0, GlobalVarsMonitors = 0;
    unsigned int numberOfAttrProtocols = 0, numberOfAttr = 0, startAt = 0;
    xmlNodePtr children = NULL, child = NULL;
    char ProtocolPacketFilename[ATTR_SIZE];

    /* parse "gss_info" and "protocols" tags if exist, from GSS 3.1.0.0 * */
    /* before they were redundant tags "PacketFormats" and "TMTCFormats" */
    if((status = CheckParseGSSInfoHeader(root, pOptions)) != 0)
    {
        configFileErrorType = GSS_INFO_HEADER;
        return status;
    }
    if((status = CheckParseGSSProtocols(root, protocols, &numberOfAttrProtocols,
            attrData)) != 0)
    {
        configFileErrorType = GSS_PROTOCOLS;
        return status;
    }
    
    /* check number of interfaces */
    if((status = GetXMLChildElementByTag(root, "Interfaces", &children)) != 0)
    {
        return status;
    }
    GetXMLNumChildren(children, &pConfig->numberOfPorts);
    
    memset(ProtocolPacketFilename, 0, ATTR_SIZE);
    //check if there is a ProtocolPacketsFile attribute / tag
    if((status = GetXMLNumAttributes(children, &idx)) != 0)
    {
        return status;
    }
    if(idx)
    {
        //look for ProtocolPacketsFile as attribute
        xmlAttrPtr attr = NULL;
        if((status = GetXMLAttributeByIndex(children, 0, &attr)) != 0)
        {
            return status;
        }
        if((status = GetXMLAttributeValue(attr, ProtocolPacketFilename, ATTR_SIZE)) != 0)
        {
            return status;
        }
    }
    else
    {
        //look for ProtocolPacketsFile as tag
        if((status = GetXMLChildElementByIndex(children, 0, &child)) != 0)
        {
            return status;
        }
        if(strncmp((const char*)child->name, "ProtocolPacketsFile", 19) == 0)
        {
            if((status = GetXMLAttributeValueByName (child, "href",
                    ProtocolPacketFilename, ATTR_SIZE)) != 0)
            {
                return status;
            }
            ProtocolPacketFilename[strlen(ProtocolPacketFilename)-2] = '\0'; //remove "#/"
            startAt = 1;
            pConfig->numberOfPorts -= startAt;
        }
    }
    if(pConfig->numberOfPorts > MAX_INTERFACES)
    {
        errorNumbers = pConfig->numberOfPorts;
        pConfig->numberOfPorts = 0;
        return TOO_MUCH_INTERFACES;
    }
    for(idx=0; idx<MAX_INTERFACES; ++idx)
    {
        ports[idx].portType = NO_PORT;
    }
    pConfig->mainPort = -1;
    pConfig->mainPortAux = -1;

    /* Parse Interfaces and Levels */
    for(idx=0; idx<pConfig->numberOfPorts; ++idx)
    {
        /* get "Interface" handle */
        if((status = GetXMLChildElementByIndex(children, idx+startAt, &child)) != 0)
        {
            return status;
        }
        if(pConfig->mainPort == -1)
        {
            /* Check if is MainInterface */
            if(!strncmp((const char *)child->name, "MainInterface", 13))
                pConfig->mainPort = idx;
        }
        /* parse interface */
        if((status = ParseInterface(child, ports, serials, attrData, idx,
                relativePath)) != 0)
        {
            configFileErrorType = INTERFACE;
            return status;
        }

        /* get number of levels */
        GetXMLNumChildren(child, &pConfig->numberOfLevels[idx]);
        
        /* check if "ifConfig" was defined as a tag */
        xmlNodePtr firstChild = NULL;
        unsigned int firstElement = 0;
        if((status = GetXMLChildElementByIndex(child, 0, &firstChild)) != 0)
        {
            configFileErrorType = INTERFACE;
            return status;
        }
        if(!strncmp((char *)firstChild->name, "ifConfig", 8))
        {
            pConfig->numberOfLevels[idx]--;
            firstElement = 1;
        }
            
        if(pConfig->numberOfLevels[idx] > MAX_LEVELS)
        {
            errorNumbers = pConfig->numberOfLevels[idx];
            pConfig->numberOfLevels[idx] = 0;
            return TOO_MUCH_LEVELS;
        }
        
        attrData[0] = 0;
        ports[idx].protocolID = -1;
        /* parse protocolID, if exists */
        GetXMLNumAttributes(child, &numberOfAttr);
        if(numberOfAttr == 6)
        {
            //look for protocolID when ifConfig is an attribute
            xmlAttrPtr attribute = NULL;
            if((status = GetXMLAttributeByIndex(child, 5, &attribute)) != 0)
            {
                return status;
            }
            /* get protocolID */
            if((status = GetXMLAttributeValue(attribute, attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }
        }
        else if(numberOfAttr == 5)
        {
            //look for protocolID when ifConfig is a tag
            xmlAttrPtr attribute = NULL;
            if((status = GetXMLAttributeByIndex(child, 4, &attribute)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeName(attribute, attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }
            if(strncmp(attrData, "protocolID", 10) == 0)
            {
                /* get protocolID */
                if((status = GetXMLAttributeValue(attribute, attrData, ATTR_SIZE)) != 0)
                {
                    return status;
                }
            }
            else
            {
                attrData[0] = 0;
            }
            
        }
        if(attrData[0] != 0)
        {
            ports[idx].protocolID = atoi(attrData);
            if((ports[idx].protocolID+1) > (int)numberOfAttrProtocols)
            {
                current = ports[idx].protocolID;
                ports[idx].protocolID = -1;
                return WRONG_IF_PROTOCOL_ID;
            }
            if(protocols[ports[idx].protocolID].typeLevel > (pConfig->numberOfLevels[idx]-1))
            {
                current = protocols[ports[idx].protocolID].typeLevel;
                ports[idx].protocolID = -1;
                return WRONG_IF_PROTOCOL_ID_LEVELS;
            }
        }
        
        /* parse level 0 */
        if((status = ParseLevel(child, &pConfig->levels[idx][0], NULL, 0,
                firstElement, ports[idx].ioType, relativePath)) != 0)
        {
            configFileErrorType = LEVEL;
            return status;
        }
        /* parse levels 1+ */
        current = 0;
        for(current=1; current<pConfig->numberOfLevels[idx]; ++current)
        {
            /* parse level*/
            if((status = ParseLevel(child, &pConfig->levels[idx][current],
                    &pConfig->levels[idx][current-1], current,
                    current+firstElement, ports[idx].ioType, relativePath)) != 0)
            {
                configFileErrorType = LEVEL;
                return status;
            }
        }
    }
    
    
    /* parse ProtocolPackets if defined */
    if(ProtocolPacketFilename[0] != 0)
    {    
        if((status = ParseProtocolPackets(pConfig, ProtocolPacketFilename,
                relativePath)) != 0)
        {
            errorNumbers = status;
            return PROTOCOL_PACKET_ERROR;
        }
    }
    startAt = 0;
    
    if((status = GetGSSConfigOptions(root, children, &SpecialPackets,
            &PeriodicTCs, &GlobalVarsMonitors, configItems, testsInOtherFile)) != 0)
    {
        configFileErrorType = GSS_CONFIG_OPTIONS;
        return status;
    }

    /* if SpecialPackets are defined */
    if(SpecialPackets)
    {
        if((status = GetXMLChildElementByIndex(root, SpecialPackets, &children)) != 0)
        {
            return status;
        }
        GetXMLNumChildren(children, &pConfig->numberOfSpecialPackets);
        if(pConfig->numberOfSpecialPackets > MAX_SPECIAL_PACKETS)
        {
            errorNumbers = pConfig->numberOfSpecialPackets;
            pConfig->numberOfSpecialPackets = 0;
            return TOO_MUCH_SPECIAL_PACKETS;
        }
        pConfig->specialPackets =
                calloc(pConfig->numberOfSpecialPackets, sizeof(output));
        pConfig->specialInfo =
                calloc(pConfig->numberOfSpecialPackets, sizeof(special_packet_info));

        for(idx = 0; idx<pConfig->numberOfSpecialPackets; ++idx)
        {
            pConfig->specialPackets[idx].level = NULL;
            pConfig->specialPackets[idx].numberOfLevels = 0;
            pConfig->specialInfo[idx].print_enabled = 0;
            pConfig->specialInfo[idx].struct_show = -1;
            pConfig->specialInfo[idx].struct_show_field = 0;
            pConfig->specialInfo[idx].type = SPECIAL_NORMAL;
            pConfig->specialInfo[idx].minValueMs = 0.0;
            pConfig->specialInfo[idx].timesUnderValue = 0;
            pConfig->specialInfo[idx].maxValueMs = 0.0;
            pConfig->specialInfo[idx].timesOverValue = 0;
            pConfig->specialInfo[idx].lastTimeMs = 0.0;
            pConfig->specialInfo[idx].intervalTimeMs = 0;
            pConfig->specialInfo[idx].outOfMinMax = 0;
            pConfig->specialInfo[idx].currentIntervalValue = 0;
            pConfig->specialInfo[idx].counter = 0;
            pConfig->specialInfo[idx].enabled = 0;
            pConfig->specialInfo[idx].period_enabled = 0;
        }

        for(idx = 0; idx<pConfig->numberOfSpecialPackets; ++idx)
        {
            /* get SpecialPacket handle */
            if((status = GetXMLChildElementByIndex(children, idx, &child)) != 0)
            {
                return status;
            }
            /* get id */
            if((status = GetXMLAttributeValueByName(child, "id", attrData, ATTR_SIZE)) != 0)
            {
                return status;
            } 
            if (atoi(attrData) != (int)idx)
            {
                return WRONG_SPECIAL_PACKET_ID;
            }
            if((status = ParseSpecialPacket(child,
                    &pConfig->specialPackets[idx], pConfig->levels, ports,
                    &pConfig->specialInfo[idx], &current, attrData, relativePath)) != 0)
            {
                configFileErrorType = SPECIAL_PACKET;
                return status;
            }
        }
    }

    /* if PeriodicTCs are defined */
    if(PeriodicTCs)
    {
        if((status = GetXMLChildElementByIndex(root, PeriodicTCs, &children)) != 0)
        {
            return status;
        }
        GetXMLNumChildren(children, &pConfig->numberOfPeriodicTCs);
        if(pConfig->numberOfPeriodicTCs > MAX_PERIODIC_TCS)
        {
            errorNumbers = pConfig->numberOfPeriodicTCs;
            pConfig->numberOfPeriodicTCs = 0;
            return TOO_MUCH_PERIODIC_TCS;
        }
        pConfig->periodicTCs =
                calloc(pConfig->numberOfPeriodicTCs, sizeof(input));
        pConfig->periodicTCnames =
                calloc(pConfig->numberOfPeriodicTCs, sizeof(char *));
        for(idx = 0; idx < pConfig->numberOfPeriodicTCs; ++idx)
        {
            pConfig->periodicTCs[idx].level = NULL;
            pConfig->periodicTCnames[idx] =
                    calloc(MAX_FIELD_NAME_LEN, sizeof(char));
        }

        for(idx=0; idx<pConfig->numberOfPeriodicTCs; ++idx)
        {
            /* get PeriodicTC_level_X handle */
            if((status = GetXMLChildElementByIndex(children, idx, &child)) != 0)
            {
                return status;
            }

            /* get id */
            if((status = GetXMLAttributeValueByName(child, "id", attrData, ATTR_SIZE)) != 0)
            {
                return status;
            } 
            if (atoi(attrData) != (int)idx)
            {
                return WRONG_PERIODIC_TC_ID;
            }

            if((status = ParsePeriodicTC(child, &pConfig->periodicTCs[idx],
                    pConfig->periodicTCnames[idx], pConfig->levels, ports,
                    &current, attrData, relativePath)) != 0)
            {
                configFileErrorType = PERIODICTC;
                return status;
            }
        }
    }

    /* if GlobalVars & Monitors are also defined */
    if(GlobalVarsMonitors)
    {
        if((status = GetXMLChildElementByIndex(root, GlobalVarsMonitors, &children)) != 0)
        {
            return status;
        }
        GetXMLNumChildren(children, &pConfig->numberOfGlobalVars);
        if(pConfig->numberOfGlobalVars > MAX_GLOBALVARS)
        {
            errorNumbers = pConfig->numberOfGlobalVars;
            pConfig->numberOfGlobalVars = 0;
            return TOO_MUCH_GLOBALVARS;
        }
        pConfig->globalVars =
                calloc(pConfig->numberOfGlobalVars, sizeof(globalVar));

        for(idx=0; idx<pConfig->numberOfGlobalVars; ++idx)
        {
            /* get GlobalVar handle */
            if((status = GetXMLChildElementByIndex(children, idx, &child)) != 0)
            {
                return status;
            }
            /* get id */
            if((status = GetXMLAttributeValueByName (child, "id", attrData, ATTR_SIZE)) != 0) 
            {
                return status;
            } 
            if (atoi(attrData) != (int)idx)
            {
                return WRONG_GLOBALVAR_ID;
            }

            if((status = ParseGlobalVar (child, &pConfig->globalVars[idx],
                    pConfig->periodicTCs, pConfig->numberOfPeriodicTCs,
                    pConfig->specialPackets, pConfig->numberOfSpecialPackets,
                    pConfig->levels, ports, attrData)) != 0)
            {
                configFileErrorType = GLOBALVAR;
                return status;
            }
        }

        if((status = GetXMLChildElementByIndex(root, GlobalVarsMonitors+1, &children)) != 0)
        {
            return status;
        }
        /* get number of monitors */
        GetXMLNumChildren(children, &pConfig->numberOfMonitors);
    
        //check if there is a ChartsFile attribute / tag
        if((status = GetXMLNumAttributes(children, &idx)) != 0)
        {
            return status;
        }
        if(idx)
        {
            //look for ChartsFile as attribute
            xmlAttrPtr attr = NULL;
            if((status = GetXMLAttributeByIndex (children, 0, &attr)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeValue (attr, attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }
        }
        else
        {
            //look for ChartsFile as tag
            if((status = GetXMLChildElementByIndex (children, 0, &child)) != 0)
            {
                return status;
            }
            if(strncmp((const char*)child->name, "ChartsFile", 10) == 0)
            {
                if((status = GetXMLAttributeValueByName (child, "href",
                        attrData, ATTR_SIZE)) != 0)
                {
                    return status;
                }
                attrData[strlen(attrData)-2] = '\0'; //remove "#/"
                idx = 1;
                startAt = 1;
                pConfig->numberOfMonitors -= startAt;
            }
        }
        if(idx)
        {
            if((status = ParseCharts(pConfig, attrData, relativePath)) != 0)
            {
                configFileErrorType = CHARTS;
                return status;
            }
        }
        
        if(pConfig->numberOfMonitors > MAX_MONITORINGS)
        {
            errorNumbers = pConfig->numberOfMonitors;
            pConfig->numberOfMonitors = 0;
            return TOO_MUCH_MONITORS;
        }
        pConfig->monitors =
                calloc(pConfig->numberOfMonitors, sizeof(monitor));
        for(idx=0; idx<pConfig->numberOfMonitors; ++idx)
        {
            pConfig->monitors[idx].monitorRef.globalVarFilter.TMFields = NULL;
            pConfig->monitors[idx].monitorRef.globalVarFilter.numberOfTMFields = 0;
            pConfig->monitors[idx].monitorRef.globalVarFilter.numberOfFDICTMFields = 0;
            pConfig->monitors[idx].monitorRef.globalVarFilter.crcTMFieldRefs = NULL;
            pConfig->monitors[idx].monitorRef.globalVarFilter.numberOfcrcTMFields = 0;
            pConfig->monitors[idx].monitorRef.globalVarFilter.boolVars = NULL;
            pConfig->monitors[idx].monitorRef.globalVarFilter.numberOfBoolVars = 0;
            pConfig->monitors[idx].monitorRef.globalVarFilter.filters = NULL;
            pConfig->monitors[idx].monitorRef.globalVarFilter.numberOfFilters = 0;
            pConfig->monitors[idx].monitorRef.globalVarFilter.typeOfFilters = 0;
            pConfig->monitors[idx].monitorRef.globalVarFilter.defaultFilter = 0;
            pConfig->monitors[idx].monitorRef.globalVarFilter.importInBytes = 0;
            pConfig->monitors[idx].monitorRef.globalVarFilter.virtualFields = NULL;
            pConfig->monitors[idx].monitorRef.globalVarFilter.numberOfVirtualFields = 0;
        }

        ResetAlarmVals();
        for(idx=0; idx<pConfig->numberOfMonitors; ++idx)
        {
            /* get Monitor handle */
            if((status = GetXMLChildElementByIndex(children, idx+startAt, &child)) != 0)
            {
                return status;
            }

            if((status = ParseMonitor(child, &pConfig->monitors[idx],
                    pConfig->levels, pConfig->globalVars, pConfig->numberOfGlobalVars,
                    pConfig->charts, pConfig->numberOfCharts, attrData, relativePath)) != 0)
            {
                configFileErrorType = MONITOR;
                return status;
            }
            if((pConfig->monitors[idx].typeOfRef == GVR_FILTERED_TM) ||
                    (pConfig->monitors[idx].typeOfRef == GVR_FILTERED_TC))
            {
                ParseMonitorFilter(&pConfig->monitors[idx], pConfig->globalVars,
                        pConfig->levels, pConfig->periodicTCs, pConfig->specialPackets);
            }
        }
    }
    return 0;
}

void DisplayEgseConfigFileParserError (int status, portConfig * ports,
	char * msg, unsigned int maxMsgSize)
{
    unsigned int len = 0;
    switch(configFileErrorType)
    {
        case INTERFACE:
            DisplayParseInterfaceError (status, &ports[idx], msg, maxMsgSize);
            break;

        case LEVEL:
            DisplayParseLevelError(status, idx, current, msg, maxMsgSize);
            break;

        case SPECIAL_PACKET:
            DisplayParseSpecialPacketError(status, idx, msg, maxMsgSize);
            break;

        case PERIODICTC:
            DisplayParsePeriodicTCError(status, idx, msg, maxMsgSize);
            break;

        case GLOBALVAR:
            DisplayParseGlobalVarError(status, idx, msg, maxMsgSize);
            break;

        case CHARTS:
            DisplayParseChartsError(status, msg, maxMsgSize);
            break;

        case MONITOR:
            DisplayParseMonitorError(status, idx, msg, maxMsgSize);
            break;

        case GSS_INFO_HEADER:
            DisplayCheckParseGSSInfoHeaderError(status, msg, maxMsgSize);
            break;

        case GSS_PROTOCOLS:
            DisplayCheckParseGSSProtocolsError(status, msg, maxMsgSize);
            break;

        case GSS_CONFIG_OPTIONS:
            DisplayGetGSSConfigOptionsError(status, msg, maxMsgSize);
            break;
            
        case XMLFILE: default:
            switch (status)
            { 		  
                case TOO_MUCH_INTERFACES:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Too much interfaces (%d). Maximum are %d",
                            errorNumbers, MAX_INTERFACES);
                    break;

                case TOO_MUCH_LEVELS:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Too much levels (%d) at Interface %d. Maximum are %d",
                            errorNumbers, idx, MAX_LEVELS);
                    break;

                case TOO_MUCH_SPECIAL_PACKETS:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Too much special packets (%d). Maximum are %d",
                            errorNumbers, MAX_SPECIAL_PACKETS);
                    break;

                case WRONG_SPECIAL_PACKET_ID:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Wrong special packet %d id", idx);
                    break;

                case TOO_MUCH_PERIODIC_TCS:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Too much periodicTCs (%d). Maximum are %d",
                            errorNumbers, MAX_PERIODIC_TCS);
                    break;

                case WRONG_PERIODIC_TC_ID:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Wrong periodic TC %d id", idx);
                    break;

                case TOO_MUCH_GLOBALVARS:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Too much globalVars (%d). Maximum are %d",
                            errorNumbers, MAX_GLOBALVARS);
                    break;

                case WRONG_GLOBALVAR_ID:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Wrong GlobalVar %d id", idx);
                    break;

                case TOO_MUCH_MONITORS:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Too much monitors (%d). Maximum are %d",
                            errorNumbers, MAX_MONITORINGS);
                    break;

                case WRONG_IF_PROTOCOL_ID:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Wrong Interface %d protocolID (%d)", current, idx);
                    break;

                case WRONG_IF_PROTOCOL_ID_LEVELS:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Wrong Interface %d protocolID \"typeLevel\" (%d)",
                            current, idx);
                    break;
            
                case PROTOCOL_PACKET_ERROR:
                    DisplayParseProtocolPacketError(errorNumbers, msg, maxMsgSize);
                    break;

                default:
                    XMLerrorInfo(status, &msg[len], maxMsgSize-len);
                    break;
            }
            break;
    }
    return;
}
