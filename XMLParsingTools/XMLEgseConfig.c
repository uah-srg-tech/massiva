/**
 * \file	XMLEgseConfig.c
 * \brief	functions for parsing egse_config.xml file (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		06/06/2016 at 12:52:35
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2016, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <libxml/xmlreader.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "XMLTools.h"				/* GetXMLValueByTag, ... */
#include "XMLEgseConfigFileParser.h"		/* EgseConfigFileParser */
						/* gss_config, levelOut */
#include "XMLProcedureListTools.h"		/* ParseProcedureList */

static enum {
    CONFIG_ENVIRONMENT_FILE_NOT_FOUND,
    CONFIG_ENVIRONMENT_FILE_PARSING_ERROR,
    CONFIG_FILE_NOT_GSS_CONFIG_OR_CAMPAIGN,
    CAMPAIGN_WRONG_ENVIRONMENT_FILE,
    ENVIRONMENT_FILE_NOT_GSS_ENVIRONMENT,
    SCENARIO_NOT_FOUND_IN_GSS_ENVIRONMENT,
    GSS_CONFIG_FILE,
    PROCEDURE_LIST,
    GSS_ERROR_XML
} egseConfigErrorType = GSS_ERROR_XML;

static char filenameError[MAX_STR_LEN]; 

int EgseConfig(const char * filename, gss_config * pConfig,
        portConfig * ports, serialConfig * serials, gss_options * pOptions,
        portProtocol * protocols)
{
    char attrData[ATTR_SIZE];
    int status = 0;
    unsigned int configItems = 0, port = 0, isCampaign = 0;
    char relativePath[130];
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL;
    xmlDocPtr environmentDoc = NULL;
    xmlNodePtr scenarioRoot = NULL;

    LIBXML_TEST_VERSION
    
    strncpy(filenameError, filename, MAX_STR_LEN);
    /* look for file */
    if(fileExists(filename, NULL, relativePath, 130) == NULL)
    {
        egseConfigErrorType = CONFIG_ENVIRONMENT_FILE_NOT_FOUND;
        return -1;
    }
    /* Open Document */
    doc = xmlParseFile(relativePath);
    if (doc == NULL)
    {
        egseConfigErrorType = CONFIG_ENVIRONMENT_FILE_PARSING_ERROR;
        return -1;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        egseConfigErrorType = CONFIG_ENVIRONMENT_FILE_PARSING_ERROR;
        return -1;
    }
    if((!strncmp((char*)root->name, "gss_campaign", 15)) ||
            (!strncmp((char*)root->name, "GSSCampaignCampaign", 25)))
    {
        isCampaign = 1;
    }
    else if((strncmp((char*)root->name, "gss_config", 10)) && 
            (strncmp((char*)root->name, "GSSConfigGSSConfig", 18)))
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        egseConfigErrorType = CONFIG_FILE_NOT_GSS_CONFIG_OR_CAMPAIGN;
        return -1;
    }

    unsigned int i, j;
    for(i=0; i<MAX_INTERFACES; ++i)
    {
        for(j=0; j<MAX_LEVELS; ++j)
        {    
            pConfig->levels[i][j].in.TCFields = NULL;
            pConfig->levels[i][j].in.numberOfTCFields = 0;
            pConfig->levels[i][j].in.numberOfFDICTCFields = 0;
            pConfig->levels[i][j].in.crcTCFieldRefs = NULL;
            pConfig->levels[i][j].in.numberOfcrcTCFields = NULL;
            pConfig->levels[i][j].in.exportFields = NULL;
            pConfig->levels[i][j].in.numberOfExportFields = 0;
            pConfig->levels[i][j].in.ActiveDICs = NULL;
            pConfig->levels[i][j].in.numberOfActiveDICs = 0;
            pConfig->levels[i][j].out.TMFields = NULL;
            pConfig->levels[i][j].out.numberOfTMFields = 0;
            pConfig->levels[i][j].out.numberOfFDICTMFields = 0;
            pConfig->levels[i][j].out.crcTMFieldRefs = NULL;
            pConfig->levels[i][j].out.numberOfcrcTMFields = NULL;
            pConfig->levels[i][j].out.boolVars = NULL;
            pConfig->levels[i][j].out.numberOfBoolVars = 0;
            pConfig->levels[i][j].out.filters = NULL;
            pConfig->levels[i][j].out.numberOfFilters = 0;
            pConfig->levels[i][j].out.typeOfFilters = FILTER_NOT_DEF;
            pConfig->levels[i][j].out.defaultFilter = 0;
            pConfig->levels[i][j].out.importInBytes = 0;
            pConfig->levels[i][j].out.virtualFields = NULL;
            pConfig->levels[i][j].out.numberOfVirtualFields = 0;
        }
    }
    
    /* get scenario */
    if(isCampaign)
    {
        /* look for environment file */
        char environmentFilename[MAX_STR_LEN];
        xmlNodePtr scenarioHandle = NULL;
        xmlNodePtr environmentRoot = NULL;
        memset(environmentFilename, 0, MAX_STR_LEN);
    
        /* get scenario element handle */
        if((status = GetXMLChildElementByTag(root, "Scenario",
                &scenarioHandle)) != 0)
        {
            xmlFreeDoc(doc);
            xmlFreeDoc(environmentDoc);
            xmlCleanupParser();
            return status;
        }
        
        /* get environment file */
        if((status = GetXMLValueFromAttributeOrHRefTag(scenarioHandle,
                "environment", environmentFilename, MAX_STR_LEN)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        strncpy(filenameError, environmentFilename, MAX_STR_LEN);
        
        /* get number of scenario */
        int scenarioNumber = -1;
        char scenarioName[ATTR_SIZE];
        if((status = GetXMLValueFromAttributeOrHRefTag(scenarioHandle,
                "scenarioRef", attrData, ATTR_SIZE)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        /* now we have to check wether it is full path or just scenario name */
        /* we made is by looking for @ */
        for(scenarioNumber=strlen(attrData)-1; scenarioNumber>=0; --scenarioNumber)
        {
            if(attrData[scenarioNumber] == '@')
                break;
        }
        if(scenarioNumber == -1)
        {
            /* Get scenario name for checking later */
            snprintf(scenarioName, ATTR_SIZE, "%s", attrData);
        }
        else
        {
            /* Check environment file is the same as defined */
            if(strncmp(attrData, environmentFilename, strlen(environmentFilename)))
            {
                xmlFreeDoc(doc);
                xmlCleanupParser();
                return CAMPAIGN_WRONG_ENVIRONMENT_FILE;
            }
            /* Get scenario number */
            scenarioNumber = atoi(&attrData[strlen(environmentFilename)+13]);
            //[NAME]#//@scenario.X
        }
        
        /* check if environment file exists */
        if(fileExists(environmentFilename, NULL, relativePath, 130) == NULL)
        {
            egseConfigErrorType = CONFIG_ENVIRONMENT_FILE_NOT_FOUND;
            return -1;
        }
        /* Open Document */
        environmentDoc = xmlParseFile(relativePath);
        if (doc == NULL)
        {
            egseConfigErrorType = CONFIG_ENVIRONMENT_FILE_PARSING_ERROR;
            return -1;
        }
        environmentRoot = xmlDocGetRootElement(environmentDoc);
        if (environmentRoot == NULL)
        {
            xmlFreeDoc(doc);
            xmlFreeDoc(environmentDoc);
            xmlCleanupParser();
            egseConfigErrorType = CONFIG_ENVIRONMENT_FILE_PARSING_ERROR;
            return -1;
        }
        if((strncmp((char*)environmentRoot->name, "gss_environment", 12)) && 
                (strncmp((char*)environmentRoot->name, "GSSEnvironmentEnvironment", 19)))
        {
            xmlFreeDoc(doc);
            xmlFreeDoc(environmentDoc);
            xmlCleanupParser();
            egseConfigErrorType = ENVIRONMENT_FILE_NOT_GSS_ENVIRONMENT;
            return -1;
        }
        
        /* Get scenario handler */
        /* Check if scenario id exists environment file */
        if(scenarioNumber == -1)    //from classic
        {
            unsigned int numberOfScenarios = 0;
            GetXMLNumChildren(environmentRoot, &numberOfScenarios);
            for(scenarioNumber=0; scenarioNumber<(int)numberOfScenarios; ++scenarioNumber)
            {
                if((status = GetXMLChildElementByIndex(environmentRoot,
                        scenarioNumber, &scenarioRoot)) != 0)
                {
                    xmlFreeDoc(doc);
                    xmlFreeDoc(environmentDoc);
                    xmlCleanupParser();
                    return status;
                }
                if((status = GetXMLAttributeValueByName (scenarioRoot, "name",
                        attrData, ATTR_SIZE)) != 0)
                {
                    xmlFreeDoc(doc);
                    xmlFreeDoc(environmentDoc);
                    xmlCleanupParser();
                    return status;
                }
                if(strncmp(scenarioName, attrData, strlen(scenarioName)) == 0)
                    break;//scenario found
            }
            if(scenarioNumber == (int)numberOfScenarios)
            {
                xmlFreeDoc(doc);
                xmlFreeDoc(environmentDoc);             
                strncpy(filenameError, scenarioName, MAX_STR_LEN);
                egseConfigErrorType = SCENARIO_NOT_FOUND_IN_GSS_ENVIRONMENT;
                return -1;
            }
        }
        else //from MDE
        {
            if((status = GetXMLChildElementByIndex(environmentRoot,
                    scenarioNumber, &scenarioRoot)) != 0)
            {
                xmlFreeDoc(doc);
                xmlFreeDoc(environmentDoc);
                xmlCleanupParser();
                return status;
            }
        }
        getRelativePath(environmentFilename, relativePath, 130);
    }
    else
    {
        scenarioRoot = root;
        getRelativePath(filename, relativePath, 130);
    }
        
    GetXMLNumChildren(scenarioRoot, &configItems);
    if((status = EgseConfigFileParser(scenarioRoot, pConfig, ports, serials,
            pOptions, protocols, configItems, attrData, relativePath,
            isCampaign)) != 0)
    {
        xmlFreeDoc(doc);
        if(isCampaign)
            xmlFreeDoc(environmentDoc);
        xmlCleanupParser();
        egseConfigErrorType = GSS_CONFIG_FILE;
        return status;
    }
    if(isCampaign)
    {
        xmlFreeDoc(environmentDoc);
        strncpy(filenameError, filename, MAX_STR_LEN);
        //getRelativePath(filename, relativePath, 130);
    }
    
    /* check test procedures */
    if((status = ParseProcedureList(root, pConfig)) != 0)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        snprintf(filenameError, MAX_STR_LEN, "3");
        egseConfigErrorType = PROCEDURE_LIST;
        return status;
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();

    /* find aux port if exists */
    for(port=0; port<pConfig->numberOfPorts; ++port)
    {
        if(ports[port].portType == SPW_TC_PORT)
        {
            pConfig->mainPortAux = status;
            break;
        }
    }
    return 0;
}

int DisplayEgseConfigError(int status, portConfig * ports, char * msg,
	unsigned int maxMsgSize)
{
    int ret = 0;
    unsigned int len = 0;
    switch(egseConfigErrorType)
    {
        case CONFIG_ENVIRONMENT_FILE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "\"%s\" not found", filenameError);
            ret = 1;
            break;
            
        case CONFIG_ENVIRONMENT_FILE_PARSING_ERROR:
            snprintf(msg, maxMsgSize, "\"%s\" parsing error ", filenameError);
            ret = 1;
            break;
            
        case CONFIG_FILE_NOT_GSS_CONFIG_OR_CAMPAIGN:
            snprintf(msg, maxMsgSize, "\"%s\" is not a \"<gss_config>\" file "
                    "nor a \"<gss_campaign file>\"",
                    filenameError);
            ret = 1;
            break;
            
        case CAMPAIGN_WRONG_ENVIRONMENT_FILE:
            snprintf(msg, maxMsgSize, "Different file defined in "
                    "\"<environment>\" and \"<scenario>\" tags");
            ret = 1;
            break;
            
        case ENVIRONMENT_FILE_NOT_GSS_ENVIRONMENT:
            snprintf(msg, maxMsgSize, "\"%s\" is not a \"<gss_environment>\" file",
                    filenameError);
            ret = 1;
            break;
            
        case SCENARIO_NOT_FOUND_IN_GSS_ENVIRONMENT:
            snprintf(msg, maxMsgSize, "Scenario \"%s\" not found in "
                    "environment file", filenameError);
            ret = 1;
            break;
            
        case GSS_CONFIG_FILE:
            len = snprintf(msg, maxMsgSize, "Error in \"%s\": ",
                    filenameError);
            DisplayEgseConfigFileParserError(status, ports, &msg[len],
                    maxMsgSize-len);
            break;

        case PROCEDURE_LIST:
            DisplayParseProcedureListError (status, msg, maxMsgSize);
            break;

        default:
            len = snprintf(msg, maxMsgSize, "XML error in \"%s\": ",
                    filenameError);
            XMLerrorInfo(status, &msg[len], maxMsgSize-len);
            break;
    }
    return ret;
}

void UnconfigEgseConfig(gss_config * pConfig)
{
    unsigned int i, j, k;

    /* free default levels of config */
    for(i=0; i<MAX_INTERFACES; ++i)
    {
        for(j=0; j<MAX_LEVELS; ++j)
        {
            if(pConfig->levels[i][j].in.TCFields != NULL)
            {
                free(pConfig->levels[i][j].in.TCFields);
                pConfig->levels[i][j].in.numberOfTCFields = 0;
            }
            if(pConfig->levels[i][j].in.crcTCFieldRefs != NULL)
            {
                for(k=0; k<pConfig->levels[i][j].in.numberOfFDICTCFields; ++k)
                {
                    if(pConfig->levels[i][j].in.crcTCFieldRefs[k] != NULL)
                        free(pConfig->levels[i][j].in.crcTCFieldRefs[k]);
                }
                //free(pConfig->levels[i][j].in.crcTCFieldRefs);
                pConfig->levels[i][j].in.numberOfFDICTCFields = 0;
            }
            if(pConfig->levels[i][j].in.numberOfcrcTCFields != NULL)
            {
                free(pConfig->levels[i][j].in.numberOfcrcTCFields);
            }
            if(pConfig->levels[i][j].in.exportFields != NULL)
            {
                free(pConfig->levels[i][j].in.exportFields);
                pConfig->levels[i][j].in.numberOfExportFields = 0;
            }
            if(pConfig->levels[i][j].in.ActiveDICs != NULL)
            {
                free(pConfig->levels[i][j].in.ActiveDICs);
                pConfig->levels[i][j].in.numberOfActiveDICs = 0;
            }
            if(pConfig->levels[i][j].out.TMFields != NULL)
            {
                free(pConfig->levels[i][j].out.TMFields);
                pConfig->levels[i][j].out.numberOfTMFields = 0;
            }
            if(pConfig->levels[i][j].out.crcTMFieldRefs != NULL)
            {
                for(k=0; k<pConfig->levels[i][j].out.numberOfFDICTMFields; ++k)
                {
                    if(pConfig->levels[i][j].out.crcTMFieldRefs[k] != NULL)
                        free(pConfig->levels[i][j].out.crcTMFieldRefs[k]);
                }
                //free(pConfig->levels[i][j].out.crcTMFieldRefs);
                pConfig->levels[i][j].out.numberOfFDICTMFields = 0;
            }
            if(pConfig->levels[i][j].out.numberOfcrcTMFields != NULL)
            {
                free(pConfig->levels[i][j].out.numberOfcrcTMFields);
            }
            if(pConfig->levels[i][j].out.boolVars != NULL)
            {
                free(pConfig->levels[i][j].out.boolVars);
                pConfig->levels[i][j].out.numberOfBoolVars = 0;
            }
            if(pConfig->levels[i][j].out.filters != NULL)
            {
                for(k=0; k<pConfig->levels[i][j].out.numberOfFilters; ++k)
                {
                    if(pConfig->levels[i][j].out.filters[k].boolVarRef != NULL)
                        free(pConfig->levels[i][j].out.filters[k].boolVarRef);
                }
                free(pConfig->levels[i][j].out.filters);
                pConfig->levels[i][j].out.numberOfFilters = 0;
            }
            if(pConfig->levels[i][j].out.virtualFields != NULL)
            {
                free(pConfig->levels[i][j].out.virtualFields);
                pConfig->levels[i][j].out.numberOfVirtualFields = 0;
                
            }
        }
        pConfig->numberOfLevels[i] = 0;
    }
    pConfig->numberOfPorts = 0;

    /* free specialPackets */
    if(pConfig->specialPackets != NULL)
    {
        for(i=0; i<pConfig->numberOfSpecialPackets; ++i)
        {
            if(pConfig->specialPackets[i].level != NULL)
            {
                for(j=0; j<pConfig->specialPackets[i].numberOfLevels; ++j)
                {
                    if(pConfig->specialPackets[i].level[j].TMFields != NULL)
                        free(pConfig->specialPackets[i].level[j].TMFields);
                    if(pConfig->specialPackets[i].level[j].boolVars != NULL)
                        free(pConfig->specialPackets[i].level[j].boolVars);
                    if(pConfig->specialPackets[i].level[j].filters != NULL)
                    {
                        for(k=0; k<pConfig->specialPackets[i].level[j].numberOfFilters; ++k)
                        {
                            if(pConfig->specialPackets[i].level[j].filters[k].boolVarRef != NULL)
                                free(pConfig->specialPackets[i].level[j].filters[k].boolVarRef);
                        }
                        free(pConfig->specialPackets[i].level[j].filters);
                    }
                    if(pConfig->specialPackets[i].level[j].numberOfcrcTMFields != NULL)
                        free(pConfig->specialPackets[i].level[j].numberOfcrcTMFields);
                    if(pConfig->specialPackets[i].level[j].virtualFields != NULL)
                        free(pConfig->specialPackets[i].level[j].virtualFields);
                }
                free(pConfig->specialPackets[i].level);
            }
        }
        free(pConfig->specialPackets);
        pConfig->specialPackets = NULL;
        free(pConfig->specialInfo);
        pConfig->specialInfo = NULL;
        pConfig->numberOfSpecialPackets = 0;
    }

    /* free periodicTcs */
    if(pConfig->periodicTCs != NULL)
    {
        for(i=0; i<pConfig->numberOfPeriodicTCs; ++i)
        {
            if(pConfig->periodicTCs[i].level != NULL)
            {
                for(j=0; j<pConfig->periodicTCs[i].numberOfLevels; ++j)
                {
                    if(pConfig->periodicTCs[i].level[j].TCFields != NULL)
                        free(pConfig->periodicTCs[i].level[j].TCFields);
                    if(pConfig->periodicTCs[i].level[j].crcTCFieldRefs != NULL)
                    {
                        for(k=0; k<pConfig->periodicTCs[i].level[j]. numberOfFDICTCFields; ++k)
                        {
                            if(pConfig->periodicTCs[i].level[j].crcTCFieldRefs[k] != NULL)
                                free(pConfig->periodicTCs[i].level[j].crcTCFieldRefs[k]);
                        }
                        free(pConfig->periodicTCs[i].level[j].crcTCFieldRefs);
                    }
                    if(pConfig->periodicTCs[i].level[j].exportFields != NULL)
                        free(pConfig->periodicTCs[i].level[j].exportFields);
                }
                free(pConfig->periodicTCs[i].level);
            }
        }
        free(pConfig->periodicTCs);
        pConfig->periodicTCs = NULL;
        pConfig->numberOfPeriodicTCs = 0;
    }

    if(pConfig->periodicTCnames != NULL)
    {
        for(i=0; i<pConfig->numberOfPeriodicTCs; ++i)
            free(pConfig->periodicTCnames[i]);
        free(pConfig->periodicTCnames);
        pConfig->periodicTCnames = NULL;
    }

    /* free globalVars */
    if(pConfig->globalVars != NULL)
    {
        free(pConfig->globalVars);
        pConfig->globalVars = NULL;
        pConfig->numberOfGlobalVars = 0;
    }

    /* free charts */
    if(pConfig->charts != NULL)
    {
        free(pConfig->charts);
        pConfig->charts = NULL;
        pConfig->numberOfCharts = 0;
    }

    /* free monitors */
    if(pConfig->monitors != NULL)
    {
        for(i=0; i<pConfig->numberOfMonitors; ++i)
        {
            if((pConfig->monitors[i].typeOfRef == GVR_FILTERED_TM) ||
                    (pConfig->monitors[i].typeOfRef == GVR_FILTERED_TC))
            {
                if(pConfig->monitors[i].monitorRef.globalVarFilter.TMFields != NULL)
                    free(pConfig->monitors[i].monitorRef.globalVarFilter.TMFields);
                if(pConfig->monitors[i].monitorRef.globalVarFilter.boolVars != NULL)
                    free(pConfig->monitors[i].monitorRef.globalVarFilter.boolVars);
                if(pConfig->monitors[i].monitorRef.globalVarFilter.filters != NULL)
                {
                    for(j=0; j<pConfig->monitors[i].monitorRef.globalVarFilter.numberOfFilters; ++j)
                    {
                        if(pConfig->monitors[i].monitorRef.globalVarFilter.filters[j].boolVarRef != NULL)
                            free(pConfig->monitors[i].monitorRef.globalVarFilter.filters[j].boolVarRef);
                    }
                    free(pConfig->monitors[i].monitorRef.globalVarFilter.filters);
                }
            }
        }
        free(pConfig->monitors);
        pConfig->monitors = NULL;
        pConfig->numberOfMonitors = 0;
    }
    /* free ProtocolPackets */
    if(pConfig->protocolPackets != NULL)
    {
        for(i=0; i<pConfig->numberOfProtocolPackets; ++i)
        {
            for(j=0; j<2; ++j)
            {
                if(pConfig->protocolPackets[i].in[j].TCFields != NULL)
                    free(pConfig->protocolPackets[i].in[j].TCFields);
                if(pConfig->protocolPackets[i].in[j].crcTCFieldRefs != NULL)
                {
                    for(k=0; k<pConfig->protocolPackets[i].in[j].numberOfFDICTCFields; ++k)
                    {
                        if(pConfig->protocolPackets[i].in[j].crcTCFieldRefs[k] != NULL)
                            free(pConfig->protocolPackets[i].in[j].crcTCFieldRefs[k]);
                    }
                    free(pConfig->protocolPackets[i].in[j].crcTCFieldRefs);
                }
                if(pConfig->protocolPackets[i].in[j].exportFields != NULL)
                    free(pConfig->protocolPackets[i].in[j].exportFields);
            }
        }
        free(pConfig->protocolPackets);
        pConfig->protocolPackets = NULL;
        pConfig->numberOfProtocolPackets = 0;
    }
    return;
}
