/**
 * \file	XMLProcedureListTools.c
 * \brief	functions for parsing egse_config.xml file (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/01/2013 at 12:52:35
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdlib.h>
#include <string.h>
#include "XMLTools.h"			/* GetXMLValueByTag, ... */
#include "../definitions.h"		/* config */

enum {
    TOO_MUCH_PROCEDURES = -201,
    PROC_NAME_TOO_LONG = -202,
    PROC_FILE_TOO_LONG = -203,
    PROC_REQ_TOO_LONG = -204,
    WRONG_REQ_ACTION = -205,
    ONLY_ONE_REQ_ATTRIB = -206,
    PREV_ACTION_PARAM_TOO_LONG = -207
};

static unsigned int tp = 0;
static int errorNumbers = 0;

int ParseProcedureList(xmlNodePtr root, gss_config * pConfig)
{
    int status = 0;
    unsigned int length = 0, tp_children = 0, tp_attrib = 0;
    xmlNodePtr tests = NULL, test_proc = NULL;
    xmlAttrPtr attribute = NULL;

    if((status = GetXMLChildElementByTag(root, "Tests", &tests)) != 0)
    {
        return status;
    }
    /* get number of test procedures */
    GetXMLNumChildren(tests, &pConfig->numberOfProcedures);
    
    if(pConfig->numberOfProcedures > MAX_PROCEDURES)
    {
        errorNumbers = pConfig->numberOfProcedures;
        pConfig->numberOfProcedures = 0;
        return TOO_MUCH_PROCEDURES;
    }

    /* parse test procedures filenames */
    for(tp=0; tp<pConfig->numberOfProcedures; ++tp)
    {
        /* get TestCase handle */
        if((status = GetXMLChildElementByIndex(tests, tp, &test_proc)) != 0)
        {
            return status;
        }
        /* get TestCase name length */
        if((status = GetXMLAttributeLengthByName (test_proc, "name", &length)) != 0)
        {
            return status;
        }
        if((length+1) > MAX_PROC_NAME_LEN)
        {
            errorNumbers = length+1;
            return PROC_NAME_TOO_LONG;
        }
        /* get TestCase name */
        if((status = GetXMLAttributeValueByName (test_proc, "name",
                pConfig->procedures[tp].name, MAX_PROC_NAME_LEN)) != 0)
        {
            return status;
        }

        pConfig->procedures[tp].prevMsg[0] = '\0';
        pConfig->procedures[tp].prevAction = 0;
        pConfig->procedures[tp].prevActionParam[0] = '\0';
        pConfig->procedures[tp].enabled = 1;

        /* get TestCase procedure file length */
        if((status = GetXMLLengthFromAttributeOrHRefTag (test_proc, "procedure",
                &length)) != 0)
        {
            memset(pConfig->procedures[tp].name, 0, MAX_PROC_NAME_LEN);
            return status;
        }
        if((length+1) > MAX_PROC_FILE_LEN)
        {
            memset(pConfig->procedures[tp].name, 0, MAX_PROC_NAME_LEN);
            errorNumbers = length+1;
            return PROC_FILE_TOO_LONG;
        }

        /* get TestCase procedure file */
        if((status = GetXMLValueFromAttributeOrHRefTag (test_proc, "procedure",
                pConfig->procedures[tp].filename, MAX_PROC_FILE_LEN)) != 0)
        {
            memset(pConfig->procedures[tp].name, 0, MAX_PROC_NAME_LEN);
            return status;
        }
        //check if procedure is child or not
        GetXMLNumChildren(test_proc, &tp_children);

        GetXMLNumAttributes(test_proc, &tp_attrib);
        if((tp_children + tp_attrib) > 2)
        {
            if((tp_children + tp_attrib) > 4)
            {
                return ONLY_ONE_REQ_ATTRIB;
            }
            
            char attrName[11];
            /* get TestCase req/reqAction/prev/prevAction handle */
            if((status = GetXMLAttributeByIndex(test_proc, 2-tp_children,
                    &attribute)) != 0)
            {
                return status;
            }
            /* get TestCase req/reqAction/prev/prevAction length */
            if((status = GetXMLAttributeLength(attribute, &length)) != 0)
            {
                return status;
            }
            if((length+1) > MAX_PROC_NAME_LEN)
            {
                errorNumbers = length+1;
                return PROC_REQ_TOO_LONG;
            }

            /* get TestCase req/reqAction/prev/prevAction value */
            if((status = GetXMLAttributeValue(attribute,
                    pConfig->procedures[tp].prevMsg, MAX_PROC_NAME_LEN)) != 0)
            {
                return status;
            }
            /* get TestCase req/reqAction/prev/prevAction attr name */
            if((status = GetXMLAttributeName(attribute, attrName, 11)) != 0)
            {
                return status;
            }

            if((strncmp(attrName, "reqAction", 9) == 0) ||
                    (strncmp(attrName, "prevAction", 10) == 0))
            {
                if(strncmp(pConfig->procedures[tp].prevMsg, "reset", 5) == 0)
                {
                    pConfig->procedures[tp].prevAction = 1;
                }
                else
                {
                    pConfig->procedures[tp].prevAction =
                        atoi(pConfig->procedures[tp].prevMsg);
                }
                pConfig->procedures[tp].prevMsg[0] = '\0';
                //'0' is not a valid value
                if((pConfig->procedures[tp].prevAction <= 0) ||
                        (pConfig->procedures[tp].prevAction > MAX_NUMBER_ACTIONS))
                {
                    errorNumbers = pConfig->procedures[tp].prevAction;
                    return WRONG_REQ_ACTION;
                }
                if((tp_children + tp_attrib) == 4)
                {
                    /* get TestCase req/reqAction/prev/prevAction handle */
                    if((status = GetXMLAttributeByIndex(test_proc,
                            3-tp_children, &attribute)) != 0)
                    {
                        return status;
                    }
                    /* get TestCase req/reqAction/prev/prevAction length */
                    if((status = GetXMLAttributeLength(attribute, &length)) != 0)
                    {
                        return status;
                    }
                    if((length+1) > MAX_PROC_NAME_LEN)
                    {
                        errorNumbers = length+1;
                        return PREV_ACTION_PARAM_TOO_LONG;
                    }
                    /* get reqAction/prevAction value */
                    if((status = GetXMLAttributeValue(attribute,
                            pConfig->procedures[tp].prevActionParam, MAX_PROC_NAME_LEN)) != 0)
                    {
                        return status;
                    }
                }
            }
            else if(((strncmp(attrName, "req", 3) != 0) && (strncmp(attrName, "prev", 4) != 0)) ||
                    ((tp_children + tp_attrib) == 4))
            {
                return ONLY_ONE_REQ_ATTRIB;
            }
        }
        test_proc = NULL;
    }
    return 0;
}

void DisplayParseProcedureListError(int status, char * msg, int maxMsgSize)
{
    unsigned int len = snprintf(msg, maxMsgSize, "Error in TestCase %d: ", tp);
    switch (status)
    {
        case TOO_MUCH_PROCEDURES:
            snprintf(msg, maxMsgSize, "Too much Procedures (%d). "
                    "Maximum are %d", errorNumbers, MAX_PROCEDURES);
            break;

        case PROC_NAME_TOO_LONG:
            snprintf(&msg[len], maxMsgSize-len, "Name too long (%d). "
                    "Max lenght are %d", errorNumbers, MAX_PROC_NAME_LEN-1);
            break;

        case PROC_FILE_TOO_LONG:
            snprintf(&msg[len], maxMsgSize-len, "Filename too long (%d). "
                    "Max lenght are %d", errorNumbers, MAX_PROC_FILE_LEN-1);
            break;

        case PROC_REQ_TOO_LONG:
            snprintf(&msg[len], maxMsgSize-len, "Message too long (%d). "
                    "Max lenght are %d", errorNumbers, MAX_PROC_NAME_LEN-1);
            break;

        case ONLY_ONE_REQ_ATTRIB:
            snprintf(&msg[len], maxMsgSize-len, "Wrong number of attributes. "
                    "Only valid one of this: \"req\", \"reqAction\", "
                    "\"prev\", OR \"prevAction\" with an optional \"prevActionParam\"");
            break;

        case WRONG_REQ_ACTION:
            snprintf(&msg[len], maxMsgSize-len, "Action %d not found. "
                    "Current valid value is 1", errorNumbers);
            break;

        case PREV_ACTION_PARAM_TOO_LONG:
            snprintf(&msg[len], maxMsgSize-len, "Action parameter too long (%d). "
                    "Max lenght are %d", errorNumbers, MAX_PROC_NAME_LEN-1);
            break;

        default:
            XMLerrorInfo(status, &msg[len], maxMsgSize-len);
            break;
    }
    return;
}
