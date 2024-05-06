/**
 * \file	EnqueueWorker.cpp
 * \brief	functions for send and receive packets (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/01/2014 at 15:30:23
 * Company:		Space Research Group, Universidad de AlcalÃÂ¡.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <cstdio>                               /* fprintf */
#include <cstring>                              /* strncpy, memset */
#include <QMutex>
#include "EnqueueWorker.h"
#include "RxPacket.h"
#include "../PortTools/raw.h"                   /* ReadRaw */
#include "../PortTools/configTCPSocket.h"
#include "../PortTools/configUDPSocket.h"
#include "../CommonTools/GetSetFieldTools.h"    /* GetFieldFromBufferAsUint */

//#define DEBUG_PRINT_EVERY_RX

EnqueueWorker::EnqueueWorker(gssStructs * origGssStruct, Logs * origLogs,
            TestManager * origTestMgr, mainForm * pMainGui, RxTxInfo * origTabs,
            MonitorPlots * origMonitors, SpecialPackets * pSpecials,
            TestButtons * origTestButtons, unsigned int port)
{
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pTestMgr = origTestMgr;
    pMonitors = origMonitors;
    rxPort = port;
    
    connect(this, SIGNAL(setStatusBar(const QString &)),
            pMainGui, SLOT(setStatusBar(const QString &)));
    connect(this, SIGNAL(setStatusBarColor(const QString &, int, int, int)),
           pMainGui, SLOT (setStatusBarColor(const QString &, int, int, int)));
    connect(this, SIGNAL (setRxOutput(unsigned int, unsigned int, unsigned int)),
            pMainGui, SLOT (setRxOutput(unsigned int, unsigned int, unsigned int)));
    
    connect (this, SIGNAL (IncreaseDisplaySpecialValue(unsigned int)),
            pSpecials, SLOT (IncreaseDisplaySpecialValueSlot(unsigned int)));
    connect (this, SIGNAL (SetEnabledSpecialPeriod(unsigned int, int)),
            pSpecials, SLOT (SetEnabledSpecialPeriodSlot(unsigned int, int)));
    connect (this, SIGNAL (DisplaySpecialPeriod(unsigned int, double, QColor)),
            pSpecials, SLOT (DisplaySpecialPeriodSlot(unsigned int, double, QColor)));
    connect (this, SIGNAL (DisplaySpecialField(unsigned int, unsigned int, int)),
            pSpecials, SLOT (DisplaySpecialFieldSlot(unsigned int, unsigned int, int)));
    connect (this, SIGNAL (DisplaySpecialField(unsigned int, unsigned int, double)),
            pSpecials, SLOT (DisplaySpecialFieldSlot(unsigned int, unsigned int, double)));
    
    connect(this, SIGNAL(RxDataInc(unsigned int)),
            origTabs, SLOT(RxDataInc(unsigned int)),
            Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(RxDataPrintPacket(unsigned int, const unsigned char *, int, unsigned)),
            origTabs, SLOT(RxDataPrintPacket(unsigned int, const unsigned char *, int, unsigned)),
            Qt::BlockingQueuedConnection);
    
    connect(this, SIGNAL(RequestTestFinish(testEndStatus, unsigned int, unsigned int)),
            origTestButtons, SLOT(RequestTestFinish(testEndStatus, unsigned int, unsigned int)),
            Qt::BlockingQueuedConnection);
}

/* process the receiving part of the test */
void EnqueueWorker::EnqueuePackets()
{
#ifdef DEBUG_PRINT_EVERY_RX
    unsigned int counter = 0;
#endif
    int status = 0;
    unsigned int length = 0;
    char rxErrorMsg[MAX_MSG_SIZE];
    unsigned char InQueue[MAX_PACKET_SIZE+MAX_PHY_HDR];
    
    unsigned int portPhyHeaderOffsetTM = pGssStruct->getPortPhyHeaderOffsetTM(rxPort);
    portConfig * pPort = pGssStruct->getPointerToPort(rxPort);

    if((pPort->portType == SPW_USB_PORT) || (pPort->portType == SPW_PCI_PORT) ||
             (pPort->portType == SPW_USB_MK2_PORT))
    {
        snprintf(rxErrorMsg, MAX_MSG_SIZE, "Flushing %s port...",
                pPort->name);
        emit setStatusBar(QString(rxErrorMsg));
    }
    status = FlushPort(pPort, InQueue, MAX_PACKET_SIZE+MAX_PHY_HDR);
    if(status)
    {
        snprintf(rxErrorMsg, MAX_MSG_SIZE,
                "Flushed %d packets at %s port", status, pPort->name);
        pLogs->saveMsgToLog(rxPort, NULL, rxErrorMsg);
        emit setStatusBar(QString(rxErrorMsg));
    }
    else if(rxPort == 0)
    {
        emit setStatusBar(QString(""));
    }
    
    int retry = 0;
    if(pPort->portType == TCP_SOCKET_SRV_PORT)
    {
        retry = 1;
    }
    do {
        if(pPort->portType == TCP_SOCKET_SRV_PORT)
        {
            snprintf(rxErrorMsg, MAX_MSG_SIZE,
                    "Waiting for connection at %s:%u (MASSIVA port %u (%s))", 
                    pPort->config.socket.localIp,
                    pPort->config.socket.localPort, rxPort, pPort->name);
            pLogs->saveMsgToLog(rxPort, NULL, rxErrorMsg);
            emit setStatusBarColor(QString(rxErrorMsg), 255, 255, 0);

            if((status = PrepareServer(pPort, rxPort, &retry)) != 0)
            {
                DisplayPrepareServerError(status, rxErrorMsg, MAIN_LOG_IDX,
                        pPort, rxPort);
                pLogs->SetTimeInLog(MAIN_LOG_IDX, rxErrorMsg, false);
                pLogs->SetTimeInLog(rxPort, rxErrorMsg, false);
                continue;
            }
            if(retry != 0)
            {
                snprintf(rxErrorMsg, MAX_MSG_SIZE,"Connected %s:%u\n",
                    pPort->config.socket.localIp,
                    pPort->config.socket.localPort);
                pLogs->saveMsgToLog(rxPort, NULL, rxErrorMsg);
                emit setStatusBarColor(QString(rxErrorMsg), 240, 240, 240);
            }
        }

        while(1)
        {
            /* Wait for and receive data */
            status = ReadRaw(InQueue, &length, pPort);
            if(status == 0)
            {
                /* device closed */
                break;
            }
            if(status < 0)
            {
                /* RX ERROR */
                pLogs->printRawPacket(Logs::RAW_RX, NULL, status, 0, rxPort);
                snprintf(rxErrorMsg, MAX_MSG_SIZE, "%s port Rx Error: ",
                        pPort->name);
                RawRWError(status, &rxErrorMsg[strlen(rxErrorMsg)],
                        MAX_MSG_SIZE-strlen(rxErrorMsg), pPort);
                pLogs->SetTimeInLog(MAIN_LOG_IDX, rxErrorMsg, false);
                pLogs->SetTimeInLog(rxPort, rxErrorMsg, false);
#ifdef DEBUG_PRINT_EVERY_RX
                pLogs->closeOpenLogs(true);
#endif
                emit setStatusBarColor(QString(rxErrorMsg), 255, 0, 0);
                continue;
            }
            else if(length > (MAX_PACKET_SIZE+MAX_PHY_HDR))
            {
                pLogs->printRawPacket(Logs::RAW_RX, InQueue, MAX_PACKET_SIZE+MAX_PHY_HDR,
                    portPhyHeaderOffsetTM, rxPort);
                snprintf(rxErrorMsg, MAX_MSG_SIZE, "%s port Rx Error: Packet "
                        "received is %d bytes, %d bigger than allowed",
                        pPort->name, length, length-MAX_PACKET_SIZE-MAX_PHY_HDR);
                pLogs->SetTimeInLog(MAIN_LOG_IDX, rxErrorMsg, false);
                pLogs->SetTimeInLog(rxPort, rxErrorMsg, false);
#ifdef DEBUG_PRINT_EVERY_RX
                pLogs->closeOpenLogs(true);
#endif
                emit setStatusBarColor(QString(rxErrorMsg), 255, 0, 0);
                continue;
            }
            pLogs->printRawPacket(Logs::RAW_RX, InQueue, length,
                    portPhyHeaderOffsetTM, rxPort);

            /* check header and get error message if any */
            if((status = CheckRawHeader(InQueue, length, pPort, rxPort)) != 0)
            {
                GetCheckRawHeaderError(status, rxErrorMsg, MAX_MSG_SIZE, pPort,
                        rxPort);
                pLogs->savePacketMsgToLogs(rxPort, rxErrorMsg, InQueue, length,
                        pPort->portPhyHeaderOffsetTM);
#ifdef DEBUG_PRINT_EVERY_RX
                pLogs->closeOpenLogs(true);
#endif
                continue;
            }
            emit RxDataInc(rxPort);

#ifdef DEBUG_PRINT_EVERY_RX
            snprintf(rxErrorMsg, MAX_MSG_SIZE, "Raw Header OK port %d packet %d",
                    rxPort, counter++);
            pLogs->SetTimeInLog(MAIN_LOG_IDX, rxErrorMsg, false);
            pLogs->SetTimeInLog(rxPort, rxErrorMsg, false);
            pLogs->closeOpenLogs(true);
#endif
            /* now check packet */
            int specialIndex = -1;
            bool notExpected = false;
            RxPacket rxPacket(pGssStruct, pLogs, pTestMgr, InQueue,
                    (unsigned int)length, portPhyHeaderOffsetTM, rxPort);
#ifdef DEBUG_PRINT_EVERY_RX
            pLogs->closeOpenLogs(true);
#endif
            if((status = rxPacket.CheckProcessPacket(specialIndex, notExpected)) < 0)
            {
                /* FIXME: action if processing error ? */
                continue;
            }
            if(specialIndex != -1)
            {
                special_packet_info * spInfo = pGssStruct->getPointerToSpecialInfo(specialIndex);
                ShowSpecialFields(specialIndex,
                        pGssStruct->getPointerToLevelsSpecialPacket(specialIndex),
                        spInfo->struct_show, spInfo->struct_show_field,
                        &InQueue[portPhyHeaderOffsetTM]);
                emit IncreaseDisplaySpecialValue(specialIndex);
                if(spInfo->type == special_packet_info::SPECIAL_PERIODIC)
                {
                    if(spInfo->counter == 1)
                    {
                        emit SetEnabledSpecialPeriod(specialIndex, true);
                    }
                    else
                    {
                        QColor color = Qt::black;
                        if(spInfo->outOfMinMax != 0.0)
                            color = QColor(255,140,0);//dark orange
                        emit DisplaySpecialPeriod(specialIndex, spInfo->intervalTimeMs/1000, color);
                    }
                }
            }
            else if(notExpected)
            {
                rxPacket.GetNotExpectedInfoMsg(rxErrorMsg, MAX_MSG_SIZE);
                emit setStatusBarColor(QString(rxErrorMsg), 255, 255, 0);
                emit RxDataPrintPacket(rxPort, InQueue, length, portPhyHeaderOffsetTM);
            }
            else
            {
                /* show received output at GSS interface */
                unsigned int step = 0;
                unsigned int numberOfOutputs = 0;
                unsigned int outputsReceived = 0;
                bool testFinished = false;
                if(rxPacket.GetStepInfo(step, numberOfOutputs, outputsReceived,
                        testFinished))
                {
                    emit setRxOutput(step, numberOfOutputs, outputsReceived);
                }
                if(testFinished)
                {
                    /* test finished: restore buttons */
                    emit RequestTestFinish(TEST_ENDED, 0, 0);
                }
            }
            pGssStruct->UpdateGlobalVarTM(&InQueue[portPhyHeaderOffsetTM], rxPort,
                    specialIndex);
            /* Check if any monitor of a GlobalVar TM type has to be applied */
            if((status = pMonitors->CheckApplyMonitors(GVR_FILTERED_TM,
                    &InQueue[portPhyHeaderOffsetTM], rxPort, specialIndex,
                    rxErrorMsg, MAX_MSG_SIZE)) < 0)
            {
                emit setStatusBarColor(QString(rxErrorMsg), 255, 0, 0);
            }
            memset(InQueue, 0, MAX_PACKET_SIZE+MAX_PHY_HDR);

#ifdef DEBUG_PRINT_EVERY_RX
            pLogs->SetTimeInLog(MAIN_LOG_IDX, "Packet checked", false);
            pLogs->SetTimeInLog(rxPort, "Packet checked", false);
            pLogs->closeOpenLogs(true);
#endif
        }
    }while(retry);
    if(pPort->portType == TCP_SOCKET_SRV_PORT)
    {
        UnprepareServer(pPort, rxPort);
    }
    else if((pPort->portType == TCP_SOCKET_CLI_PORT) &&
            (pPort->ptcl.portValid == 1))
    {
        snprintf(rxErrorMsg, MAX_MSG_SIZE, "Server connection at port %u"
                " closed. Reset MASSIVA for reconnect", rxPort);
        pLogs->SetTimeInLog(MAIN_LOG_IDX, rxErrorMsg, false);
        pLogs->SetTimeInLog(rxPort, rxErrorMsg, false);
    }
    emit finished();
}

void EnqueueWorker::ShowSpecialFields(int idx, levelOut * spLevels,
        int levelRef, int firstFld, const unsigned char * packet)
{
    int import=0;
    
    if(levelRef == -1)
        return;

    for(int lvl=0; lvl<=levelRef; ++lvl)
    {
        import += spLevels[lvl].importInBytes;
    }

    for(unsigned int fld=firstFld; fld<spLevels[levelRef].numberOfTMFields;++fld)
    {
        /* get data */
        /* fixme: only for number smaller or equal than 4 bytes (32 bits) */
        if(spLevels[levelRef].TMFields[fld].totalSizeInBits > 32)
            continue;
        
        unsigned int data = 0;
        GetFieldFromBufferAsUint(&packet[import], 0, MAX_PACKET_SIZE, 
                spLevels[levelRef].TMFields[fld].offsetInBits,
                spLevels[levelRef].TMFields[fld].totalSizeInBits, &data);

        /* show data in field */
        if(spLevels[levelRef].TMFields[fld].type == CSFORMULAFIELD)
        {
            double formulaData = data
                    * spLevels[levelRef].TMFields[fld].info.formula.slope
                    + spLevels[levelRef].TMFields[fld].info.formula.intercept;
            emit DisplaySpecialField(idx, fld-firstFld, formulaData);
        }
        else
        {
            emit DisplaySpecialField(idx, fld-firstFld, (int)data);
        }
    }
    return;
}
