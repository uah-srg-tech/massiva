/**
 * \file	TestButtons.h
 * \brief	functions for prepare and coordinate tests (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/01/2014 at 15:30:23
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef TESTER_H
#define TESTER_H

#include <time.h>				/* clock_gettime */
#include <QObject>
#include "../Forms/mainForm.h"
#include "../GuiClasses/SpecialPackets.h"
#include "../GuiClasses/RxTxInfo.h"
#include "../GuiClasses/MonitorPlots.h"
#include "../TesterClasses/TestManager.h"

class TestButtons : public QObject
{
Q_OBJECT

public:
    TestButtons(gssStructs * origGssStruct, Logs * origLogs,
        InitialConfig * origInitialConfig, TestManager * origTestMgr,
        mainForm * origMainGui, RxTxInfo * origTxRxTabs,
        SpecialPackets * origSpecials, MonitorPlots * origMonitors);
    bool RequestTestInit(const char * stepFilename);
    bool checkIsFullTest();

public slots:
    void CancelButton();
    void LoadTPButton();
    void LaunchTPButton();
    void LoadAndLaunchTPButton();
    void LoadAndLaunchAllTPButton();
    void LoadAndLaunchAllTPConfirmed();
    void SendOneStep();
    void SendSeveralSteps();
    void RequestTestFinish(testEndStatus status, unsigned int step,
            unsigned int input);
    
signals:
    void ClearRxTxData(void);
    void setStatusBar(const QString &);
    void setStatusBarColor(const QString &, int r, int g, int b);
    void setEnabledControls(bool, bool, bool, bool);
    void setEnabledLaunch(bool);
    void setEnabledLaunchAllStop(bool);
    void setLoadLaunchAllItalic(bool);
    void SelectTP(unsigned int index, bool status);
    void setRxOutput(unsigned int currentStepNumber,
            unsigned int numberOfOutputs, unsigned int outputsReceived);
    
private:
    typedef enum{
        RESET_GLOBAL_VARS = 1,
    }actions;
    
    gssStructs * pGssStruct;
    Logs * pLogs;
    InitialConfig * pInitialConfig;
    TestManager * pTestMgr;
    
    mainForm * pMainGui;
    SpecialPackets * pSpecials;
    RxTxInfo * pTxRxTabs;
    MonitorPlots * pMonitors;
    
    bool IsFullTest;
    bool testWasCanceled;
    unsigned int fullTestErrors[TEST_ERRORS];
    std::string reqActionNames[MAX_NUMBER_ACTIONS];
    
    bool askReqAction(bool perform);
};

#endif /* TESTER_H */