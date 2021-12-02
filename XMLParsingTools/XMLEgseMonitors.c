/**
 * \file	XMLEgseMonitors.c
 * \brief	functions for parsing XML global variables (definition)
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
#include <string.h>
#include "XMLTools.h"			/* GetXMLAttributeValueByName, ... */
#include "XMLFilterTools.h"		/* CreateFilterArray */

enum {
    WRONG_GLOBAL_VAR_REF = -101,
    ERROR_WHILE_PARSING_FILTER = -102,
    WRONG_GVREF_TYPE = -103,
    WRONG_CHARTREF = -104,
    WRONG_CHART_MAX = -105,
    WRONG_ALARM_TYPE = -106,
    WRONG_MODIFY_TYPE = -107,
    WRONG_MONITOR_TYPE = -108,
    MONITOR_NAME_TOO_LONG = -109,
    ALARM_MSG_TOO_LONG = -110,
    TOO_MUCH_ALARM_VALS = -111,
};

enum {
    CHARTS_FILE_NOT_FOUND = -101,
    CHARTS_FILE_PARSING_ERROR = -102,
    TOO_MUCH_CHARTS = -103,
    WRONG_CHART_ID = -104,
    CHART_NAME_TOO_LONG = -105
};

static char filenameError[ATTR_SIZE];
static int errorNumbers = 0;
static unsigned int numberOfAlarmVals = 0;

void ResetAlarmVals(void)
{
    numberOfAlarmVals = 0;
}

int ParseMonitor(xmlNodePtr monitor_handle, monitor * monitor,
	level defaultLevels[MAX_INTERFACES][MAX_LEVELS],
        globalVar * globalVars, unsigned int numberOfGlobalVars,
        chart * pCharts, unsigned int numberOfCharts, char attrData[ATTR_SIZE],
        const char * relativePath)
{
    int status = 0;
    unsigned int nameLength = 0, numberOf = 0, gvref_handle_index = 0;
    xmlNodePtr gvref_handle = NULL;
    
    memset(filenameError, 0, ATTR_SIZE);
    errorNumbers = 0;

    /* get name */
    if((status = GetXMLAttributeLengthByName(monitor_handle, "name",
            &nameLength)) != 0)
    {
        return status;
    }
    if(nameLength > MAX_FIELD_NAME_LEN)
    {
        errorNumbers = nameLength;
        return MONITOR_NAME_TOO_LONG;
    }						 
    if((status = GetXMLAttributeValueByName(monitor_handle, "name",
            monitor->name, MAX_FIELD_NAME_LEN)) != 0)
    {
        return status;
    }
    
    if((status = GetXMLNumChildren(monitor_handle, &numberOf)) != 0)
    {
        return status;
    }
    
    char monitorType[10];
    unsigned int skipType = 0;
    if((status = SearchXMLAttributeValueByName(monitor_handle, "type", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if((attrData[0] != '\0') && (strncmp(attrData, "gss_2:GSSScenario", 17) == 0))
    {
        strncpy(monitorType, &attrData[17], 10);//remove initial "gss_2:GSSScenario"
        monitorType[0] += 32;//overwrite Uppercase
        skipType = 1;
    }
    else
    {
        strncpy(monitorType, (char*)monitor_handle->name, 10);
    }
    if((strncmp(monitorType, "plot", 4) == 0) &&
            (numberOf > 1))
    {
        gvref_handle_index = 1;
    }

    /* get handle of GVFiltered / GVPeriodic */
    if((status = GetXMLChildElementByIndex(monitor_handle, gvref_handle_index,
            &gvref_handle)) != 0)
    {
        return status;
    }

    /* get GlobalVarRef */
    if((status = GetXMLAttributeValueByName (gvref_handle, "GlobalVarRef",
            attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(!strncmp(attrData, "//@GlobalVars/@GlobalVar.", 25))
        monitor->globarVarRef = atoi(&attrData[25]);
    else if(!strncmp(&attrData[13],
            "/@GlobalVars/@GlobalVar.2", 24)) //@scenario.X/@GlobalVars/@GlobalVar.
        monitor->globarVarRef = atoi(&attrData[37]);
    else
        monitor->globarVarRef = atoi(attrData);
    if((monitor->globarVarRef+1) > numberOfGlobalVars)
    {
        return WRONG_GLOBAL_VAR_REF;
    }

    char gvType[11];
    if((status = SearchXMLAttributeValueByName(gvref_handle, "type", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(attrData[0] != '\0')
    {
        strncpy(gvType, &attrData[17], 11);//remove initial "gss_2:GSSScenario"
    }
    else
    {
        strncpy(gvType, (char*)gvref_handle->name, 11);
    }
    if(strncmp(gvType, "GVFiltered", 10) == 0)
    {
        if((globalVars[monitor->globarVarRef].commandType == GLOBAL_TC) ||
                (globalVars[monitor->globarVarRef].commandType == GLOBAL_TC_PERIOD))
        {
            monitor->typeOfRef = GVR_FILTERED_TC;
        }
        else if((globalVars[monitor->globarVarRef].commandType == GLOBAL_TM) ||
                (globalVars[monitor->globarVarRef].commandType == GLOBAL_TM_SPECIAL))
        {
            monitor->typeOfRef = GVR_FILTERED_TM;
        }

        /* get filter */
        if((status = TryGetXMLValueFromAttributeOrHRefTag (gvref_handle,
                "filter", filenameError, ATTR_SIZE, "DEFAULT", 7)) != 0)
        {
            return status;
        }
        if(strncmp(filenameError, "DEFAULT", 7) != 0) 
        {
            formatField * LevelFields = NULL;
            unsigned int numberOfLevelFields = 0;
            globalVar gvRef = globalVars[monitor->globarVarRef];
            
            if((gvRef.commandType == GLOBAL_TC) ||(gvRef.commandType == GLOBAL_TC_PERIOD))
            {
                LevelFields = defaultLevels[gvRef.Interface][gvRef.level].in.TCFields;
                numberOfLevelFields = defaultLevels[gvRef.Interface][gvRef.level].in.numberOfTCFields; 
            }
            else
            {
                LevelFields = defaultLevels[gvRef.Interface][gvRef.level].out.TMFields;
                numberOfLevelFields = defaultLevels[gvRef.Interface][gvRef.level].out.numberOfTMFields;
            }
                            
            /* parse level0_filter extra_filter */
            if((status = CreateFilterArray(filenameError, relativePath,
                    &monitor->monitorRef.globalVarFilter.boolVars,
                    &monitor->monitorRef.globalVarFilter.numberOfBoolVars,
                    &monitor->monitorRef.globalVarFilter.filters,
                    &monitor->monitorRef.globalVarFilter.numberOfFilters,
                    &monitor->monitorRef.globalVarFilter.typeOfFilters,
                    LevelFields, numberOfLevelFields)) != 0)
            {
                errorNumbers = status;
                return ERROR_WHILE_PARSING_FILTER;
            }
        }
        /* later we should fill also the TM fields */
    }
    else if(strncmp(gvType, "GVPeriodic", 10) == 0)
    {
        monitor->typeOfRef = GVR_PERIODIC;

        /* get period_value */
        if((status = GetXMLAttributeValueByName (gvref_handle, "period_value",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        monitor->monitorRef.periodInMs = atoi(attrData);

        /* get period_unit */
        if((status = GetXMLAttributeValueByName (gvref_handle, "period_unit",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strcmp(attrData, "miliseconds") == 0)
        {
            monitor->monitorRef.periodInMs *= 1;
        }
        else if(strcmp(attrData, "seconds") == 0)
        {
            monitor->monitorRef.periodInMs *= 1000;
        } 
    }
    else
    {
        return WRONG_GVREF_TYPE;
    }

    /* get type of monitor */
    if(strncmp(monitorType, "plot", 4) == 0)
    {
        monitor->type = PLOT;

        /* get chartRef */
        if((status = GetXMLValueFromAttributeOrHRefTag(monitor_handle,
                "chartRef", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        monitor->data.chartRef = atoi(attrData);
        if(monitor->data.chartRef >= numberOfCharts)
        {
            errorNumbers = numberOfCharts;
            return WRONG_CHARTREF;
        }
        pCharts[monitor->data.chartRef].numberOfPlots++;
        if(pCharts[monitor->data.chartRef].numberOfPlots > MAX_PLOTS_PER_CHART)
        {
            errorNumbers = monitor->data.chartRef;
            return WRONG_CHART_MAX;
        }
    }
    else if(strncmp(monitorType, "alarmMsg", 8) == 0)
    {
        monitor->type = ALARM_MSG;
        /* get type */
        if((status = GetXMLAttributeValueByNameSkip(monitor_handle, "type",
                skipType, attrData, ATTR_SIZE)) != 0)
        {
            return status;	  
        }
        if(strncmp(attrData, "alarm1", 6) == 0)
        {
            monitor->dataType.alarmType = ALARM1;
        }
        else if(strncmp(attrData, "alarm2", 6) == 0)
        {
            monitor->dataType.alarmType = ALARM2;
        }
        else if(strncmp(attrData, "alarm3", 6) == 0)
        {
            monitor->dataType.alarmType = ALARM3;
        }
        else
        {
            return WRONG_ALARM_TYPE;
        }
        /* get msg */
        if((status = GetXMLAttributeLengthByName(monitor_handle, "text",
                &nameLength)) != 0)
        {
            return status;	  
        }
        if(nameLength > MAX_ALARM_MSG_LEN)
        {
            errorNumbers = nameLength;
            return ALARM_MSG_TOO_LONG;
        }
        memset(monitor->data.msg, 0, MAX_ALARM_MSG_LEN);
        if((status = GetXMLAttributeValueByName(monitor_handle, "text",
                monitor->data.msg, MAX_ALARM_MSG_LEN)) != 0)
        {
            return status;	  
        }
    }
    else if(strncmp(monitorType, "modify", 6) == 0)
    {
        monitor->type = MODIFY;
        /* get type */
        if((status = GetXMLAttributeValueByNameSkip(monitor_handle, "type",
                skipType, attrData, ATTR_SIZE)) != 0)
        {
            return status;	  
        }
        if(strncmp(attrData, "increment1wrap", 15) == 0)
        {
            monitor->dataType.modifyType = INCREMENT_1_WRAP;
        }
        else if(strncmp(attrData, "increment", 9) == 0)
        {
            monitor->dataType.modifyType = INCREMENT;
        }
        else
        {
            return WRONG_MODIFY_TYPE;
        }
        /* get data (aux value) */
        if((status = GetXMLAttributeValueByName(monitor_handle, "value",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        monitor->data.value = atoi(attrData);
    }
    else if(strncmp(monitorType, "alarmVal", 8) == 0)
    {
        if((++numberOfAlarmVals) > MAX_NUMBER_OF_ALARM_VALS)
        {
            errorNumbers = numberOfAlarmVals;
            return TOO_MUCH_ALARM_VALS;
        }
        monitor->type = ALARM_VAL;

        /* get type */
        if((status = GetXMLAttributeValueByNameSkip(monitor_handle, "type",
                skipType, attrData, ATTR_SIZE)) != 0)
        {
            return status;	  
        }
        if(strncmp(attrData, "alarm1", 6) == 0)
        {
            monitor->dataType.alarmType = ALARM1;
        }
        else
        {
            return WRONG_ALARM_TYPE;
        }
        monitor->data.controlIdx = numberOfAlarmVals-1;
    }
    else
    {
        return WRONG_MONITOR_TYPE;
    }
    return 0;
}

void ParseMonitorFilter(monitor * monitor, globalVar * globalVars,
	level levels[MAX_INTERFACES][MAX_LEVELS], input * periodicTCs,
        output * specialPackets)
{
    int gVr = monitor->globarVarRef;

    /* using offset for import value for filter */
    monitor->monitorRef.globalVarFilter.importInBytes =
            levels[globalVars[gVr].Interface][globalVars[gVr].level].out.importInBytes +
            globalVars[gVr].offset;

    if(globalVars[gVr].commandType == GLOBAL_TC)
    {
        monitor->monitorRef.globalVarFilter.numberOfTMFields = 
                levels[globalVars[gVr].Interface][globalVars[gVr].level].in.numberOfTCFields;
        monitor->monitorRef.globalVarFilter.TMFields =
                calloc(monitor->monitorRef.globalVarFilter.numberOfTMFields,
                sizeof(formatField));
        memcpy(monitor->monitorRef.globalVarFilter.TMFields,
                levels[globalVars[gVr].Interface][globalVars[gVr].level].in.TCFields,
                sizeof(formatField) * monitor->monitorRef.globalVarFilter.numberOfTMFields);
    }
    else if(globalVars[gVr].commandType == GLOBAL_TM)
    {
        monitor->monitorRef.globalVarFilter.numberOfTMFields = 
                levels[globalVars[gVr].Interface][globalVars[gVr].level].out.numberOfTMFields;
        monitor->monitorRef.globalVarFilter.TMFields =
                calloc(monitor->monitorRef.globalVarFilter.numberOfTMFields,
                sizeof(formatField));
        memcpy(monitor->monitorRef.globalVarFilter.TMFields,
                levels[globalVars[gVr].Interface][globalVars[gVr].level].out.TMFields,
                sizeof(formatField) * monitor->monitorRef.globalVarFilter.numberOfTMFields);
    }
    else if(globalVars[gVr].commandType == GLOBAL_TC_PERIOD)
    {
        monitor->monitorRef.globalVarFilter.numberOfTMFields =
                periodicTCs[globalVars[gVr].idRef].level[globalVars[gVr].level].numberOfTCFields;
        monitor->monitorRef.globalVarFilter.TMFields =
                calloc(monitor->monitorRef.globalVarFilter.numberOfTMFields,
                sizeof(formatField));
        memcpy(monitor->monitorRef.globalVarFilter.TMFields,
                periodicTCs[globalVars[gVr].idRef].level[globalVars[gVr].level].TCFields,
                sizeof(formatField) * monitor->monitorRef.globalVarFilter.numberOfTMFields);
    }
    else if(globalVars[gVr].commandType == GLOBAL_TM_SPECIAL)
    {
        monitor->monitorRef.globalVarFilter.numberOfTMFields =
                specialPackets[globalVars[gVr].idRef].level[globalVars[gVr].level].numberOfTMFields;
        monitor->monitorRef.globalVarFilter.TMFields =
                calloc(monitor->monitorRef.globalVarFilter.numberOfTMFields,
                sizeof(formatField));
        memcpy(monitor->monitorRef.globalVarFilter.TMFields,
                specialPackets[globalVars[gVr].idRef].level[globalVars[gVr].level].TMFields,
                sizeof(formatField) * monitor->monitorRef.globalVarFilter.numberOfTMFields);
    }
    return;
}

void DisplayParseMonitorError (int status, int errorMonitor, char * msg,
        unsigned int maxMsgSize)
{
    unsigned int len = snprintf(msg, maxMsgSize, "Error in monitor %d: ",
            errorMonitor);
    switch(status)
    {
        case WRONG_GLOBAL_VAR_REF:
            snprintf(&msg[len], maxMsgSize-len, "GVarRef not defined");
            break;

        case ERROR_WHILE_PARSING_FILTER:
            len += snprintf(&msg[len], maxMsgSize-len,
                    "Error while parsing filter: %s. ", filenameError);
            DisplayCreateFilterArrayError(errorNumbers, &msg[len],
                    maxMsgSize-len);
            break;

        case WRONG_GVREF_TYPE:
            snprintf(&msg[len], maxMsgSize-len, "Wrong GVRef type. "
                    "Only valid \"GVFiltered\" or \"GVPeriodic\"");
            break;

        case WRONG_CHARTREF:
            snprintf(&msg[len], maxMsgSize-len,
                    "Plot \"chartRef\" not valid. Maximum number is %d",
                    errorNumbers);
            break;

        case WRONG_CHART_MAX:
            snprintf(&msg[len], maxMsgSize-len,
                    "Plot \"chartRef\" %d has too much plots configured."
                    "Maximum number is %d", errorNumbers, MAX_PLOTS_PER_CHART);
            break;

        case WRONG_ALARM_TYPE:
            snprintf(&msg[len], maxMsgSize-len, "Wrong alarm type. "
                    "\"alarm1\" / \"alarm2\" / \"alarm3\" for \"alarmMsg\" "
                    "and \"alarmVal\" for \"alarmMsg\"");
            break;

        case WRONG_MODIFY_TYPE:
            snprintf(&msg[len], maxMsgSize-len, "Wrong modify type. "
                    "Only valid \"increment\"");
            break;

        case WRONG_MONITOR_TYPE:
            snprintf(&msg[len], maxMsgSize-len, "Wrong Monitor type. "
                    "Only valid \"plot\", \"alarm\" or \"modify\"");
            break;

        case MONITOR_NAME_TOO_LONG:
            snprintf(&msg[len], maxMsgSize-len, "Name too long. "
                    "Maximum name length is %d", MAX_FIELD_NAME_LEN-1);
            break;

        case ALARM_MSG_TOO_LONG:
            snprintf(&msg[len], maxMsgSize-len, "Alarm message name too long (%d). "
                    "Maximum message length is %d",
                    errorNumbers, MAX_ALARM_MSG_LEN-1);
            break; 

        case TOO_MUCH_ALARM_VALS:
            snprintf(&msg[len], maxMsgSize-len,
                    "Too much alamVars. Maximum number is %d",
                    errorNumbers);
            break;

        default:
            XMLerrorInfo(status, &msg[len], maxMsgSize-len);
            break;
    }
    return;
}

int ParseCharts(gss_config * pConfig, char attrData[70],
        const char * relativePath)
{
    int status = 0;
    unsigned int idx = 0, nameLength = 0;
    xmlDocPtr doc = NULL;
    xmlNodePtr root, child = NULL;
    char fullPath[MAX_STR_LEN];
    
    memset(filenameError, 0, ATTR_SIZE);
    strncpy(filenameError, attrData, ATTR_SIZE);
    errorNumbers = 0;
    
    /* look for file */
    if(fileExists(filenameError, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
        return CHARTS_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return CHARTS_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return CHARTS_FILE_PARSING_ERROR;
    }

    GetXMLNumChildren(root, &pConfig->numberOfCharts);
    if(pConfig->numberOfCharts > MAX_CHARTS)
    {
        errorNumbers = pConfig->numberOfCharts;
        pConfig->numberOfCharts = 0;
        return TOO_MUCH_CHARTS;
    }

    pConfig->charts =
            calloc(pConfig->numberOfCharts, sizeof(protocolPacket));

    for(idx=0; idx<pConfig->numberOfCharts; ++idx)
    {
        chart * pChart = &pConfig->charts[idx];
        pChart->numberOfPlots = 0;

        /* get chart handle */
        if((status = GetXMLChildElementByIndex(root, idx, &child)) != 0)
        {
            return status;
        }

        /* get id */
        if((status = GetXMLAttributeValueByName (child, "id",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if (atoi(attrData) != (int)idx)
        {
            errorNumbers = idx;
            return WRONG_CHART_ID;
        }

        /* get name */
        if((status = GetXMLAttributeLengthByName (child, "name",
                &nameLength)) != 0)
        {
            return status;
        }
        if(nameLength > MAX_FIELD_NAME_LEN)
        {
            errorNumbers = idx;
            return CHART_NAME_TOO_LONG;
        }						 
        if((status = GetXMLAttributeValueByName (child, "name",
                pChart->name, MAX_FIELD_NAME_LEN)) != 0)
        {
            return status;
        }

        /* get y_units */
        if((status = GetXMLAttributeValueByName (child, "y_units",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;	  
        }
        snprintf(pChart->yAxisUnit, MAX_FIELD_NAME_LEN, "%s", attrData);

        /* get y_max */
        if((status = GetXMLAttributeValueByName (child, "y_max",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        pChart->yMax = atof(attrData);
    }
    return 0;
}

void DisplayParseChartsError (int status, char * msg,
        unsigned int maxMsgSize)
{
    switch(status)
    {
        case CHARTS_FILE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "ChartsFile \"%s\" not found",
                    filenameError);
            break;

        case CHARTS_FILE_PARSING_ERROR:
            snprintf(msg, maxMsgSize, "ChartsFile \"%s\" parsing error",
                    filenameError);
            break;

        case TOO_MUCH_CHARTS:
            snprintf(msg, maxMsgSize, "Too much Charts (%d). Maximum are %d",
                    errorNumbers, MAX_CHARTS);
            break;
                    
        case WRONG_CHART_ID:
            snprintf(msg, maxMsgSize, "Wrong chart %d id", errorNumbers);
            break;

        case CHART_NAME_TOO_LONG:
            snprintf(msg, maxMsgSize, "Chart %d name too long. "
                    "Maximum name length is %d", errorNumbers,
                    MAX_FIELD_NAME_LEN-1);
            break;

        default:
            XMLerrorInfo(status, msg, maxMsgSize);
            break;
    }
    return;
}