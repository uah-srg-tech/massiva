/**
 * \file	ProcessPacket.c
 * \brief	functions for process (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		18/09/2013 at 11:23:12
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <cstdio>			/* snprintf */
#include <cstdlib>			/* calloc */
#include <cstring>			/* strlen */
#include "ProcessPacket.h"
#include "CheckFormatTools.h"           /* CheckFormat */
#include "CheckFilterTools.h"           /* CheckBoolVars, CheckFilters */
#include "ImportFields.h"		/* importVirtualFields*/

ProcessPacket::ProcessPacket()
{
}

int ProcessPacket::Expected(const uint8_t * dataPacket,
        unsigned int &level0Length, const level * defaultLevels,
        output * expectedPacket, unsigned int &correctLevels,
        unsigned int rxPort, filterResults * pResults)
{
    int status = 0, filterErrorAt = -1;
    unsigned int lvl = 0, bv = 0;
    unsigned int length = 0, importValue = 0;
    formatField * currentTMFields, * previousTMFields;
    unsigned int currentResult = 1;
    processErrorType = NO_PROCESS_ERROR;
	
    for(lvl=0; lvl<expectedPacket->numberOfLevels; ++lvl)
    {
        /* import */   
        if(expectedPacket->level[lvl].importInBytes == 0)
        {
            /* defaultImport */
            if(lvl != 0)
            {
                if(expectedPacket->level[lvl].numberOfTMFields == 0)
                {
                    /* defaultFormat of current level */
                    currentTMFields = defaultLevels[lvl].out.TMFields;
                }
                else
                {
                    /* not defaultFormat of previous level */
                    currentTMFields = expectedPacket->level[lvl].TMFields;
                }

                if(expectedPacket->level[lvl-1].numberOfTMFields == 0)
                {
                    /* defaultFormat of previous level */
                    previousTMFields = defaultLevels[lvl-1].out.TMFields;
                }
                else
                {
                    /* not defaultFormat of previous level */
                    previousTMFields = expectedPacket->level[lvl-1].TMFields;
                }
                if((status = importVirtualFields(&dataPacket[importValue],
                        defaultLevels[lvl].out.virtualFields,
                        defaultLevels[lvl].out.numberOfVirtualFields,
                        previousTMFields, currentTMFields)) < 0)
                {
                    processErrorType = IMPORT_ERROR;
                    errorLevel = lvl;
                    break;
                }
            }
            importValue += defaultLevels[lvl].out.importInBytes;
        }
        else
        {
            /* not defaultImport */
            if(lvl != 0)
            {
                if(expectedPacket->level[lvl].numberOfTMFields == 0)
                {
                    /* defaultFormat of current level */
                    currentTMFields = defaultLevels[lvl].out.TMFields;
                }
                else
                {
                    /* not defaultFormat of previous level */
                    currentTMFields = expectedPacket->level[lvl].TMFields;
                }
                if(expectedPacket->level[lvl-1].numberOfTMFields == 0)
                {
                    /* defaultFormat of previous level */
                    previousTMFields = defaultLevels[lvl-1].out.TMFields;
                }
                else
                {
                    /* not defaultFormat of previous level */
                    previousTMFields = expectedPacket->level[lvl-1].TMFields;
                }
                if((status = importVirtualFields(&dataPacket[importValue],
                        expectedPacket->level[lvl].virtualFields,
                        expectedPacket->level[lvl].numberOfVirtualFields,
                        previousTMFields, currentTMFields)) < 0)
                {
                    processErrorType = IMPORT_ERROR;
                    errorLevel = lvl;
                    break;
                }
            }
            importValue += expectedPacket->level[lvl].importInBytes;
        }

        /* check format */
        if(expectedPacket->level[lvl].numberOfTMFields == 0)
        {
            /* defaultFormat */
            status = CheckFormat(&dataPacket[importValue],
                    defaultLevels[lvl].out.TMFields,
                    defaultLevels[lvl].out.numberOfTMFields, &length, rxPort);
        }
        else
        {
            /* not defaultFormat */
            status = CheckFormat(&dataPacket[importValue],
                    expectedPacket->level[lvl].TMFields,
                    expectedPacket->level[lvl].numberOfTMFields, &length, rxPort);
        }
        if(status < 0)
        {
            /*
             *	if it is level or previous level correct 0 mark as a format error
             *	if it not mark as a wrong filter
             */
            if((lvl == 0) || (level0Length == (lvl-1)))
            {
                processErrorType = FORMAT_ERROR;
                errorLevel = lvl;
            }
            else
            {
                status = 0;
            }
            break;
        }
        if(lvl == 0)
            level0Length = length;

        if(expectedPacket->level[lvl].defaultFilter)
        {
            /* check default boolVars */
            pResults->numbersDef[lvl] =
                    new uint64_t [defaultLevels[lvl].out.numberOfBoolVars]();
            pResults->stringsDef[lvl] =
                    new uint8_t * [defaultLevels[lvl].out.numberOfBoolVars];
            for(bv=0; bv<defaultLevels[lvl].out.numberOfBoolVars; ++bv)
            {
                pResults->stringsDef[lvl][bv] = new uint8_t [MAX_STR_OPERAND_LEN]();
            }

            if(expectedPacket->level[lvl].numberOfTMFields == 0)
            {
                /* defaultFormat */
                status = CheckBoolVars(&dataPacket[importValue],
                        pResults->numbersDef[lvl], pResults->stringsDef[lvl],
                        defaultLevels[lvl].out.boolVars,
                        defaultLevels[lvl].out.numberOfBoolVars,
                        defaultLevels[lvl].out.TMFields,
                        defaultLevels[lvl].out.numberOfTMFields,
                        defaultLevels[lvl].out.crcTMFieldRefs,
                        defaultLevels[lvl].out.numberOfcrcTMFields);
            }
            else
            {
                /* not defaultFormat */
                status = CheckBoolVars(&dataPacket[importValue], 
                        pResults->numbersDef[lvl], pResults->stringsDef[lvl],
                        defaultLevels[lvl].out.boolVars,
                        defaultLevels[lvl].out.numberOfBoolVars,
                        expectedPacket->level[lvl].TMFields,
                        expectedPacket->level[lvl].numberOfTMFields,
                        expectedPacket->level[lvl].crcTMFieldRefs,
                        expectedPacket->level[lvl].numberOfcrcTMFields);
            }

            if(status < 0)
            {
                /*
                 *  if level 0 or previous level correct mark as a default filter error
                 *  if it not mark as a wrong filter
                 */
                if((lvl == 0) || (correctLevels == (lvl-1)))
                {
                    processErrorType = DEFAULT_BV_ERROR;
                    errorLevel = lvl;
                }
                else
                {
                    status = 0;
                }
                break;
            }
            /* check default filters */
            pResults->defaultResult[lvl] = CheckFilters(defaultLevels[lvl].out.filters,
                    defaultLevels[lvl].out.typeOfFilters,
                    defaultLevels[lvl].out.numberOfFilters,
                    defaultLevels[lvl].out.boolVars, &filterErrorAt);
            if(pResults->defaultResult[lvl] < 0)
            {
                status = pResults->defaultResult[lvl];
                processErrorType = DEFAULT_FILTER_ERROR;
                errorLevel = lvl;
                break;
            }
            currentResult &= pResults->defaultResult[lvl];
        }

        if(expectedPacket->level[lvl].numberOfFilters != 0)
        {
            pResults->numbersExt[lvl] =
                    new uint64_t [expectedPacket->level[lvl].numberOfBoolVars]();
            pResults->stringsExt[lvl] =
                    new uint8_t * [expectedPacket->level[lvl].numberOfBoolVars];
            for(bv=0; bv<expectedPacket->level[lvl].numberOfBoolVars; ++bv)
            {
                pResults->stringsExt[lvl][bv] = new uint8_t [MAX_STR_OPERAND_LEN]();
            }

            /* check extra boolVars */
            if(expectedPacket->level[lvl].numberOfTMFields == 0)
            {
                /* defaultFormat */
                status = CheckBoolVars(&dataPacket[importValue],
                        pResults->numbersExt[lvl], pResults->stringsExt[lvl],
                        expectedPacket->level[lvl].boolVars,
                        expectedPacket->level[lvl].numberOfBoolVars,
                        defaultLevels[lvl].out.TMFields,
                        defaultLevels[lvl].out.numberOfTMFields,
                        defaultLevels[lvl].out.crcTMFieldRefs,
                        defaultLevels[lvl].out.numberOfcrcTMFields);
            }
            else
            {
                /* not defaultFormat */
                status = CheckBoolVars(&dataPacket[importValue], 
                        pResults->numbersExt[lvl], pResults->stringsExt[lvl],
                        expectedPacket->level[lvl].boolVars,
                        expectedPacket->level[lvl].numberOfBoolVars,
                        expectedPacket->level[lvl].TMFields,
                        expectedPacket->level[lvl].numberOfTMFields,
                        expectedPacket->level[lvl].crcTMFieldRefs,
                        expectedPacket->level[lvl].numberOfcrcTMFields);
            }
            if(status < 0)
            {
                /*
                 *  if level 0 or previous level correct mark as a extra filter error
                 *  if it not mark as a wrong filter
                 */
                if((lvl == 0) || (correctLevels == (lvl-1)))
                {
                    processErrorType = EXTRA_BV_ERROR;
                    errorLevel = lvl;
                }
                else
                {
                    status = 0;
                }
                break;
            }
            /* check extra filters */
            pResults->extraResult[lvl] = CheckFilters(expectedPacket->level[lvl].filters,
                    expectedPacket->level[lvl].typeOfFilters,
                    expectedPacket->level[lvl].numberOfFilters,
                    expectedPacket->level[lvl].boolVars, &filterErrorAt);
            if(pResults->extraResult[lvl] < 0)
            {
                status = pResults->extraResult[lvl];
                processErrorType = EXTRA_FILTER_ERROR;
                errorLevel = lvl;
                break;
            }
            currentResult &= pResults->extraResult[lvl];
        }
        /* mark level as correct is current result == 1 */
        if(currentResult)
            correctLevels = lvl;
    }
    if((status == 0) && (currentResult))
        status = 1;
    return status;
}

int ProcessPacket::NotExpected(const uint8_t * dataPacket,
        unsigned int &level0Length, const level * defaultLevels,
        unsigned int numberOfdefaultLevels, unsigned int rxPort)
{
    int status = 0;
    unsigned int length = 0, importValue = 0, lvl = 0;
    processErrorType = NO_PROCESS_ERROR;

    for(lvl=0; lvl<numberOfdefaultLevels; ++lvl)
    {
        /* import */
        if(lvl != 0)
        {
            if((status = importVirtualFields(&dataPacket[importValue],
                    defaultLevels[lvl].out.virtualFields,
                    defaultLevels[lvl].out.numberOfVirtualFields,
                    defaultLevels[lvl-1].out.TMFields,
                    defaultLevels[lvl].out.TMFields)) < 0)
            {
                processErrorType = IMPORT_ERROR;
                errorLevel = lvl;
                break;
            }
        }
        importValue += defaultLevels[lvl].out.importInBytes;

        /* check format */
        status = CheckFormat(&dataPacket[importValue],
                defaultLevels[lvl].out.TMFields,
                defaultLevels[lvl].out.numberOfTMFields, &length, rxPort);
        if(status < 0)
        {
            processErrorType = FORMAT_ERROR;
            errorLevel = lvl;
            break;
        }

        if(lvl == 0)
            level0Length = length;
        /* check default boolVars */	
        status = CheckBoolVars(&dataPacket[importValue], NULL, NULL,
                defaultLevels[lvl].out.boolVars,
                defaultLevels[lvl].out.numberOfBoolVars,
                defaultLevels[lvl].out.TMFields,
                defaultLevels[lvl].out.numberOfTMFields,
                defaultLevels[lvl].out.crcTMFieldRefs,
                defaultLevels[lvl].out.numberOfcrcTMFields);
        if(status < 0)
        {
            processErrorType = DEFAULT_BV_ERROR;
            errorLevel = lvl;
            break;
        }
    }
    return status;
}

int ProcessPacket::checkIfSpecial(const uint8_t * dataPacket,
        unsigned int &level0Length, unsigned int numberOfSpecialPackets,
        const level * defaultLevels, const output * specialPackets,
        special_packet_info * specialInfo, int &specialIndex,
        unsigned int rxPort)
{
    int status = 0, filterErrorAt = -1;
    unsigned int sp = 0, lvl = 0;
    unsigned int length = 0;
    unsigned int * importPointer, importValue = 0;
    uint8_t ** results = new uint8_t* [numberOfSpecialPackets];
    processErrorType = NO_PROCESS_ERROR;
    for(sp=0; sp<numberOfSpecialPackets; ++sp)
    {
        results[sp] = new uint8_t[3]();
    }

    for(sp=0; sp<numberOfSpecialPackets; ++sp)
    {
        if((specialPackets[sp].ifRef != rxPort) || (specialInfo[sp].enabled == 0))
        {
            //this special packet isn't configured for this port
            //or this special packet mustn't be processed
            continue;
        }

        for(lvl=0; lvl<specialPackets[sp].numberOfLevels; ++lvl)//levels
        {
            /* import */
            if(lvl == 0)
                importValue = 0;
            else
            {
                if(specialPackets[sp].level[lvl].TMFields == NULL)
                {
                    status = importVirtualFields(&dataPacket[importValue],
                            defaultLevels[lvl].out.virtualFields,
                            defaultLevels[lvl].out.numberOfVirtualFields,
                            defaultLevels[lvl-1].out.TMFields,
                            defaultLevels[lvl].out.TMFields);
                    importPointer = (unsigned int *)&defaultLevels[lvl].out.importInBytes;
                }
                else
                {
                    if(specialPackets[sp].level[lvl-1].TMFields == NULL)
                    {
                        status = importVirtualFields(&dataPacket[importValue],
                                specialPackets[sp].level[lvl].virtualFields,
                                specialPackets[sp].level[lvl].numberOfVirtualFields,
                                defaultLevels[lvl-1].out.TMFields,
                                specialPackets[sp].level[lvl].TMFields);
                    }
                    else
                    {
                        status = importVirtualFields(&dataPacket[importValue],
                                specialPackets[sp].level[lvl].virtualFields,
                                specialPackets[sp].level[lvl].numberOfVirtualFields,
                                specialPackets[sp].level[lvl-1].TMFields,
                                specialPackets[sp].level[lvl].TMFields);
                    }
                    importPointer = (unsigned int *)&specialPackets[sp].level[lvl].importInBytes;
                }
                if(status < 0)
                {
                    processErrorType = IMPORT_ERROR;
                    errorLevel = lvl;
                    break;
                }
                importValue += *importPointer;
            }

            /* check format */
            if(specialPackets[sp].level[lvl].TMFields == NULL)
            {
                status = CheckFormat(&dataPacket[importValue],
                        defaultLevels[lvl].out.TMFields,
                        defaultLevels[lvl].out.numberOfTMFields, &length, rxPort);
            }
            else
            {
                status = CheckFormat(&dataPacket[importValue],
                        specialPackets[sp].level[lvl].TMFields,
                        specialPackets[sp].level[lvl].numberOfTMFields, &length, rxPort);
            }
            if(status < 0)
            {
                processErrorType = FORMAT_ERROR;
                errorLevel = lvl;
                break;
            }

            if(lvl == 0)
                level0Length = length;

            if(defaultLevels[lvl].out.boolVars == NULL)
            {
                /* check default boolVars */
                status = CheckBoolVars(&dataPacket[importValue], NULL, NULL,
                        defaultLevels[lvl].out.boolVars,
                        defaultLevels[lvl].out.numberOfBoolVars,
                        defaultLevels[lvl].out.TMFields,
                        defaultLevels[lvl].out.numberOfTMFields,
                        defaultLevels[lvl].out.crcTMFieldRefs,
                        defaultLevels[lvl].out.numberOfcrcTMFields);
                if(status < 0)
                {
                    processErrorType = DEFAULT_BV_ERROR;
                    errorLevel = lvl;
                    break;
                }
            }
            if(specialPackets[sp].level[lvl].boolVars == NULL)
            {
                //this level is not filter-configured, mark as OK and continue
                results[sp][lvl] = 1;
                continue;
            }

            /* check special packet j boolVars at level i */
            if(specialPackets[sp].level[lvl].TMFields == NULL)
            {
                status = CheckBoolVars(&dataPacket[importValue], NULL, NULL, 
                        specialPackets[sp].level[lvl].boolVars,
                        specialPackets[sp].level[lvl].numberOfBoolVars,
                        defaultLevels[lvl].out.TMFields, 
                        defaultLevels[lvl].out.numberOfTMFields,
                        defaultLevels[lvl].out.crcTMFieldRefs,
                        defaultLevels[lvl].out.numberOfcrcTMFields);
            }
            else
            {
                status = CheckBoolVars(&dataPacket[importValue], NULL, NULL, 
                        specialPackets[sp].level[lvl].boolVars,
                        specialPackets[sp].level[lvl].numberOfBoolVars,
                        specialPackets[sp].level[lvl].TMFields, 
                        specialPackets[sp].level[lvl].numberOfTMFields,
                        specialPackets[sp].level[lvl].crcTMFieldRefs,
                        specialPackets[sp].level[lvl].numberOfcrcTMFields);
            }
            if(status < 0)
            {
                processErrorType = EXTRA_BV_ERROR;
                errorLevel = lvl;
                break;
            }

            /* check special packet j filters at level i */
            status = CheckFilters(specialPackets[sp].level[lvl].filters,
                    specialPackets[sp].level[lvl].typeOfFilters, 
                    specialPackets[sp].level[lvl].numberOfFilters,
                    specialPackets[sp].level[lvl].boolVars, &filterErrorAt);
            if(status < 0)
            {
                processErrorType = EXTRA_FILTER_ERROR;
                errorLevel = lvl;
                break;
            }
            else if(status > 0)
            {
                results[sp][lvl] = 1;
            }
            else /* if(status == 0) */
            /* if level filter wasn't ok, don't continue with this dataPacket */
            {
                break;
            }
        }
        if(status < 0)
        {
            break;
        }
    }
    if(status < 0)
    {
        specialIndex = sp;
    }
    else
    {
        /* check if any dataPacket is special */
        for(sp=0; sp<numberOfSpecialPackets; ++sp)
        {
            for(lvl=0; lvl<specialPackets[sp].numberOfLevels; ++lvl)
            {
                if(!results[sp][lvl])
                    break;
            }
            if(lvl == specialPackets[sp].numberOfLevels)
            {
                specialIndex = sp;
                break;
            }
        }
    }
    for(sp=0; sp<numberOfSpecialPackets; ++sp)
    {
        delete[] results[sp];
    }
    delete[] results;
    return status;
}

void ProcessPacket::printError(int status, char title[60], char * msg,
        unsigned int maxMsgSize, int specialPacket, int step, int output,
        const char * portName, unsigned int rxPort)
{
    unsigned int messageLen = 60;
    switch(processErrorType)
    {
        case IMPORT_ERROR:
            snprintf(title, 60, "%s port Import error", portName);
            DisplayImportFieldsError (status, msg, maxMsgSize);
            break;

        case FORMAT_ERROR:
            snprintf(title, 60, "%s port TM Format error", portName);
            ParseCheckFormatError(status, msg, maxMsgSize, rxPort);
            break;

        case DEFAULT_BV_ERROR:
            snprintf(title, 60, "%s port Default BoolVar error", portName);
            ParseCheckBoolVarError(status, msg, maxMsgSize);
            break;

        case DEFAULT_FILTER_ERROR:
            snprintf(title, 60, "%s port Default Filter error", portName);
            ParseCheckFilterError(status, msg, maxMsgSize);
            break;

        case EXTRA_BV_ERROR:
            snprintf(title, 60, "%s port Extra BoolVar error", portName);
            ParseCheckBoolVarError(status, msg, maxMsgSize);
            break;

        case EXTRA_FILTER_ERROR:
            snprintf(title, 60, "%s port Extra Filter error", portName);
            ParseCheckFilterError(status, msg, maxMsgSize);
            break;

        case NO_PROCESS_ERROR:
            break;

        default:
            snprintf(title, 60, "%s port unknown error %d", portName, status);
            break;
    }
    messageLen = strlen(title);
    if(processErrorType != NO_PROCESS_ERROR)
    {
        if(step == -1) //unexpected packet
        {
            if(specialPacket == -1)
            {
                snprintf(&title[messageLen], 60-messageLen,
                        " at level %d:", errorLevel);
            }
            else
            {
                snprintf(&title[messageLen], 60-messageLen,
                        " at special packet %d, level %d:", specialPacket, errorLevel);
            }
        }
        else if(output == -1) //unexpected packet
        {
            snprintf(&title[messageLen], 60-messageLen,
                    " at step %d, level %d:", step, errorLevel);
        }
        else
        {
            snprintf(&title[messageLen], 60-messageLen,
                    " at step %d, output %d, level %d:", step, output, errorLevel);
        }
    }
    processErrorType = NO_PROCESS_ERROR;
    errorLevel = 0;
    return;
}
