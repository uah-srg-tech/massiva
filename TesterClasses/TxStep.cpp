/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TxStep.cpp
 * Author: Aaron
 * 
 * Created on 8 de junio de 2017, 16:03
 */

#include <cstdio>
#include <QThread>
#include "TxStep.h"
#include "PrepareInput.h"
#include "../PortTools/raw.h"

TxStep::TxStep(gssStructs * origGssStruct, Logs * origLogs,
        TestManager * origTestMgr, mainForm * origMainGui,
        SpecialPackets * pSpecials, RxTxInfo * origTxRxTabs,
        MonitorPlots * origMonitors, unsigned int origNumberOfSteps,
        unsigned int * origTxStepRef, unsigned int * origInputRef,
        SendStatus * origRetConcurValue)
{
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pTestMgr = origTestMgr;
    pMainGui = origMainGui;
    pTxRxTabs = origTxRxTabs;
    pMonitors = origMonitors;
    
    pTxStepRef = origTxStepRef;
    pInputRef = origInputRef;
    retConcurValue = origRetConcurValue;
    
    connect(this, SIGNAL(setStatusBar(const QString &)),
            pMainGui, SLOT(setStatusBar(const QString &)));
    connect(this, SIGNAL(setStatusBarColor(const QString &, int, int, int)),
            pMainGui, SLOT (setStatusBarColor(const QString &, int, int, int)));
    connect(this, SIGNAL(clearStatusBarColor()),
            pMainGui, SLOT(clearStatusBarColor()));
    connect(this, SIGNAL(TxDataInc (unsigned int)), 
            pTxRxTabs, SLOT(TxDataInc (unsigned int)));
    connect(this, SIGNAL(TxDataPrintPacket (unsigned int, const unsigned char *, int, unsigned)), 
            pTxRxTabs, SLOT(TxDataPrintPacket (unsigned int, const unsigned char *, int, unsigned)));
    if(*pTxStepRef == (origNumberOfSteps-1))
    {
        connect(this, SIGNAL(ToggleSpecial(const spec_in_step *)),
                pSpecials, SLOT(ToggleSpecial(const spec_in_step *)), Qt::BlockingQueuedConnection);
    }
    else
    {
        connect(this, SIGNAL(ToggleSpecial(const spec_in_step *)),
                pSpecials, SLOT(ToggleSpecial(const spec_in_step *)));
    }
    
    connect(this, SIGNAL(showPopUp(const QString &, const QString &, mainForm::PopUpType)),
            pMainGui, SLOT(showPopUp(const QString &, const QString &, mainForm::PopUpType)),
            Qt::BlockingQueuedConnection);
}

TxStep::SendStatus TxStep::send(bool isTestProcedure)
{
    int status = 0, length;
    unsigned int idx = 0;
    SendStatus sendStatus = SendStatus::OK;
    
    const stepStruct * currentStep = pGssStruct->getPointerToStep(*pTxStepRef);
    
    if((sendStatus = waitForPreviousStep(currentStep)) != SendStatus::OK)
    {
        return sendStatus;
    }
    for(idx=0; idx<currentStep->numberOfSpecials; ++idx)
    {
        emit ToggleSpecial(&currentStep->specials[idx]);
    }

    for(idx=0; idx<currentStep->replays; ++idx)
    {
        if(pTestMgr->getSessionStatus() == CANCELED)
        {
            return SendStatus::CANCELED;
        }
        for(*pInputRef=0; *pInputRef<currentStep->numberOfInputs; ++*pInputRef)
        {
            input * pCurrentInput = &currentStep->inputs[*pInputRef];
            const unsigned int txPort = pCurrentInput->ifRef;
            unsigned int portPhyHeaderOffsetTC = pGssStruct->getPortPhyHeaderOffsetTC(txPort);
            
            if(isTestProcedure)
            {
                pMainGui->setValueTxStep(*pTxStepRef+1);
                pMainGui->setMaximumInput(currentStep->numberOfInputs*currentStep->replays);
                pMainGui->setValueInput(idx * currentStep->numberOfInputs + *pInputRef + 1);
            }
            
            if(pCurrentInput->delayInMs > 1000) //1s
            {
                //first sleep the non-full second part
                msleep(pCurrentInput->delayInMs % 1000);

                //then get number of remaining seconds and waitForInt;
                unsigned int waitSeconds =
                        (pCurrentInput->delayInMs - pCurrentInput->delayInMs % 1000) / 1000;

                for(unsigned int sec=waitSeconds; sec>0; --sec)
                {
                    if(pTestMgr->getSessionStatus() == CANCELED)
                    {
                        return SendStatus::CANCELED;
                    }
                    snprintf(msg, MAX_MSG_SIZE, "Waiting %d seconds", sec);
                    msleep(1000);
                    emit setStatusBar(QString(msg));
                }
                emit clearStatusBarColor();
            }
            else
            {
                msleep(pCurrentInput->delayInMs); /* ms */
            }
            if((idx == 0) && (*pInputRef == 0))
            {
                char * pName = pGssStruct->getPointerToStepName(*pTxStepRef);
                if(pName[0] != 0)
                {
                    snprintf(msg, MAX_MSG_SIZE, "STEP %d: %s", *pTxStepRef, pName);
                    pLogs->saveMsgToLog(MAIN_LOG_IDX, NULL, msg);
                    emit setStatusBar(QString(msg));
                }
            }

            PrepareInput prepare(pGssStruct);
            if((length = prepare.prepare(tcBuffer, portPhyHeaderOffsetTC,
                    pCurrentInput)) < 0)
            {
                char title[60];
                prepare.printError(length, title, msg, MAX_MSG_SIZE,
                        pGssStruct->getPortName(txPort), txPort,
                        (int)*pTxStepRef, *pInputRef);
                pLogs->saveMsgToLog(MAIN_LOG_IDX, title, msg);
                return SendStatus::TX_ERROR;
            }
            if(pTestMgr->getSessionStatus() == CANCELED)
            {
                return SendStatus::CANCELED;
            }
            
            /* Send data */
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
                pLogs->saveMsgToLog(txPort, NULL, msg);
                return SendStatus::TX_ERROR;
            }
            pLogs->printRawPacket(Logs::RAW_TX, tcBuffer[0], length,
                    portPhyHeaderOffsetTC, txPort);
            unsigned char * dataPacket = &tcBuffer[0][portPhyHeaderOffsetTC];
            if(pGssStruct->getPortType(txPort) != DUMMY_PORT)
            {
                unsigned int msgLen = 0;
                if(!isTestProcedure)
                {
                    msgLen = snprintf(msg, MAX_MSG_SIZE, "Sent packet");
                }
                else
                {
                    if(idx != 0)
                    {
                        msgLen = snprintf(msg, MAX_MSG_SIZE,
                                "Step %d, Input %d (%d) Sent packet",
                                *pTxStepRef, *pInputRef, idx+1);
                    }
                    else
                    {
                        msgLen = snprintf(msg, MAX_MSG_SIZE,
                                "Step %d, Input %d Sent packet",
                                *pTxStepRef, *pInputRef);
                    }
                }
                pGssStruct->printPacketProtocolTypeSubtype(dataPacket,
                        length-portPhyHeaderOffsetTC, txPort, msg, MAX_MSG_SIZE,
                        &msgLen);
                snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen,
                        " to %s port\n\tData: ",
                        pGssStruct->getPortName(txPort));
                pLogs->savePacketMsgToLogs(txPort, msg, tcBuffer[0], length,
                        portPhyHeaderOffsetTC);
            }
            /* Check if any monitor of a GlobalVar TC type has to be applied */
            if((status = pMonitors->CheckApplyMonitors(GVR_FILTERED_TC,
                    dataPacket, txPort, -1, msg, MAX_MSG_SIZE)) < 0)
            {
                emit setStatusBarColor(QString(msg), 255, 0, 0);
            }
            emit TxDataInc(txPort);
            if(!isTestProcedure)
                emit TxDataPrintPacket(txPort, tcBuffer[0], length, portPhyHeaderOffsetTC);
            if(pTestMgr->getSessionStatus() == CANCELED)
            {
                return SendStatus::CANCELED;
            }
        }
    }
    return SendStatus::OK;
}
                
void TxStep::sendConcurrent()
{
    SendStatus status = SendStatus::OK;
    stepStruct * currentStep = NULL;
    while(1)
    {
        currentStep = pGssStruct->getPointerToStep(*pTxStepRef);
        if((status = send(true)) != SendStatus::OK)
            break;
        *pTxStepRef = currentStep->concurrent.next;
        if(currentStep->concurrent.nextIsNotConc)
            break;
    }
    if(retConcurValue != NULL)
        *retConcurValue = status;
    emit finished();
}

processStatus TxStep::waitForOutput(unsigned char * pPrevReceived,
        unsigned int prevStep, unsigned int prevOutput)
{
    snprintf(msg, MAX_MSG_SIZE, "Waiting for step %d output %d", prevStep,
            prevOutput);
    emit setStatusBar(QString(msg));
    while(*pPrevReceived == 0)
    {
        if(pTestMgr->getSessionStatus() == CANCELED)
        {
            return CANCELED;
        }
    }
    emit clearStatusBarColor();
    return IN_PROGRESS;
}

TxStep::SendStatus TxStep::performAction(actionStruct * action,
        const stepStruct * currentStep, bool isFullTest)
{
    char msg[MAX_MSG_SIZE];
    QString fullActionText;
    SendStatus sendStatus = SendStatus::OK;
    
    if((sendStatus = waitForPreviousStep(currentStep)) != SendStatus::OK)
    {
        return sendStatus;
    }
    for(unsigned int idx=0; idx<currentStep->numberOfSpecials; ++idx)
    {
        emit ToggleSpecial(&currentStep->specials[idx]);
    }
    
    /* set action text, adding "check" before if needed */
    if((action->type == actionStruct::TMTC_CHECKING_ACTION) ||
            (action->type == actionStruct::CHECKING_ACTION))
    {
        fullActionText = QString("Check ");
    }
    fullActionText += QString(action->actionMessage);
    
    /* sleep previous delay Time */
    if(action->delayInMs > 1000) //1s
    {
        //first sleep the non-full second part
        msleep(action->delayInMs % 1000);

        //then get number of remaining seconds and waitForInt;
        unsigned int waitSeconds =
                (action->delayInMs - action->delayInMs % 1000) / 1000;

        for(unsigned int sec=waitSeconds; sec>0; --sec)
        {
            if(pTestMgr->getSessionStatus() == CANCELED)
            {
                return SendStatus::CANCELED;
            }
            snprintf(msg, MAX_MSG_SIZE, "Waiting %d seconds before %s",
                    sec, fullActionText.toLatin1().constData());
            msleep(1000);
            emit setStatusBar(QString(msg));
        }
        emit clearStatusBarColor();
    }
    else
    {
        msleep(action->delayInMs); /* ms */
    }
    
    if(((action->type == actionStruct::TMTC_CHECKING_ACTION) && (!isFullTest)) ||
            (action->type == actionStruct::CHECKING_ACTION) ||
            (action->type == actionStruct::INSTRUCTION_ACTION))
    {
        QString fullActionTextWithSpan = fullActionText;
        if(action->spanInMs != 0)
        {
            fullActionTextWithSpan += QString(" for ") +
                    QString::number(action->spanInMs/1000) + QString(" seconds");
        }
        emit showPopUp(QString("Action"),
                fullActionTextWithSpan, mainForm::PopUpType::INFO);
    }
    
    /* sleep span Time */
    if(action->spanInMs > 1000) //1s
    {
        //first sleep the non-full second part
        msleep(action->spanInMs % 1000);

        //then get number of remaining seconds and waitForInt;
        unsigned int waitSeconds =
                (action->spanInMs - action->spanInMs % 1000) / 1000;

        for(unsigned int sec=waitSeconds; sec>0; --sec)
        {
            if(pTestMgr->getSessionStatus() == CANCELED)
            {
                return SendStatus::CANCELED;
            }
            snprintf(msg, MAX_MSG_SIZE, "%s for %d seconds",
                    fullActionText.toLatin1().constData(), sec);
            msleep(1000);
            emit setStatusBar(QString(msg));
        }
        emit clearStatusBarColor();
    }
    else
    {
        msleep(action->spanInMs); /* ms */
    }
    if(((action->type == actionStruct::TMTC_CHECKING_ACTION) && (!isFullTest)) ||
            (action->type == actionStruct::CHECKING_ACTION))
    {
        emit showPopUp(QString("Confirm checking action"),
                tr(action->actionMessage), mainForm::PopUpType::QUESTION);
        if(pMainGui->getPopUpBtnClickedYes() == false)
        {
            return SendStatus::ACTION_CHECK_KO;
        }
    }
    return SendStatus::OK;
}

TxStep::SendStatus TxStep::waitForPreviousStep(const stepStruct * currentStep)
{
    if(currentStep->prevStepIdRef != -1)
    {
        const stepStruct * previousStep = pGssStruct->getPointerToStep(currentStep->prevStepIdRef);
        if(waitForOutput(&previousStep->outputs[currentStep->outIdRefFromPrevStep].received,
                currentStep->prevStepIdRef, currentStep->outIdRefFromPrevStep)
                == CANCELED)
        {
            return SendStatus::CANCELED;
        }
        if((currentStep->mode == stepStruct::CONTINUOUS) &&
                (previousStep->mode == stepStruct::CONCURRENT))
        {
            for(unsigned int idx=0; idx<previousStep->concurrent.number; ++idx)
            {
                stepStruct * previousConcurrent =
                        pGssStruct->getPointerToStep(previousStep->concurrent.list[idx]);
                if(previousConcurrent->numberOfOutputs)
                {
                    if(waitForOutput(&previousConcurrent->outputs[previousConcurrent->numberOfOutputs-1].received,
                            previousStep->concurrent.list[idx],
                            previousConcurrent->numberOfOutputs-1) == CANCELED)
                    {
                        return SendStatus::CANCELED;
                    }
                }
            }
        }
    }
    return SendStatus::OK;
}

void TxStep::msleep(unsigned int msec)
{
    QMutex mutex;
    QWaitCondition waitCondition;
    
    mutex.lock();
    waitCondition.wait(&mutex, msec);
    mutex.unlock();
}