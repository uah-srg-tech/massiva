/**
 * \file	PrepareInput.cpp
 * \brief	functions for prepare an input to be launched (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		20/06/2012 at 17:03:38
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <cstdio>                  	/* snprintf */
#include <cstdlib>           		/* calloc, free, NULL */
#include <cstring>                  /* memset */
#include "PrepareInput.h"
#include "../CommonTools/GetSetFieldTools.h"	/* SettingNumber */
#include "../CheckTools/ExportSettingTools.h"
#include "../CheckTools/CheckFormatTools.h"

PrepareInput::PrepareInput(gssStructs * origGssStruct)
{
    pGssStruct = origGssStruct;
    prepareErrorType = PREPARE_NO_ERROR;
}

int PrepareInput::prepare(unsigned char tcBuffer[MAX_LEVELS][MAX_PACKET_SIZE+MAX_PHY_HDR],
        unsigned int portPhyHeaderOffsetTC, input * currentInput)
{
    int status = 0;
    unsigned int length = 0;
    formatField * targetFields = NULL, * sourceFields = NULL;
    unsigned char * targetBuffer = NULL, * sourceBuffer = NULL;
    unsigned int numberOfTargetFields = 0;
    unsigned int * numberOfcrcFields = NULL, ** crcFieldRefs = NULL;
    unsigned int numberOfActiveDICs, * ActiveDICs = NULL;
    levelExport * exportFields = NULL;
    unsigned int numberOfExportFields = 0;
    unsigned int txPort = currentInput->ifRef;
    level * defaultLevels = pGssStruct->getPointerToLevels(txPort);
    unsigned int upperLevel = currentInput->numberOfLevels;

    /* export up to level 0 (if necessary) */
    for(curLevel=upperLevel; curLevel>0; --curLevel)
    {
        memset(tcBuffer[curLevel-1], 0, MAX_PACKET_SIZE+MAX_PHY_HDR);
        if(currentInput->level[curLevel-1].numberOfTCFields == 0)
        {
            /* defaultFormat for targetFields  */
            targetFields = defaultLevels[curLevel-1].in.TCFields;
            numberOfTargetFields = defaultLevels[curLevel-1].in.numberOfTCFields;
            numberOfcrcFields = defaultLevels[curLevel-1].in.numberOfcrcTCFields;
            crcFieldRefs = defaultLevels[curLevel-1].in.crcTCFieldRefs;
        }
        else
        {
            /* not defaultFormat for targetFields */
            targetFields = currentInput->level[curLevel-1].TCFields;
            numberOfTargetFields = currentInput->level[curLevel-1].numberOfTCFields;
            numberOfcrcFields = currentInput->level[curLevel-1].numberOfcrcTCFields;
            crcFieldRefs = currentInput->level[curLevel-1].crcTCFieldRefs;
        }
        if(curLevel == 1)
        {
            targetBuffer = &tcBuffer[curLevel-1][portPhyHeaderOffsetTC];
        }
        else
        {
            targetBuffer = &tcBuffer[curLevel-1][0];
        }
        if(curLevel == upperLevel)
        {
            sourceBuffer = NULL;
            sourceFields = NULL;
        }
        else
        {
            sourceBuffer = tcBuffer[curLevel];
            if(currentInput->level[curLevel].numberOfTCFields == 0)
            {
                /* defaultFormat for sourceFields */
                sourceFields = defaultLevels[curLevel].in.TCFields;
            }
            else
            {
                /* not defaultFormat for sourceFields (level i) */
                sourceFields = currentInput->level[curLevel].TCFields;
            }
        }
        
        if(currentInput->level[curLevel-1].numberOfExportFields == 0)
        {
            /* defaultExport */
            numberOfExportFields = defaultLevels[curLevel-1].in.numberOfExportFields;
            exportFields = defaultLevels[curLevel-1].in.exportFields;
            numberOfActiveDICs = defaultLevels[curLevel-1].in.numberOfActiveDICs;
            ActiveDICs = defaultLevels[curLevel-1].in.ActiveDICs;
        }
        else
        {
            /* not defaultExport */
            numberOfExportFields = currentInput->level[curLevel-1].numberOfExportFields;
            exportFields = currentInput->level[curLevel-1].exportFields;
            numberOfActiveDICs = currentInput->level[curLevel-1].numberOfActiveDICs;
            ActiveDICs = currentInput->level[curLevel-1].ActiveDICs;
        }

        /* 1) export CSFIELDS info from level i to level lvl-1 */
        if((status = exportInformation(EXPORT_CONST, exportFields,
                numberOfExportFields, targetFields, numberOfTargetFields,
                targetBuffer, sourceFields, sourceBuffer)) < 0)
        {
            prepareErrorType = PREPARE_EXPORT_ERROR;
            return status;
        }
        /* 2) check format to fill totalSizeInBits (VBLE) or offsetInBits (CRC) */
        if((status = CheckFormat(targetBuffer, targetFields,
                numberOfTargetFields, &length, txPort)) < 0)
        {
            prepareErrorType = PREPARE_FORMAT_ERROR;
            return status;
        }
        /* 3) export ALL info from level i to level lvl-1 */
        if((status = exportInformation(EXPORT_ALL, exportFields,
                numberOfExportFields, targetFields, numberOfTargetFields,
                targetBuffer, sourceFields, sourceBuffer)) < 0)
        {
            prepareErrorType = PREPARE_EXPORT_ERROR;
            return status;
        }

        /* if not all fields have been exported, */
        /* check if any globalVar TC has to be applied */
        pGssStruct->UpdateGlobalVarTC(targetBuffer, targetFields,
                numberOfTargetFields, txPort, curLevel-1);

        /* perform DICs (if exists) */
        if((status = performDICs(numberOfActiveDICs, ActiveDICs, crcFieldRefs,
                numberOfcrcFields, targetBuffer, targetFields)) < 0)
        {
            prepareErrorType = PREPARE_DICS_ERROR;
            return status;
        }
	if(upperLevel == 0)
	{
            /* if upper level exported is level 0, check format again to get real length */
            status = CheckFormat(targetBuffer, targetFields,
                    numberOfTargetFields, &length, txPort);
            if(status < 0)
            {
                prepareErrorType = PREPARE_FORMAT_ERROR;
                return status;
            }
	}
    }
    if(portPhyHeaderOffsetTC)
    {
        CreatePortPhyHeaderOffset(tcBuffer[0], length,
                pGssStruct->getPointerToPort(txPort));
        length += portPhyHeaderOffsetTC;
    }
    return length;
}

void PrepareInput::printError(int status, char title[60], char * msg,
        unsigned int maxMsgSize, const char * portName, unsigned int port,
        int txStep, unsigned int input)
{
    unsigned int length = 0;
    switch(prepareErrorType)
    {
        case PREPARE_EXPORT_ERROR:
            length = snprintf(title, 60, "Export error at port %s ", portName);
            break;

        case PREPARE_FORMAT_ERROR:
            length = snprintf(title, 60, "TC Format error at port %s ", portName);
            break;

        case PREPARE_DICS_ERROR:
            length = snprintf(title, 60, "DICs error at port %s ", portName);
            break;

        case PREPARE_NO_ERROR:
            return;

        default:
            length = snprintf(title, 60, "Unknown %d error at port %s ",
                    status, portName);
            break;
    }
    if(txStep == -1) //PeriodicTC
    {
        length = snprintf(title, 60-length, "PeriodicTC %d level %d:",
                input, curLevel);
    }
    else
    {
        length = snprintf(title, 60-length, "step %d, input %d, level %d:",
                txStep, input, curLevel);
    }
            
    switch(prepareErrorType)
    {
        case PREPARE_EXPORT_ERROR:
            DisplayExportInformationError(status, msg, maxMsgSize);
            break;

        case PREPARE_FORMAT_ERROR:
            ParseCheckFormatError(status, msg, maxMsgSize, port);
            break;

        case PREPARE_DICS_ERROR:
            DisplayPerformDICsError(status, msg, maxMsgSize);
            break;

        case PREPARE_NO_ERROR:
            break;

        default:
            break;
    }
    prepareErrorType = PREPARE_NO_ERROR;
    return;
}

void PrepareInput::CreatePortPhyHeaderOffset(unsigned char * packet, int length,
        portConfig * pPort)
{
    switch(pPort->portType)
    {
        case SPW_USB_MK2_PORT:
            packet[0] = pPort->config.spw.link;
            packet[1] = pPort->config.spw.writingPort;
            packet[2] = pPort->config.spw.protocolID;
            packet[3] = 0x00;
            packet[4] = 0x00;
            break;

        case SPW_USB_PORT:
            packet[0] = pPort->config.spw.link;
            packet[1] = pPort->config.spw.writingPort;
            packet[2] = pPort->config.spw.protocolID;
            packet[3] = 0x00;
            packet[4] = 0x00;
            break;

        case SPW_PCI_PORT:
            packet[0] = pPort->config.spw.writingPort;
            packet[1] = pPort->config.spw.protocolID;
            packet[2] = 0x00;
            packet[3] = 0x00;
            break;

        case UART_PORT: case TCP_SOCKET_SRV_PORT: case TCP_SOCKET_CLI_PORT:
            memcpy(packet, pPort->ptcl.syncPattern, pPort->ptcl.syncPatternLength);
            SettingNumber(length, packet, pPort->ptcl.sizeFieldTrimInBits,
                    pPort->ptcl.sizeFieldOffsetInBits);
            break;
            
        default:
            break;
    }
}