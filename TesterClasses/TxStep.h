/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TxStep.h
 * Author: Aaron
 *
 * Created on 8 de junio de 2017, 16:03
 */

#ifndef TXSTEP_H
#define TXSTEP_H

#include <QtCore>
#include "../Forms/mainForm.h"
#include "../CommonClasses/Logs.h"
#include "../GuiClasses/SpecialPackets.h"
#include "../GuiClasses/RxTxInfo.h"
#include "../GuiClasses/MonitorPlots.h"

class TxStep : public QObject
{
Q_OBJECT

public:
    enum class SendStatus {
        OK,
        CANCELED,
        TX_ERROR,
        ACTION_CHECK_KO
    };
    
    TxStep(gssStructs * origGssStruct, Logs * origLogs,
            TestManager * origTestMgr, mainForm * origMainGui,
            SpecialPackets * pSpecials, RxTxInfo * origTxRxTabs,
            MonitorPlots * origMonitors, unsigned int origNumberOfSteps,
            unsigned int * origTxStepRef, unsigned int * origInputRef,
            SendStatus * origRetConcurValue);
    SendStatus send(bool isTestProcedure);
    
    SendStatus performAction(actionStruct * action,
            const stepStruct * currentStep, bool isFullTest);

public slots:
    void sendConcurrent();
    
signals:
    void setStatusBar(const QString &);
    void setStatusBarColor(const QString &, int r, int g, int b);
    void clearStatusBarColor();
    void ToggleSpecial(const spec_in_step *special);
    void TxDataInc(unsigned int tab);
    void TxDataPrintPacket(unsigned int tab, const unsigned char * packet,
        int length, unsigned int portPhyHeaderOffset);
    void finished();
    void showPopUp(const QString &title, const QString &text,
            mainForm::PopUpType type);

private:
    gssStructs * pGssStruct;
    Logs * pLogs;
    TestManager * pTestMgr;
    mainForm * pMainGui;
    RxTxInfo * pTxRxTabs;
    MonitorPlots * pMonitors;
    
    unsigned int *pTxStepRef, *pInputRef;
    unsigned char tcBuffer[MAX_LEVELS][MAX_PACKET_SIZE+MAX_PHY_HDR];
    char msg[MAX_MSG_SIZE];
    SendStatus * retConcurValue;
    
    void msleep(unsigned int msec);
    
    SendStatus waitForPreviousStep(const stepStruct * currentStep);
    processStatus waitForOutput(unsigned char * pPrevReceived,
        unsigned int prevStep, unsigned int prevOutput);
};

#endif /* TXSTEP_H */

