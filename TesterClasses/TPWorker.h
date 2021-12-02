/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TPWorker.h
 * Author: Aaron
 *
 * Created on 8 de junio de 2017, 16:03
 */

#ifndef TPWORKER_H
#define TPWORKER_H

#include <QtCore>
#include "TestManager.h"
#include "TxStep.h"
#include "../CommonClasses/Logs.h"
#include "../Forms/mainForm.h"
#include "../GuiClasses/SpecialPackets.h"
#include "../GuiClasses/RxTxInfo.h"
#include "../GuiClasses/MonitorPlots.h"
#include "../GuiClasses/TestButtons.h"

class TPWorker : public QObject
{
Q_OBJECT
    
public:
    TPWorker(gssStructs * origGssStruct, Logs * origLogs,
            InitialConfig * origInitialConfig, TestManager * origTestMgr,
            mainForm * origMainGui, RxTxInfo * origTxRxTabs,
            SpecialPackets * origSpecials, MonitorPlots * origMonitors,
            TestButtons * origTestButtons);
    void setFilename(const char * origFilename);
    
public slots:
    void LoadTP();
    void LaunchTP();
    void LoadAndLaunchTP();
    void LoadAndLaunchOneStep();
    void LoadAndLaunchSeveralSteps();

signals:
    void setEnabledControls(bool, bool, bool, bool);
    void setEnabledLaunch(bool);
    void setEnabledLaunchAllStop(bool);
    void setStatusBar(const QString &);
    void setStatusBarColor(const QString &, int r, int g, int b);
    void clearStatusBarColor();
    void removeFormatTestProcedure(unsigned int test);
    void setTpLoaded(const QString &);
    
    void RequestTestFinish(testEndStatus status, unsigned int step,
            unsigned int input);

private:
    static const int strMaxLen = InitialConfig::strMaxLen;
    
    gssStructs * pGssStruct;
    Logs * pLogs;
    InitialConfig * pInitialConfig;
    TestManager * pTestMgr;
    mainForm * pMainGui;
    SpecialPackets * pSpecials;
    RxTxInfo * pTxRxTabs;
    MonitorPlots * pMonitors;
    TestButtons * pTestButtons;
    char filename[strMaxLen];
};

#endif /* TPWORKER_H */

