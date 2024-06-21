/**
 * \file	XMLTPTools.c
 * \brief	functions for parsing XML test procedure files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		07/03/2013 at 17:32:12
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */

#include <string.h>
#include "XMLTools.h"			/* GetXMLAttributeByName, ... */
#include "XMLTPStepTools.h"		/* ParseStep */
#ifndef NO_QT
#include "../GuiClasses/ProgressBarsWrapper.h"
#endif

/*!
 * Maximum number of steps
 */
#define MAX_STEPS						1900

enum {
    TP_FILE_NOT_FOUND = -201,
    TP_PARSING_ERROR = -202,
    TOO_MUCH_STEPS = -203
};

static unsigned int step;
static char filenameError[MAX_PROC_FILE_LEN];
static levelTypeError levelError;
static int errorNumbers;

int ParseTestProcedure(const char * filename, gss_config * config,
	portProtocol * protocols, portConfig * ports, stepStruct ** steps,
        unsigned int * pNumberOfSteps, processStatus * pParsing)
{
    int status = 0, aux = 0;
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL, step_handle = NULL;
    char fullPath[MAX_STR_LEN];

    int lastStepWithOutputsAtPort[MAX_INTERFACES];
    for(aux=0; aux<MAX_INTERFACES; ++aux)
        lastStepWithOutputsAtPort[aux] = -1;

    strncpy(filenameError, filename, MAX_PROC_FILE_LEN);
    levelError = CONFIG_OTHER;

    /* look for file */
    if(fileExists(filenameError, NULL, fullPath, MAX_STR_LEN) == NULL)
    {
        return TP_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlReadFile(filenameError, NULL, 0);
    //doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return TP_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return TP_PARSING_ERROR;
    }
	
    /* get NumberOfSteps */
    GetXMLNumChildren(root, pNumberOfSteps);
    if(*pNumberOfSteps > MAX_STEPS)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        errorNumbers = *pNumberOfSteps;
        *pNumberOfSteps = 0;
        return TOO_MUCH_STEPS;
    }
    *steps = calloc((*pNumberOfSteps), sizeof(stepStruct));
#ifndef NO_QT
    SetProgressBarMaximum(BAR_TX_STEP, *pNumberOfSteps);
    SetProgressBarValue(BAR_TX_STEP, 0);
    ShowHideBar(BAR_TX_STEP, SHOW);
#endif
	
    for(step=0; step<*pNumberOfSteps; ++step)
    {
        steps[0][step].name[0] = 0;
        steps[0][step].prevStepIdRef = -1;
        steps[0][step].outIdRefFromPrevStep = -1;
        steps[0][step].mode = CONTINUOUS;
        steps[0][step].action = NULL;
        steps[0][step].inputs = NULL;
        steps[0][step].numberOfInputs = 0;
        steps[0][step].intervalInMs = 0.0;
        steps[0][step].outputsCheckmode = ALL;
        steps[0][step].outputs = NULL;
        steps[0][step].numberOfOutputs = 0;
        steps[0][step].outputsReceived = 0;
        for(aux=0; aux<MAX_INTERFACES; ++aux)
        {
            steps[0][step].numberOfOutputsAtPort[aux] = 0;
            steps[0][step].outputsReceivedAtPort[aux] = 0;
            steps[0][step].nextStepWithOutputsAtPort[aux] = -1;
        }
        steps[0][step].specials = NULL;
        steps[0][step].numberOfSpecials = 0;
        steps[0][step].concurrent.list = NULL;
        steps[0][step].concurrent.number = 0;
        steps[0][step].concurrent.next = 0;
        steps[0][step].concurrent.nextIsNotConc = 0;
        steps[0][step].replays = 1;
    }
	
    for(step=0; step<*pNumberOfSteps; ++step)
    {
        strncpy(filenameError, filename, MAX_PROC_FILE_LEN);
        levelError = TP_STEP;
        /* get step_handle handle */
        if((status = GetXMLChildElementByIndex(root, step, &step_handle)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
#ifndef NO_QT
        SetProgressBarValue(BAR_TX_STEP, step+1);
#endif
        if((status = ParseStep(doc, step_handle, config, protocols, ports,
                steps, step, filenameError, &levelError, pParsing, 0,
                *pNumberOfSteps, root)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            if(levelError == TP_STEP)
                strncpy(filenameError, filename, MAX_PROC_FILE_LEN);
            return status;
        }
        if(*pParsing == CANCELED)
        {
            break;
        }
        /* create valid next step with outputs for previous step */
        for(aux=0; aux<MAX_INTERFACES; ++aux)
        {
            if(steps[0][step].numberOfOutputsAtPort[aux] != 0)
            {
                if(lastStepWithOutputsAtPort[aux] != -1)
                    steps[0][lastStepWithOutputsAtPort[aux]].nextStepWithOutputsAtPort[aux] = step;
                lastStepWithOutputsAtPort[aux] = step;
            }
        }
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
#ifndef NO_QT
    if(*pParsing != CANCELED)
    {
        ShowHideBar(BAR_TX_STEP, HIDE);
    }
#endif
    return 0;
}

void DisplayParseTestProcedureError(int status, char * msg,
        unsigned int maxMsgSize)
{
    unsigned int len = 0;
    switch(levelError)
    {
        case CONFIG_OTHER:
            len = snprintf(msg, maxMsgSize, "Error in test procedure file %s: ",
                    filenameError);

            switch (status)
            {
                case TP_FILE_NOT_FOUND:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Test procedure file not found");
                    break;
                case TP_PARSING_ERROR:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Test procedure file parsing error");
                    break;

                case TOO_MUCH_STEPS:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Too much steps (%d). Maximum number are %d",
                            errorNumbers, MAX_STEPS);
                    break;

                default:
                    XMLerrorInfo(status, &msg[len], maxMsgSize-len);
                    break;
            }
            break;

        default:
            DisplayParseStepError(status, step, filenameError, levelError,
                    &msg[len], maxMsgSize-len);
            break;
    }
    memset(filenameError, 0, MAX_PROC_FILE_LEN);
    return;
}
