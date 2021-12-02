/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   periodicTCs.cpp
 * Author: Aaron
 *
 * Created on 26 de septiembre de 2017, 16:54
 */

#include <time.h>				/* clock_gettime */
#include <cstdio>
#include <QThread>
#include "PeriodicTC.h"
#include "PrepareInput.h"
#include "../PortTools/raw.h"

PeriodicTC::PeriodicTC(gssStructs * origGssStruct, MonitorPlots * origMonitors,
        Logs * origLogs, unsigned int idRef)
{
    pGssStruct = origGssStruct;
    pMonitors = origMonitors;
    pLogs = origLogs;
    
    id = idRef;
    value = 0;
    periodicEnabled = true;
}

void PeriodicTC::setPeriodicEnabled(bool state)
{
    periodicEnabled = state;
    if(state == false)
        sleepWaitCondition.wakeAll();
}

void PeriodicTC::performPeriodicTC()
{
    int status = 0, length = 0;
    unsigned char tcBuffer[MAX_LEVELS][MAX_PACKET_SIZE+MAX_PHY_HDR];
    char msg[MAX_MSG_SIZE];
    input * pPeriodicTC = pGssStruct->getPointerToPeriodicTC(id);
    unsigned int txPort = pPeriodicTC->ifRef;
    unsigned int msgLen = 0;
    unsigned int portPhyHeaderOffsetTC = pGssStruct->getPortPhyHeaderOffsetTC(txPort);
    unsigned char * dataPacket = &tcBuffer[0][portPhyHeaderOffsetTC];
    
    emit updatePeriod(id, 0.0, true);
    QMutex mutex;
    double elapsedMs = 0.0, elapsedLoopS = 0.0;
    timespec markIni, markEnd, lastLoop;
    clock_gettime(CLOCK_MONOTONIC, &markIni);
    do {
        lastLoop = markIni;
        clock_gettime(CLOCK_MONOTONIC, &markIni);
        /* print time since last loop */
        elapsedLoopS = markIni.tv_sec - lastLoop.tv_sec;
        elapsedLoopS += (markIni.tv_nsec - lastLoop.tv_nsec) / 1000000000.0;
        emit updatePeriod(id, elapsedLoopS, false);
        
        if(!periodicEnabled)
            break;
        PrepareInput prepare(pGssStruct);
        if((length = prepare.prepare(tcBuffer, portPhyHeaderOffsetTC,
                pPeriodicTC)) < 0)
        {
            char title[60];
            prepare.printError(length, title, msg, MAX_MSG_SIZE,
                    pGssStruct->getPortName(txPort), txPort, -1, id);
            pLogs->saveMsgToLog(MAIN_LOG_IDX, title, msg);
        }
        
        if(!periodicEnabled)
            break;
        status = WriteRaw(tcBuffer[0], length, pGssStruct->getPointerToPort(txPort));
        if((status < 0) || (status != length))
        {
            /* TX ERROR / MISSING BYTES */
            pLogs->printRawPacket(Logs::RAW_TX, NULL, status, 0, txPort);
            unsigned int msgLen = snprintf(msg, MAX_MSG_SIZE,
                    "%s port Tx Error: ", pGssStruct->getPortName(txPort));
            if(status < 0)
            {
                RawRWError(status, &msg[msgLen], MAX_MSG_SIZE-msgLen,
                        pGssStruct->getPointerToPort(txPort));
            }
            else
            {
                snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen,
                        "Sent only %d bytes, %d expected", status, length);
            }
            emit setStatusBarColor(QString(msg), 255, 0, 0);
            pLogs->SetTimeInLog(MAIN_LOG_IDX, NULL, false);
            pLogs->SetTimeInLog(txPort, NULL, false);
            periodicEnabled = false;
            break;
        }
        emit TxDataInc(txPort);
        //printSentPeriodicPacket;
        pLogs->printRawPacket(Logs::RAW_TX, tcBuffer[0], length,
                portPhyHeaderOffsetTC, txPort);
        msgLen = snprintf(msg, MAX_MSG_SIZE, "Sent PeriodicTC %d", id);
        pGssStruct->printPacketProtocolTypeSubtype(dataPacket,
                length-portPhyHeaderOffsetTC, txPort, msg, MAX_MSG_SIZE, &msgLen);
        snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen, " to %s port.\n\tData: ",
                pGssStruct->getPortName(txPort));
        pLogs->savePacketMsgToLogs(txPort, msg, tcBuffer[0], length,
                portPhyHeaderOffsetTC);
        /* Check if any monitor of a GlobalVar TC type has to be applied */
        if((status = pMonitors->CheckApplyMonitors(GVR_FILTERED_TC,
                dataPacket, txPort, -1, msg, MAX_MSG_SIZE)) < 0)
        {
            emit setStatusBarColor(QString(msg), 255, 0, 0);
        }
        /* wait for loop interval */
        clock_gettime(CLOCK_MONOTONIC, &markEnd);
        elapsedMs = (markEnd.tv_sec - markIni.tv_sec) * 1000.0;
        elapsedMs += (markEnd.tv_nsec - markIni.tv_nsec) / 1000000.0;
        if(!periodicEnabled)
            break;
        if(elapsedMs < pPeriodicTC->delayInMs)
        {
            /* controlled sleep if needed */
            mutex.lock();
            sleepWaitCondition.wait(&mutex, pPeriodicTC->delayInMs - elapsedMs);
            mutex.unlock();
        }
    } while(periodicEnabled);
    emit setCheckEnabled(id, false);
    emit finished();
}