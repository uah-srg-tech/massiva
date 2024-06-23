/**
 * \file	XMLTPStepTools.c
 * \brief	functions for parsing XML test procedure files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/01/2014 at 15:30:23
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdlib.h>
#include <string.h>
#include "XMLTools.h"			/* GetXMLAttributeValueByName, ... */
#include "XMLTPInputTools.h"	/* ParseTestInput */
								/* stepStruct */
#include "XMLTPOutputTools.h"	/* ParseTestOutput, DisplayParseTestIOError */
#ifndef NO_QT
#include "../GuiClasses/ProgressBarsWrapper.h"
#endif
			
/*!
 * Maximum number of inputs
 */
#define MAX_INPUTS						17000
/*!
 * Maximum number of outputs
 */ 
#define MAX_OUTPUTS						5000
			
enum {
    WRONG_STEP_ID = -111,
    WRONG_PREV_STEP_ID = -112,
    WRONG_PREV_STEP_ID_OUTPUT = -113,
    WRONG_NULL_PREV_OUTPUT = -114,
    WRONG_PREV_OUTPUT = -115,
    WRONG_MODE = -116,
    TOO_MUCH_INPUTS = -117,
    WRONG_CHECKMODE = -118,
    SPECIAL_ID_BIGGER_THAN_SPECIAL_PACKETS_DEFINED = -119,
    TOO_MUCH_OUTPUTS = -120,
    NEEDED_CONCURRENT = -121,
    WRONG_NEXT_CONCURRENT = -122,
    WRONG_NEXT_CONCURRENT_IS_LAST = -123,
    WRONG_CONCURRENT_STEP = -124,
    WRONG_STEP_MODE = -125,
    WRONG_ACTION_TYPE = -126,
    WRONG_CONCURRENT_ACTION = -127,
};

static int levelErrorRef, errorNumbers;
static unsigned int io;

int ParseStep (xmlDocPtr doc, xmlNodePtr element, gss_config * config,
        portProtocol * protocols, portConfig * ports, stepStruct ** steps,
        unsigned int step, char fnameError[130], levelTypeError * levelError,
        processStatus * pParsing, int stepIdNotInFile,
        unsigned int numberOfSteps, xmlNodePtr elemParent)
{
    int status = 0;
    unsigned int numberOfElements = 0;
    char attrData[ATTR_SIZE];
    xmlNodePtr in_outs = NULL, in_out_level = NULL;
    xmlAttrPtr attr_handle = NULL;
    stepStruct * currentStep = &steps[0][step];
    char stepFilename[130];
    unsigned char existSpecials = 0, existOutputs = 0, existConcurrents = 0;

    levelErrorRef = 0;
    *levelError = TP_STEP;
    /* get id [step] */
    if((status = GetXMLAttributeValueByName (element, "id", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if ((atoi(attrData) != (int)step) && (!stepIdNotInFile))
    {
        errorNumbers = atoi(attrData);
        return WRONG_STEP_ID;
    }
    
    /* get name - in fact used as description */
    if((status = GetXMLAttributeValueByName (element, "name",
            currentStep->name, MAX_STEP_NAME_LEN)) != 0)
    {
        return status;
    }
    snprintf(attrData, 43, "tp_step%d", step); //to remove also "tp_stepX" names
    if((!strncmp(currentStep->name, "NULL", 4)) ||
            (!strncmp(currentStep->name, attrData, strlen(attrData))))
        currentStep->name[0] = 0;

    /* check if exist "prev_step_idref" and "output_idref_from_prev_step" */
    GetXMLNumAttributes(element, &numberOfElements);
    if((!stepIdNotInFile) && (numberOfElements > 3))
    {
        /* get prev_step_idref */
        if((status = GetXMLAttributeValueByName (element, "prev_step_idref",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strncmp(attrData, "NULL", 4) == 0)
        {
            currentStep->prevStepIdRef = -1;
        }
        else
        {
            /* check if EMF Fragment Path */
            if(strncmp(attrData, "//@step.", 8) == 0)
            {
                currentStep->prevStepIdRef = atoi(&attrData[8]);
            }
            else
            {
                currentStep->prevStepIdRef = atoi(attrData);
            }
            /* can't wait for same or future steps */
            if(currentStep->prevStepIdRef >= (int)step)
            {
                errorNumbers = currentStep->prevStepIdRef;
                return WRONG_PREV_STEP_ID;
            }
            /* waiting step must have outputs */
            else if(steps[0][currentStep->prevStepIdRef].numberOfOutputs == 0)
            {
                return WRONG_PREV_STEP_ID_OUTPUT;
            }
        }

        /* get output_idref_from_prev_step */
        if((status = GetXMLAttributeValueByName (element,
                "output_idref_from_prev_step", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strncmp(attrData, "NULL", 4) == 0)
        {
            if(currentStep->prevStepIdRef == -1)
            {
                currentStep->outIdRefFromPrevStep = -1;
            }
            else
            {
                /* if prevStepIdRef, outIdRefFromPrevStep can't be null */
                return WRONG_NULL_PREV_OUTPUT;
            }
        }
        else
        {
            /* check if EMF Fragment Path */
            if(strncmp(attrData, "//@step.", 8) == 0)
            {
                xmlXPathObjectPtr xPathObject = NULL;
                xmlNodePtr xPathNode = NULL;
                int stepNr = -1, outputNr = -1, outputLevel = -1;
                char xPath[60], * pch = strtok (attrData,"/@");
                while (pch != NULL)
                {
                    if(strncmp(pch, "step.", 5) == 0)
                    {
                        stepNr = atoi(&pch[5]);
                    }
                    else if(strncmp(pch, "output_level_", 13) == 0)
                    {
                        outputLevel = atoi(&pch[13]);
                        outputNr = atoi(&pch[15]);
                    }
                    else if(strncmp(pch, "output.", 7) == 0)
                    {
                        outputNr = atoi(&pch[7]);
                    }
                    pch = strtok (NULL, "/@");
                }
                
                if(outputLevel == -1)
                {
                    snprintf((char*)xPath, 60,
                            "//step[%d]/outputs/output[%d]",
                            stepNr+1, outputNr+1);
                }
                else
                {
                    snprintf((char*)xPath, 60,
                            "//step[%d]/outputs/output_level_%d[%d]",
                            stepNr+1, outputLevel, outputNr+1);
                }
                if((status = getXPathObject(doc, (xmlChar*)xPath,
                        &xPathObject)) != 0)
                {
                    return status;
                }
                else
                {
                    xPathNode = xPathObject->nodesetval->nodeTab[0];
                    if((status = GetXMLAttributeValueByName (xPathNode, "id",
                            attrData, ATTR_SIZE)) != 0)
                    {
                        return status;
                    }
                    currentStep->outIdRefFromPrevStep = atoi(attrData);
                    xmlXPathFreeObject(xPathObject);
                }
            }
            else
            {
                currentStep->outIdRefFromPrevStep = atoi(attrData);
            }
            /* output id from waiting step must exist */
            if(currentStep->outIdRefFromPrevStep >=
                    (int)steps[0][currentStep->prevStepIdRef].numberOfOutputs)
            {
                return WRONG_PREV_OUTPUT;
            }
        }
    }
    
    /* get mode */
    if((status = GetXMLAttributeValueByName (element, "mode", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(strncmp(attrData, "continuous", 10) == 0)
    {
        currentStep->mode = CONTINUOUS;
    }  
    else if(strncmp(attrData, "manual", 6) == 0)
    {
        currentStep->mode = MANUAL;
    }
    else if(strncmp(attrData, "concurrent", 10) == 0)
    {
        currentStep->mode = CONCURRENT;
    }
    else if(strncmp(attrData, "action", 6) == 0)
    {
        currentStep->mode = ACTION;
    }
    else
    {
        return WRONG_MODE;
    }

    if(currentStep->mode == ACTION)
    {
        if((status = GetXMLChildElementByIndex(element, 0, &in_outs)) != 0)
        {
            return status;
        }
        /* check if tag is action */
        if((status = SearchXMLAttributeValueByName (in_outs, "type", attrData,
                ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(((attrData[0] == '\0') && (strncmp((char*)in_outs->name, "action", 6) != 0)) ||
               ((attrData[0] != '\0') && (strncmp(attrData, "gss:GSSTestProcAction", 27) != 0)))
        {
            return WRONG_STEP_MODE;
        }
        /* create action */
        currentStep->action = calloc(1, sizeof(actionStruct));
        if((status = GetXMLAttributeValueByName (in_outs, "actionType",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strncmp(attrData, "instruction", 11) == 0)
        {
            currentStep->action->type = INSTRUCTION_ACTION;
        }
        else if(strncmp(attrData, "tmtc_checking", 13) == 0)
        {
            currentStep->action->type = TMTC_CHECKING_ACTION;
        }
        else if(strncmp(attrData, "checking", 8) == 0)
        {
            currentStep->action->type = CHECKING_ACTION;
        }
        else
        {
            return WRONG_ACTION_TYPE;
        }
        if((status = GetXMLAttributeValueByName (in_outs, "message",
                currentStep->action->actionMessage, MAX_ACTION_MSG_LEN)) != 0)
        {
            return status;
        }
            
        if((status = SearchXMLAttributeValueByName (in_outs, "delay_value",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(attrData[0] == '\0')
        {
            currentStep->action->delayInMs = 0;
        }
        else
        {
            currentStep->action->delayInMs = atoi(attrData);
            if((status = GetXMLAttributeValueByName (in_outs, "delay_unit",
                    attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }
            if(strcmp(attrData, "miliseconds") == 0)
            {
                currentStep->action->delayInMs *= 1;
            }
            else if(strcmp(attrData, "seconds") == 0)
            {
                currentStep->action->delayInMs *= 1000;
            }
        }
        if((status = SearchXMLAttributeValueByName (in_outs, "span_value",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(attrData[0] == '\0')
        {
            currentStep->action->spanInMs = 0;
        }
        else
        {
            currentStep->action->spanInMs = atoi(attrData);
            if((status = GetXMLAttributeValueByName (in_outs, "span_unit",
                    attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }
            if(strcmp(attrData, "miliseconds") == 0)
            {
                currentStep->action->spanInMs *= 1;
            }
            else if(strcmp(attrData, "seconds") == 0)
            {
                currentStep->action->spanInMs *= 1000;
            }
        }
    }
    else
    {
        /* check if tag is inputs */
        if((status = GetXMLChildElementByIndex(element, 0, &in_outs)) != 0)
        {
            return status;
        }
        if((status = SearchXMLAttributeValueByName (in_outs, "type", attrData,
                ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(((attrData[0] == '\0') && (strncmp((char*)in_outs->name, "inputs", 6) != 0)) ||
               ((attrData[0] != '\0') && (strncmp(attrData, "gss:GSSTestProcInputs", 27) != 0)))
        {
            return WRONG_STEP_MODE;
        }
        
        /* check if exists replays */	  								 
        if((status = GetXMLAttributeByIndex (element, -1, &attr_handle)) != 0)
        {
            return status;
        }				  		 
        if((status = GetXMLAttributeName (attr_handle, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }																					
        if(!strncmp(attrData, "replays", 7))
        {	   	 
            if((status = GetXMLAttributeValue (attr_handle, attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }				
            currentStep->replays = atoi(attrData);
        }

        /* get inputs */
        if((status = GetXMLChildElementByIndex(element, 0, &in_outs)) != 0)
        {
            return status;
        }
        GetXMLNumChildren(in_outs, &currentStep->numberOfInputs);
        if(currentStep->numberOfInputs > MAX_INPUTS)
        {
            errorNumbers = currentStep->numberOfInputs;
            currentStep->numberOfInputs = 0;
            return TOO_MUCH_INPUTS;
        }
#ifndef NO_QT
        SetProgressBarMaximum(BAR_INPUT, currentStep->numberOfInputs);
        SetProgressBarValue(BAR_INPUT, 0);
        ShowHideBar(BAR_INPUT, SHOW);
#endif
        currentStep->inputs = calloc(currentStep->numberOfInputs, sizeof(input));
        for(io=0; io<currentStep->numberOfInputs; ++io)
        {
            currentStep->inputs[io].ifRef = 0;
            currentStep->inputs[io].level = NULL;
            currentStep->inputs[io].delayInMs = 0;
            currentStep->inputs[io].numberOfLevels = 0;
        }

        strncpy(stepFilename, fnameError, 130);
        for(io=0; io<currentStep->numberOfInputs; ++io)
        {
            strncpy(fnameError, stepFilename, 130);
            /* get input_level_X handle */
            if((status = GetXMLChildElementByIndex(in_outs, io, &in_out_level)) != 0)
            {
                return status;
            }
            *levelError = TP_INPUT;

            if((status = ParseTestInput(in_out_level, config->levels,
                    &currentStep->inputs[io], levelError, ports, fnameError,
                    &levelErrorRef, attrData)) != 0)
            {
                return status;
            }
            if(*pParsing == CANCELED)
            {
                break;
            }
#ifndef NO_QT
            SetProgressBarValue(BAR_INPUT, io+1);
#endif
        }
    }

    /* check if exist outputs, special packets or concurrent steps*/
    GetXMLNumChildren(element, &numberOfElements);
    switch(numberOfElements)
    {
        case 1:
            /* not outputs nor special packets nor concurrent steps */
            currentStep->numberOfOutputs = 0;
            break;

        case 2:
            if((status = GetXMLChildElementByIndex(element, 1, &in_outs)) != 0)
            {
                return status;
            }
            if(strncmp((char*)in_outs->name, "outputs", 7) == 0)
            {
                existOutputs = 1;
            }
            else if(strncmp((char*)in_outs->name, "specialPackets", 14) == 0)
            {
                existSpecials = 1;
            }
            else //if(strncmp((char*)in_outs->name, "concurrent_steps", 16) == 0)
            {
                existConcurrents = 1;
            }
            break;

        case 3:
            if((status = GetXMLChildElementByIndex(element, 1, &in_outs)) != 0)
            {
                return status;
            }
            if(strncmp((char*)in_outs->name, "outputs", 7) == 0)
            {
                existOutputs = 1;
                if((status = GetXMLChildElementByIndex(element, 2, &in_outs)) != 0)
                {
                    return status;
                }
                if(strncmp((char*)in_outs->name, "specialPackets", 14) == 0)
                {
                    existSpecials = 2;
                }
                else //if(strncmp((char*)in_outs->name, "concurrent_steps", 16) == 0)
                {
                    existConcurrents = 2;
                }
            }
            else if(strncmp((char*)in_outs->name, "specialPackets", 14) == 0)
            {
                existSpecials = 1;
                existConcurrents = 2;
            }
            break;

        case 4:
            existOutputs = 1;
            existSpecials = 2;
            existConcurrents = 3;
            break;
    }
    if(!existConcurrents && (currentStep->mode == CONCURRENT))
    {
        return NEEDED_CONCURRENT;
    }

    if(*pParsing == CANCELED)
    {
        return 0;
    }	
    *levelError = TP_STEP;

    if(existOutputs)
    {
        if((status = GetXMLChildElementByIndex(element, 1, &in_outs)) != 0)
        {
            return status;
        }
        /* get checkmode */
        if((status = GetXMLAttributeValueByName (in_outs, "checkmode",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strncmp(attrData, "allunsorted", 11) == 0)
        {
            currentStep->outputsCheckmode = ALLUNSORTED;
        }  
        else if(strncmp(attrData, "all", 3) == 0)
        {
            currentStep->outputsCheckmode = ALL;
        }
        else if(strncmp(attrData, "any", 3) == 0)
        {
            currentStep->outputsCheckmode = ANY;
        }
        else
        {
            return WRONG_CHECKMODE;
        }

        /* get valid_time_interval_value */
        if((status = GetXMLAttributeValueByName(in_outs,
                "valid_time_interval_value", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        currentStep->intervalInMs = atof(attrData);

        /* get valid_time_interval_unit */
        if((status = GetXMLAttributeValueByName(in_outs,
                "valid_time_interval_unit", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strcmp(attrData, "miliseconds") == 0)
        {
            currentStep->intervalInMs *= 1.0;
        }
        else if(strcmp(attrData, "seconds") == 0)
        {
            currentStep->intervalInMs *= 1000.0;
        }
        /* NOTE: "valid_time_interval_value" and "valid_time_interval_unit" 
         *  are parsed but not used */

        GetXMLNumChildren(in_outs, &currentStep->numberOfOutputs);
        if(currentStep->numberOfOutputs > MAX_OUTPUTS)
        {
            errorNumbers = currentStep->numberOfOutputs;
            currentStep->numberOfOutputs = 0;
            return TOO_MUCH_OUTPUTS;
        }
#ifndef NO_QT
        SetProgressBarMaximum(BAR_RX_STEP, currentStep->numberOfOutputs);
        SetProgressBarValue(BAR_RX_STEP, 0);
        ShowHideBar(BAR_RX_STEP, SHOW);
#endif
        currentStep->outputs =
                calloc(currentStep->numberOfOutputs, sizeof(output));
        for(io=0; io<currentStep->numberOfOutputs; ++io)
        {
            currentStep->outputs[io].ifRef = 0;
            currentStep->outputs[io].level = NULL;
            currentStep->outputs[io].numberOfLevels = 0;
            currentStep->outputs[io].received = 0;
            currentStep->outputs[io].optional = 0;
            currentStep->outputs[io].type = 0;
            currentStep->outputs[io].subtype = 0;
        }

        *levelError = TP_OUTPUT;

        for(io=0; io<currentStep->numberOfOutputs; ++io)
        {
            strncpy(fnameError, stepFilename, 130);

            /* get output_level_X handle */
            if((status = GetXMLChildElementByIndex(in_outs, io, &in_out_level)) != 0)
            {
                return status;
            }

            if((status = ParseTestOutput(in_out_level, io, config->levels,
                    &currentStep->outputs[io], levelError, ports,
                    fnameError, &levelErrorRef, attrData,
                    currentStep->outputsCheckmode, protocols)) != 0)
            {
                return status;
            }

            if(*pParsing == CANCELED)
            {
                break;
            }
            if(!currentStep->outputs[io].optional)
                currentStep->numberOfOutputsAtPort[currentStep->outputs[io].ifRef]++;
                /* optional don't count */

#ifndef NO_QT
            SetProgressBarValue(BAR_RX_STEP, io+1);
#endif
        }

        if(*pParsing == CANCELED)
        {
            return 0;
        }	
        *levelError = TP_STEP;
    }

    if(existSpecials)
    {
        if((status = GetXMLChildElementByIndex(element, existSpecials, &in_outs)) != 0)
        {
            return status;
        }
        GetXMLNumChildren(in_outs, &currentStep->numberOfSpecials);
        if(currentStep->numberOfSpecials > (config->numberOfSpecialPackets*2))
        {
            errorNumbers = currentStep->numberOfSpecials;
            currentStep->numberOfSpecials = 0;
            return SPECIAL_ID_BIGGER_THAN_SPECIAL_PACKETS_DEFINED;
        }
        currentStep->specials =
                calloc(currentStep->numberOfSpecials, sizeof(spec_in_step));
        for(io=0; io<currentStep->numberOfSpecials; ++io)
        {
            currentStep->specials[io].id = -1;
        }

        for(io=0; io<currentStep->numberOfSpecials; ++io)
        {
            if((status = GetXMLChildElementByIndex(in_outs, io, &in_out_level)) != 0)
            {
                return status;
            }

            if((status = GetXMLAttributeValueByName(in_out_level, "id",
                    attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }
            currentStep->specials[io].id = atoi(attrData);
            if(currentStep->specials[io].id >= (config->numberOfSpecialPackets*2))
            {
                errorNumbers = currentStep->specials[io].id;
                currentStep->numberOfSpecials = 0;
                return SPECIAL_ID_BIGGER_THAN_SPECIAL_PACKETS_DEFINED;
            }
            /* get enable / disable handle */
            if(strncmp((char*)in_out_level->name, "enable_print", 12) == 0)
            {
                currentStep->specials[io].mode = ENABLE_PRINT;
            }
            else if(strncmp((char*)in_out_level->name, "disable_print", 13) == 0)
            {
                currentStep->specials[io].mode = DISABLE_PRINT;
            }
            else if(strncmp((char*)in_out_level->name, "enable", 6) == 0)
            {
                currentStep->specials[io].mode = ENABLE;
            }
            else //if(strncmp((char*)in_out_level->name, "disable", 7) == 0)
            {
                currentStep->specials[io].mode = DISABLE;
            }
            if(*pParsing == CANCELED)
            {
                break;
            }
        }
    }

    if(existConcurrents)
    {
        if(currentStep->mode == ACTION)
        {
            return WRONG_CONCURRENT_ACTION;
        }
        if((status = GetXMLChildElementByIndex(element, existConcurrents, &in_outs)) != 0)
        {
            return status;
        }
        GetXMLNumChildren(in_outs, &currentStep->concurrent.number);
        currentStep->concurrent.number--; //remove "nextStep"

        currentStep->concurrent.list =
                calloc(currentStep->concurrent.number, sizeof(unsigned int));

        /* get next_step "id" and "isConcurrent" */
        if((status = GetXMLChildElementByIndex(in_outs, 0, &in_out_level)) != 0)
        {
            return status;
        }

        if((status = GetXMLAttributeValueByName(in_out_level, "id", attrData,
                43)) != 0)
        {
            return status;
        }
        currentStep->concurrent.next = atoi(attrData);
        if(currentStep->concurrent.next > numberOfSteps)
        {
            return WRONG_NEXT_CONCURRENT;
        }

        if((status = GetXMLAttributeValueByName(in_out_level, "isConcurrent",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(!strncmp(attrData, "yes", 3))
        {
            currentStep->concurrent.nextIsNotConc = 0;
        }
        else //if(!strncmp(attrData, "no", 2))
        {
            currentStep->concurrent.nextIsNotConc = 1;
        }

        if(elemParent != NULL)
        {
            /* check if nextStep id is concurrent or not */
            if((status = GetXMLChildElementByIndex (elemParent,
                    currentStep->concurrent.next, &in_out_level)) != 0)
            {
                return WRONG_NEXT_CONCURRENT;
            }
            if((status = GetXMLAttributeValueByName (in_out_level, "mode",
                    attrData, ATTR_SIZE)) != 0)
            {
                return status;
            }

            if(((currentStep->concurrent.nextIsNotConc == 1) && (strncmp(attrData, "concurrent", 10) == 0)) ||
                    ((currentStep->concurrent.nextIsNotConc == 0) && (strncmp(attrData, "continuous", 10) == 0)))
            {
                return WRONG_NEXT_CONCURRENT_IS_LAST;
            }
        }

        /* get concurrent_step ids */
        for(io=1; io<=currentStep->concurrent.number; ++io)
        {
            if((status = GetXMLChildElementByIndex(in_outs, io, &in_out_level)) != 0)
            {
                    return status;
            }

            if((status = GetXMLAttributeValueByName(in_out_level, "id", attrData,
                    43)) != 0)
            {
                    return status;
            }
            currentStep->concurrent.list[io-1] = atoi(attrData);

            if(elemParent != NULL)
            {
                /* check if concurrent_step id is concurrent or not */
                if((status = GetXMLChildElementByIndex (elemParent,
                        currentStep->concurrent.list[io-1], &in_out_level)) != 0)
                {
                    return WRONG_NEXT_CONCURRENT;
                }
                if((status = GetXMLAttributeValueByName (in_out_level, "mode",
                        attrData, ATTR_SIZE)) != 0)
                {
                    return status;
                }
                if(strncmp(attrData, "continuous", 10) == 0)
                {
                    errorNumbers = currentStep->concurrent.list[io-1];
                    return WRONG_CONCURRENT_STEP;
                }
            }
            if(*pParsing == CANCELED)
            {
                break;
            }
        }
    }
#ifndef NO_QT
    ShowHideBar(BAR_INPUT, HIDE);
    ShowHideBar(BAR_RX_STEP, HIDE);
#endif
    return 0;
}

void DisplayParseStepError (int status, int step, char fnameError[MAX_MSG_SIZE],
        levelTypeError levelError, char * msg, unsigned int maxMsgSize)
{
    unsigned int len=0;
    switch(levelError)
    {
        case TC_FORMAT: 
            len = snprintf(msg, maxMsgSize, "Error in step %4d, input %4d, "
                    "lvl %d TC format file \"%s\": ", step, io, levelErrorRef,
                    fnameError);
            DisplayParseTestInputError(status, levelError, &msg[len],
                    maxMsgSize-len);
            break;

        case EXPORT:
            len = snprintf(msg, maxMsgSize, "Error in step %4d, input %4d, "
                    "level %d export file \"%s\": ", step, io, levelErrorRef,
                    fnameError);
            DisplayParseTestInputError(status, levelError, &msg[len],
                    maxMsgSize-len);
            break;

        case TP_INPUT:
            len = snprintf(msg, maxMsgSize, "Error in step %4d, input %4d "
                    "in file %s: ", step, io, fnameError);
            DisplayParseTestInputError(status, levelError, &msg[len],
                    maxMsgSize-len);
            break;

        case TM_FORMAT: 
            len = snprintf(msg, maxMsgSize, "Error in step %4d, output %4d, "
                    "lvl %d TM format file \"%s\": ", step, io, levelErrorRef,
                    fnameError);
            DisplayParseTestOutputError(status, levelError, &msg[len],
                    maxMsgSize-len);
            break;

        case FILTER:
            len = snprintf(msg, maxMsgSize, "Error in step %4d, output %4d, "
                    "level %d filter file \"%s\": ", step, io, levelErrorRef,
                    fnameError);
            DisplayParseTestOutputError(status, levelError, &msg[len],
                    maxMsgSize-len);
            break;

        case IMPORT:
            len = snprintf(msg, maxMsgSize, "Error in step %4d, output %4d, "
                    "level %d import file \"%s\": ", step, io, levelErrorRef,
                    fnameError);
            DisplayParseTestOutputError(status, levelError, &msg[len],
                    maxMsgSize-len);
            break;

        case TP_OUTPUT:
            len = snprintf(msg, maxMsgSize, "Error in step %4d, output %4d "
                    "in file %s: ", step, io, fnameError);
            DisplayParseTestOutputError(status, levelError, &msg[len],
                    maxMsgSize-len);
            break;

        case TP_STEP:
            len = snprintf(msg, maxMsgSize, "Error in step %4d in file %s: ",
                    step, fnameError);

            switch (status)
            {
                case WRONG_STEP_ID:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Wrong step id %d. They must be correlative", errorNumbers);
                    break;

                case WRONG_PREV_STEP_ID:
                    snprintf(&msg[len], maxMsgSize-len, "Wrong previous step %d. "
                            "Can't wait for same or future steps", errorNumbers);
                    break;

                case WRONG_PREV_STEP_ID_OUTPUT:
                    snprintf(&msg[len], maxMsgSize-len, "Wrong previous step. "
                            "Waiting step must have outputs");
                    break;

                case WRONG_NULL_PREV_OUTPUT:
                    snprintf(&msg[len], maxMsgSize-len, "Wrong previous"
                            " output. If defined a previous step, previous output"
                            " can't be NULL");
                    break;

                case WRONG_PREV_OUTPUT:
                    snprintf(&msg[len], maxMsgSize-len, "Wrong previous"
                            "output. Output id from waiting step must exist");
                    break;

                case WRONG_MODE:
                    snprintf(&msg[len], maxMsgSize-len, "Error in step %4d: "
                            "Wrong mode: only valid \"continuous\""
                            ", \"manual\" or \"concurrent\"", step);
                    break;

                case TOO_MUCH_INPUTS:
                    snprintf(&msg[len], maxMsgSize-len, "Error in step "
                            "%4d: Too much inputs (%d). Maximum number are %d", 
                            step, errorNumbers, MAX_INPUTS);
                    break;

                case WRONG_CHECKMODE:
                    snprintf(&msg[len], maxMsgSize-len, "Wrong checkmode:"
                            " only valid \"all\", \"allsorted\" or \"any\"");
                    break;   

                case SPECIAL_ID_BIGGER_THAN_SPECIAL_PACKETS_DEFINED:
                    snprintf(&msg[len], maxMsgSize-len, "Error in step %4d: "
                            "Special id (%d) bigger than special packets defined",
                            step, errorNumbers);
                    break;

                case TOO_MUCH_OUTPUTS:
                    snprintf(&msg[len], maxMsgSize-len, "Error in step %4d: "
                            "Too much outputs (%d). Maximum number are %d",
                            step, errorNumbers, MAX_OUTPUTS);
                    break;

                case NEEDED_CONCURRENT:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Needed \"concurrent_steps\" tag when step mode "
                            "is \"concurrent\"");
                    break;

                case WRONG_NEXT_CONCURRENT:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Needed \"nextStep id\" step does not exist");
                    break;

                case WRONG_NEXT_CONCURRENT_IS_LAST:
                    snprintf(&msg[len], maxMsgSize-len, "Given "
                            "\"nextStep isConcurrent\" is not well defined");
                    break;

                case WRONG_CONCURRENT_STEP:
                    snprintf(&msg[len], maxMsgSize-len, "Given concurrent_step "
                            "id=\"%d\" is not well defined", errorNumbers);
                    break;
                    
                case WRONG_STEP_MODE:
                    snprintf(msg, maxMsgSize,
                            "Wrong step mode: \"action\" requires \"action\" "
                            "tag, other modes require \"inputs\"");
                    break;
                    
                case WRONG_ACTION_TYPE:
                    snprintf(msg, maxMsgSize,
                            "Wrong action type mode: only \"instruction\" "
                            "or \"checking\" are valid");
                    break;
                    
                case WRONG_CONCURRENT_ACTION:
                    snprintf(msg, maxMsgSize,
                            "Wrong concurrent tag inside action mode");
                    break;

                default:
                    XMLerrorInfo(status, &msg[len], maxMsgSize-len);
                    break;
            }
            break;

        default:
            break;
    }
    return;
}
