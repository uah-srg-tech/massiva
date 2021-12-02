/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   sendRaw.cpp
 * Author: Aaron
 * 
 * Created on 18 de julio de 2017, 10:35
 */
#pragma GCC diagnostic ignored "-Wunused-result"

#include <cstdio>           /* fgets */
#include <cstring>          /* memset */
#include <time.h>           /* clock_gettime */
#include "sendRaw.h"
#include "../CommonTools/crc16.h"
#include "../CommonTools/GetSetFieldTools.h"
#include "../CheckTools/CheckFormatTools.h"
#include "../CheckTools/ExportSettingTools.h"
#include "../PortTools/raw.h"

sendRaw::sendRaw(gssStructs * origGssStruct, Logs * origLogs, TestManager * origTestMgr, 
        mainForm * origMainGui, MonitorPlots * origMonitors, 
        selectRawFileForm * origSelectRawFileForm, unsigned int origPort,
        char * origFilename, bool origLoop, unsigned int origIntervalMs,
        bool origDIC, bool origGlobalVars)
{
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pTestMgr = origTestMgr;
    pMainGui = origMainGui;
    pMonitors = origMonitors;
    pSelectRawFileForm = origSelectRawFileForm;
    
    port = origPort;
    strncpy(commandFilename, origFilename, strMaxLen);
    loop = origLoop;
    intervalMs = origIntervalMs;
    DIC = origDIC;
    GlobalVars = origGlobalVars;
    connect(this, SIGNAL(setEnabledControls(bool, bool, bool, bool)),
            pMainGui, SLOT(setEnabledControls(bool, bool, bool, bool)));
    connect(this, SIGNAL(setStatusBarColor(const QString &, int, int, int)),
            pMainGui, SLOT (setStatusBarColor(const QString &, int, int, int)));
    connect(this, SIGNAL(showPopUp(const QString &, const QString &, mainForm::PopUpType)),
            pMainGui, SLOT(showPopUp(const QString &, const QString &, mainForm::PopUpType)),
            Qt::BlockingQueuedConnection);
    
    connect(this, SIGNAL(setSendRawMenuEnable(unsigned int, bool)), 
            pSelectRawFileForm, SLOT(setSendRawMenuEnable(unsigned int, bool)),
            Qt::BlockingQueuedConnection);
}

void sendRaw::send()
{
    unsigned int numberOfActiveDICs = 0, * ActiveDICs = NULL;
    formatField * targetFields = NULL;
    unsigned int numberOfTargetFields = 0;
    unsigned int * numberOfcrcFields= NULL, ** crcFieldRefs = NULL;
    level * portLevel = pGssStruct->getPointerToLevel(port, 0);     
    portConfig * pPort = pGssStruct->getPointerToPort(port);
    
    unsigned int commandLen = 0, readLen = 0, lenRef = 0, data = 0, line = 0;
    unsigned char OutQueue[MAX_PACKET_SIZE+MAX_PHY_HDR], * dataPacket = NULL;
    char auxRead, OutQueueHex[(MAX_PACKET_SIZE+MAX_PHY_HDR)*2+1];
    char auxQueue[(MAX_PACKET_SIZE+MAX_PHY_HDR)*2+1];
    int status = 0;
    FILE * fp;
    char msg[MAX_MSG_SIZE];
    timespec markIni, markEnd;
    double elapsedMs = 0.0;
    unsigned int msgLen = 0, portPhyHeaderOffsetTC = pPort->portPhyHeaderOffsetTC;
    
    numberOfActiveDICs = portLevel->in.numberOfActiveDICs;
    ActiveDICs = portLevel->in.ActiveDICs;
    crcFieldRefs = portLevel->in.crcTCFieldRefs;
    numberOfcrcFields = portLevel->in.numberOfcrcTCFields;
    targetFields = portLevel->in.TCFields;
    numberOfTargetFields = portLevel->in.numberOfTCFields;
    lenRef = portLevel->in.TCFields[0].info.variable.fieldRef;
    fp = fopen(commandFilename, "rb");
    
    dataPacket = &OutQueue[portPhyHeaderOffsetTC];
    if(portPhyHeaderOffsetTC)
    {
        switch(pPort->portType)
        {
            case SPW_USB_MK2_PORT:
                OutQueue[0] = pPort->config.spw.link;
                OutQueue[1] = pPort->config.spw.writingPort;
                OutQueue[2] = pPort->config.spw.protocolID;
                OutQueue[3] = 0x00;
                OutQueue[4] = 0x00;
                break;

            case SPW_USB_PORT:
                OutQueue[0] = pPort->config.spw.link;
                OutQueue[1] = pPort->config.spw.writingPort;
                OutQueue[2] = pPort->config.spw.protocolID;
                OutQueue[3] = 0x00;
                OutQueue[4] = 0x00;
                break;

            case SPW_PCI_PORT:
                OutQueue[0] = pPort->config.spw.writingPort;
                OutQueue[1] = pPort->config.spw.protocolID;
                OutQueue[2] = 0x00;
                OutQueue[3] = 0x00;
                break;

            case UART_PORT: case SOCKET_SRV_PORT: case SOCKET_CLI_PORT:
                memcpy(OutQueue, pPort->ptcl.syncPattern,
                        pPort->ptcl.syncPatternLength);
                break;

            default:
                break;
        }
    }
    for(unsigned int fld=0; fld<numberOfTargetFields; ++fld)
    {
        targetFields[fld].exported = 1;
    }
    //remove "exported" if "Check GlobalVars" is enabled 
    if(GlobalVars)
    {
        globalVar *gVars = pGssStruct->getPointerToGlobalVars();
        for(unsigned int gv=0; gv<pGssStruct->getNumberOfGlobalVars(); ++gv)
        {
            if((gVars[gv].level == 0) && (gVars[gv].Interface == port))
            {
                targetFields[gVars[gv].fieldRef].exported = 0;
            }
        }
    }
    
    pTestMgr->setRawStatus(port, IN_PROGRESS);  
    do {
        clock_gettime(CLOCK_MONOTONIC, &markIni);
        status = 0;
        commandLen = 0;
        readLen = 0;
        line++;
        memset(dataPacket, 0, MAX_PACKET_SIZE+MAX_PHY_HDR);
        if(pTestMgr->getRawStatus(port) == CANCELED)
            break;
        
        //get packet and length
	do {
	    auxRead = fgetc(fp);
            OutQueueHex[commandLen] = auxRead;
            commandLen++;
	} while((auxRead != EOF) && (auxRead != '\n') && (auxRead != '#'));

        if(((commandLen == 1) && ((auxRead == '#') || (auxRead == '\n')))
                || ((commandLen == 2) && (auxRead == '\n'))) //\r\n
	{
            //full commented or blank line
            QString info;
            if(auxRead == '#')
            {
                //get the rest of the line before continue
                fgets((char*)auxQueue, MAX_PACKET_SIZE+MAX_PHY_HDR, fp);
                info = QString("Commented line ");
            }
            else
            {
                info = QString("Blank line ");
            }
            info += QString::number(line) + QString(" found at port ") +
                    QString::number(port) + QString(". Continue?");
            emit showPopUp(QString("Send raw ") + QString::number(port), info,
                    mainForm::PopUpType::QUESTION);
            if(pMainGui->getPopUpBtnClickedYes() == false)
            {
                pTestMgr->setRawStatus(port, CANCELED);
                break;
            }
            else
                continue;
        }
        else
        {
            //normal line
            if(auxRead == '#')
            {
                //comment in line: get the rest of the line before continue
                do {
                    auxRead = fgetc(fp);
                } while((auxRead != EOF) && (auxRead != '\n'));
            }
            
            //now parse and sent command
            commandLen /= 2;
            for(unsigned int pos=0; pos<commandLen; ++pos)
            {
                sscanf(&OutQueueHex[pos*2], "%02X", &data);
                dataPacket[pos] = (unsigned char)data;
            }
            if(GlobalVars)
            {
                /* Check if any globalVar TC has to be applied at upper level */
                pGssStruct->UpdateGlobalVarTC(dataPacket, targetFields,
                        numberOfTargetFields, port, 0);

                /* Check if any monitor of a GlobalVar TC type has to be applied */
                if((status = pMonitors->CheckApplyMonitors(GVR_FILTERED_TC,
                        dataPacket, port, -1, msg, MAX_MSG_SIZE)) < 0)
                {
                    status = -1;
                    break;
                }
            }
           if(DIC)
            {
                //add DIC length to packet
                for(unsigned int dic=0; dic<numberOfActiveDICs; ++dic)
                {
                    commandLen += targetFields[ActiveDICs[dic]].totalSizeInBits / 8;
                }
                //check format to fill totalSizeInBits (VBLE) or offsetInBits (CRC)
                if((status = CheckFormat(dataPacket, targetFields,
                        numberOfTargetFields, &commandLen, port)) < 0)
                {
                    msgLen = snprintf(msg, MAX_MSG_SIZE,
                            "Line %d: Error formatting CRC: " , line);
                    ParseCheckFormatError(status, &msg[msgLen], MAX_MSG_SIZE-msgLen,
                            port);
                    status = -1;
                    break;
                }
                int status = performDICs(numberOfActiveDICs, ActiveDICs,
                        crcFieldRefs, numberOfcrcFields, dataPacket, targetFields);
                if(status < 0)
                {
                    msgLen = snprintf(msg, MAX_MSG_SIZE,
                            "Line %d: Error performing CRC:" , line);
                    DisplayPerformDICsError(status, &msg[msgLen], MAX_MSG_SIZE-msgLen);
                    status = -1;
                }
            }
           if(dataPacket[0] != 0)
            {
                if((pPort->portType != SPW_TC_PORT) && (pPort->portType != DUMMY_PORT))
                {
                    //check length
                    unsigned int variableSize = 0;

                    if((status = GetFieldFromBufferAsUint(dataPacket, 0, MAX_PACKET_SIZE, 
                            portLevel->in.TCFields[lenRef].offsetInBits,
                            portLevel->in.TCFields[lenRef].totalSizeInBits, &variableSize)) < 0)
                    {
                        msgLen = snprintf(msg, MAX_MSG_SIZE, "Line %d: Error getting size:",
                                line);
                        ParseGetFieldError(status, &msg[msgLen], MAX_MSG_SIZE-msgLen);
                        status = -1;
                        break;
                    }

                    if((portLevel->in.TCFields[0].info.variable.refPower != variableField::NO_POWER)
                        && (variableSize > 1024))
                    {
                        snprintf(msg, MAX_MSG_SIZE, "Line %d: Can't calculate powers "
                                "bigger than 2^1024", line);
                        status = -1;
                        break;
                    }
                    else
                    {
                        switch(portLevel->in.TCFields[0].info.variable.refPower)
                        {
                            case variableField::NO_POWER:
                                readLen = variableSize;
                                break;

                            case variableField::BASE_2:
                                readLen = (unsigned int) pow(2, variableSize);
                                break;

                            case variableField::BASE_2_WITH_0:
                                if(variableSize == 0)
                                    readLen = 0;
                                else
                                    readLen = (unsigned int) pow(2, variableSize);
                                break;
                        }
                    }
                    readLen += portLevel->in.TCFields[0].info.variable.constSizeInBits/8;
                    if(commandLen != readLen)
                    {
                        snprintf(msg, MAX_MSG_SIZE, "Line %d: Length calculated %d "
                                "and given %d are different", line, commandLen, readLen);
                        status = -1;
                        break;
                    }

                    /* add portHeader data if needed */
                    if((portPhyHeaderOffsetTC) && (pPort->ptcl.sizeFieldTrimInBits != 0))
                    {
                        SettingNumber(commandLen, OutQueue,
                                pPort->ptcl.sizeFieldTrimInBits,
                                pPort->ptcl.sizeFieldOffsetInBits);
                    }
                }
                //send the command
                int fullLength = commandLen+portPhyHeaderOffsetTC;
                status = WriteRaw(OutQueue, fullLength,
                        pGssStruct->getPointerToPort(port));
                if((status < 0) || (status != fullLength))
                {
                    /* TX ERROR / MISSING BYTES */
                    pLogs->printRawPacket(Logs::RAW_TX, NULL, status, 0, port);
                    msgLen = snprintf(msg, MAX_MSG_SIZE, "Line %d: %s port Tx Error: ",
                            line, pGssStruct->getPortName(port));
                    if(status < 0)
                    {
                        RawRWError(status, &msg[msgLen], MAX_MSG_SIZE-msgLen,
                                pGssStruct->getPointerToPort(port));
                    }
                    else
                    {
                        snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen,
                                "Sent only %d bytes, %d expected", status, fullLength);
                    }
                    status = -1;
                    break;
                }
                emit TxDataInc(port);
                emit TxDataPrintPacket(port, OutQueue, fullLength, portPhyHeaderOffsetTC);

                pLogs->printRawPacket(Logs::RAW_TX, OutQueue, fullLength,
                        portPhyHeaderOffsetTC, port);
                msgLen = snprintf(msg, MAX_MSG_SIZE, "Sent packet");
                pGssStruct->printPacketProtocolTypeSubtype(dataPacket, commandLen, port,
                        msg, MAX_MSG_SIZE, &msgLen);
                snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen, " to %s port\n\tData: ",
                        pGssStruct->getPortName(port));
                pLogs->savePacketMsgToLogs(port, msg, OutQueue, fullLength,
                        portPhyHeaderOffsetTC);
                
                if(pPort->portType == SPW_TC_PORT)
                {
                    pSelectRawFileForm->modifyRawPeriodicalTC(OutQueue[0]);
                }
            }
        }
        if((auxRead == EOF) && (loop))
        {
            //never finishes until cancel
            line = 0;
            rewind(fp);
            auxRead = 0;
        }
        
        clock_gettime(CLOCK_MONOTONIC, &markEnd);
        elapsedMs = (markEnd.tv_sec - markIni.tv_sec) * 1000.0;
        elapsedMs += (markEnd.tv_nsec - markIni.tv_nsec) / 1000000.0;
        if(((loop) || (auxRead != EOF)) && (elapsedMs < intervalMs))
        {
            /* controlled sleep if needed */
            pTestMgr->setRawSleep(port, intervalMs - elapsedMs);
        }
    } while(auxRead != EOF);
    fclose(fp);
    
    if(status == -1)
    {
        emit setStatusBarColor(QString(msg), 255, 0, 0);
        pLogs->saveMsgToLog(port, NULL, msg);
    }
    pTestMgr->setRawStatus(port, FINISHED);
    emit setSendRawMenuEnable(port, true);
    
    //check if all send threads have been finished
    unsigned int finishPort = 0;
    for(finishPort=0; finishPort<MAX_INTERFACES; ++finishPort)
    {
        if(pTestMgr->getRawStatus(finishPort) == IN_PROGRESS)
            break;
    }
    if(finishPort == MAX_INTERFACES)
    {
        emit setEnabledControls(true, true, pTestMgr->getAnyTestLoaded(), true);
    }
}