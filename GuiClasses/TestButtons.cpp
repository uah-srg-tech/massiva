/**
 * \file	TestButtons.c
 * \brief	functions for prepare and coordinate tests (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		27/11/2012 at 15:37:19
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c)2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <QThread>
#include <QMessageBox>
#include <QFileDialog>
#include "TestButtons.h"
#include "../CommonClasses/Logs.h"
#include "../TesterClasses/TestManager.h"
#include "../TesterClasses/TPWorker.h"

TestButtons::TestButtons(gssStructs * origGssStruct, Logs * origLogs,
        InitialConfig * origInitialConfig, TestManager * origTestMgr,
        mainForm * origMainGui, RxTxInfo * origTxRxTabs,
        SpecialPackets * origSpecials, MonitorPlots * origMonitors)
{
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pInitialConfig = origInitialConfig;
    pTestMgr = origTestMgr;
    
    pMainGui = origMainGui;
    pSpecials = origSpecials;
    pTxRxTabs = origTxRxTabs;
    pMonitors = origMonitors;
    
    IsFullTest = false;
    testWasCanceled = false;
    for(unsigned int idx=0; idx<TEST_ERRORS; ++idx)
    {
        fullTestErrors[idx] = 0;
    }
    
    connect(this, SIGNAL(setEnabledControls(bool, bool, bool, bool)),
            pMainGui, SLOT(setEnabledControls(bool, bool, bool, bool)));
    connect(this, SIGNAL(setEnabledLaunch(bool)),
            pMainGui, SLOT(setEnabledLaunch(bool)));
    connect(this, SIGNAL(setEnabledLaunchAllStop(bool)),
            pMainGui, SLOT(setEnabledLaunchAllStop(bool)));
    connect(this, SIGNAL(setStatusBar(const QString &)),
            pMainGui, SLOT(setStatusBar(const QString &)));
    connect(this, SIGNAL(setStatusBarColor(const QString &, int, int, int)),
            pMainGui, SLOT (setStatusBarColor(const QString &, int, int, int)));
    connect(this, SIGNAL(ClearRxTxData()), pTxRxTabs, SLOT(ClearRxTxData()));
    connect(this, SIGNAL(setLoadLaunchAllItalic(bool)), 
            pMainGui, SLOT(setLoadLaunchAllItalic(bool)));
    connect(this, SIGNAL(SelectTP(unsigned int, bool)),
            pMainGui, SLOT(SelectTP(unsigned int, bool)));
    connect(this, SIGNAL (setRxOutput(unsigned int, unsigned int, unsigned int)),
            pMainGui, SLOT (setRxOutput(unsigned int, unsigned int, unsigned int)));
    
    connect(pMainGui->mainContent.tpLoadButton, SIGNAL (released()),
            this, SLOT (LoadTPButton()));
    connect(pMainGui->mainContent.tpLaunchButton, SIGNAL (released()),
            this, SLOT (LaunchTPButton()));
    connect(pMainGui->mainContent.tpLoadLaunchButton, SIGNAL (released()),
            this, SLOT (LoadAndLaunchTPButton()));
    connect(pMainGui->mainContent.tpLoadLaunchAllButton, SIGNAL (released()),
            this, SLOT (LoadAndLaunchAllTPButton()));
    connect(pMainGui->mainContent.cancelButton, SIGNAL (released()),
            this, SLOT (CancelButton()));
    
    connect(pMainGui->mainContent.actionSend_one_step,
            SIGNAL(triggered()), this, SLOT(SendOneStep()));
    connect(pMainGui->mainContent.actionSend_several_steps,
            SIGNAL(triggered()), this, SLOT(SendSeveralSteps()));
    
    reqActionNames[0] = "Reset global variable";
}

void TestButtons::CancelButton()
{
    char cancelMsg[MAX_MSG_SIZE];
    testWasCanceled = true;
    if(pTestMgr->cancelTest(cancelMsg, MAX_MSG_SIZE) == true)
    {
        /* test canceled here: restore buttons now */
        emit setEnabledControls(true, true, pTestMgr->getAnyTestLoaded(), true);
        emit setEnabledLaunchAllStop(true);
    }
    emit setStatusBarColor(QString(cancelMsg), 255, 255, 0);
    pGssStruct->setAutoTest(false);
}

void TestButtons::LoadTPButton()
{
    emit setEnabledLaunchAllStop(false);
    QThread * workerThread = new QThread();
    TPWorker * worker = new TPWorker(pGssStruct, pLogs, pInitialConfig,
            pTestMgr, pMainGui, pTxRxTabs, pSpecials, pMonitors, this);
    worker->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), worker, SLOT(LoadTP()));
    connect(workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    workerThread->start();
    return;
}

void TestButtons::LaunchTPButton()
{
    IsFullTest = false;
    if(askReqAction(false) == false)
        return;
    QThread * workerThread = new QThread();
    TPWorker * worker = new TPWorker(pGssStruct, pLogs, pInitialConfig,
            pTestMgr, pMainGui, pTxRxTabs, pSpecials, pMonitors, this);
    worker->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), worker, SLOT(LaunchTP()));
    connect(workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    workerThread->start();
    return;
}

void TestButtons::LoadAndLaunchTPButton()
{
    IsFullTest = false;
    if(askReqAction(false) == false)
        return;
    QThread * workerThread = new QThread();
    TPWorker * worker = new TPWorker(pGssStruct, pLogs, pInitialConfig,
            pTestMgr, pMainGui, pTxRxTabs, pSpecials, pMonitors, this);
    worker->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), worker, SLOT(LoadAndLaunchTP()));
    connect(workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    workerThread->start();
}

void TestButtons::LoadAndLaunchAllTPButton()
{
    char msg[MAX_MSG_SIZE];
    QMessageBox::StandardButton btnClicked = QMessageBox::NoButton;
    snprintf(msg, MAX_MSG_SIZE,
            "Are you sure you want to launch ALL TPs\nfrom %s?",
            pGssStruct->getProcedureName(pGssStruct->getCurrentTest()));
    btnClicked = QMessageBox::warning(pMainGui, "Warning!", msg,
            QMessageBox::Ok|QMessageBox::Cancel);
    if(btnClicked == QMessageBox::Ok)
    {
        LoadAndLaunchAllTPConfirmed();
    }
}

void TestButtons::LoadAndLaunchAllTPConfirmed()
{
    if(pGssStruct->getCurrentTest() == -1)
        pGssStruct->setCurrentTest(0);
    IsFullTest = true;
    for(unsigned int idx=0; idx<TEST_ERRORS; ++idx)
    {
        fullTestErrors[idx] = 0;
    }
    emit setLoadLaunchAllItalic(true);
    emit setEnabledLaunchAllStop(false);
    QThread * workerThread = new QThread();
    TPWorker * worker = new TPWorker(pGssStruct, pLogs, pInitialConfig,
            pTestMgr, pMainGui, pTxRxTabs, pSpecials, pMonitors, this);
    worker->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), worker, SLOT(LoadAndLaunchTP()));
    connect(workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    workerThread->start();
}

void TestButtons::SendOneStep()
{
    QString filenameString = QFileDialog::getOpenFileName(pMainGui,
            tr("Choose the file with the desired Step"),
            tr(pInitialConfig->GetFile(WORKSPACE_LAST)),
            tr("XML/XMI files (*.xml *.xmi)"));
    if(filenameString.isNull())
        return;
    
    IsFullTest = false;
    QThread * workerThread = new QThread();
    TPWorker * worker = new TPWorker(pGssStruct, pLogs, pInitialConfig,
            pTestMgr, pMainGui, pTxRxTabs, pSpecials, pMonitors, this);
    
    QByteArray filenameArray = filenameString.toLatin1();
    worker->setFilename(filenameArray.constData());
    pInitialConfig->SetFile(WORKSPACE_LAST, filenameArray.constData());
    
    worker->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), worker, SLOT(LoadAndLaunchOneStep()));
    connect(workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    workerThread->start();
}

void TestButtons::SendSeveralSteps()
{
    QString filenameString = QFileDialog::getOpenFileName(pMainGui,
            tr("Choose a file with the Step number list with ';'"),
            tr(pInitialConfig->GetFile(GSS_LAST)), tr("text (*.txt)"));
    if(filenameString.isNull())
        return;
    
    IsFullTest = false;
    QThread * workerThread = new QThread();
    TPWorker * worker = new TPWorker(pGssStruct, pLogs, pInitialConfig,
            pTestMgr, pMainGui, pTxRxTabs, pSpecials, pMonitors, this);
    
    QByteArray filenameArray = filenameString.toLatin1();
    worker->setFilename(filenameArray.constData());
    pInitialConfig->SetFile(WORKSPACE_LAST, filenameArray.constData());
    
    worker->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), worker, SLOT(LoadAndLaunchSeveralSteps()));
    connect(workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    workerThread->start();
}

bool TestButtons::RequestTestInit(const char * stepFilename)
{
    bool noOutputs = true;
    testWasCanceled = false;
    stepStruct * steps = pGssStruct->getPointerToSteps();
    
    emit setStatusBarColor("", 240, 240, 240);
    emit ClearRxTxData();
    int firstStepWithOutputs = pTestMgr->TestInit(stepFilename);//getFirstRxStep
    
    pMainGui->setMaximumTxStep(pGssStruct->getNumberOfSteps());
    pMainGui->setValueTxStep(0);
    pMainGui->setMaximumInput(steps[0].numberOfInputs);
    pMainGui->setValueInput(0);
    pMainGui->setVisibleTxStep(true);
    pMainGui->setVisibleInput(true);
    
    if(firstStepWithOutputs != -1)
    {
        emit setRxOutput(firstStepWithOutputs, 
                steps[firstStepWithOutputs].numberOfOutputs, 0);
        pMainGui->setMaximumRxStep(pGssStruct->getNumberOfSteps());
        pMainGui->setValueRxStep(0);
        pMainGui->setVisibleRxStep(true);
        pMainGui->setVisibleOutput(true);
        noOutputs = false;
    }
    return noOutputs;
}

bool TestButtons::checkIsFullTest()
{
    return IsFullTest;
}

void TestButtons::RequestTestFinish(testEndStatus status, unsigned int step,
        unsigned int input)
{
    char msg[MAX_MSG_SIZE];
    int r = 0, g = 0;
    status = pTestMgr->TestFinish(msg, MAX_MSG_SIZE, status, step, input);
    
    switch(status)
    {
        case TEST_ENDED:
            g = 255;
            break;
        case TEST_ENDED_WITH_TX_ERROR:
        case TEST_ENDED_WITH_ERRORS:
        case TEST_ENDED_WITH_NOT_DISCARDED_ERRORS:
            snprintf(&msg[strlen(msg)], MAX_MSG_SIZE-strlen(msg), ". See log");
            r = 255;
            break;
        case TEST_ENDED_ACTION_CHECK_KO:
        case TEST_CANCELED:
            r = 255;
            g = 255;
            break;
    }
    emit setStatusBarColor(QString(msg), r, g, 0);
    
    pMainGui->setVisibleTxStep(false);
    pMainGui->setVisibleInput(false);
    pMainGui->setVisibleRxStep(false);
    pMainGui->setVisibleOutput(false);
    
    if(IsFullTest)
    {
        if(testWasCanceled == true)
        {
            emit setLoadLaunchAllItalic(false);
            emit setEnabledControls(true, true, true, true);
            emit setStatusBarColor("All Test Procedures canceled", 255, 255, 0);
        }
        else
        {
            bool launchNextTp = false;
            bool continueFullTest = true; 
            bool allTestsFinished = false;
            pTestMgr->joinErrors(fullTestErrors);

            /* get current test and next test procedure numbers */
            pGssStruct->currentTestMutexLock();
            unsigned int currentTp = pGssStruct->getCurrentTest();
            unsigned int nextTp = currentTp;
            do {
                nextTp++;
            }while((nextTp < pGssStruct->getNumberOfProcedures()) &&
                    (!pGssStruct->getProcedureEnabled(nextTp)));

            /* if errors there are and still more tests then ask if continue */
            if(((status == TEST_ENDED_WITH_NOT_DISCARDED_ERRORS) ||
                    (status == TEST_ENDED_WITH_TX_ERROR)) &&
                    (pMainGui->LaunchAllIsChecked()) &&
                    (nextTp != pGssStruct->getNumberOfProcedures()))
            {
                QMessageBox::StandardButton btnClicked = QMessageBox::NoButton;
                btnClicked = QMessageBox::warning(pMainGui, "Warning!",
                        "Last Test finished with not discarded errors. Continue?",
                        QMessageBox::Yes|QMessageBox::No);
                if(btnClicked == QMessageBox::No)
                {
                    continueFullTest = false;
                }
            }

            /* if continue then check if there are more tps */
            if((continueFullTest) && (pMainGui->LaunchAllIsChecked()))
            {
                if(nextTp == pGssStruct->getNumberOfProcedures())
                {
                    /* no more test remaining */
                    emit setLoadLaunchAllItalic(false);
                    emit setEnabledControls(true, true, true, true);
                    emit setEnabledLaunchAllStop(true);

                    char msg[MAX_MSG_SIZE];
                    bool errors = false, notDiscardedErrors = false; 
                    pTestMgr->getTestErrorsMsg(fullTestErrors, msg, MAX_MSG_SIZE, 
                            "All Test Procedures ended", errors, notDiscardedErrors);
                    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg,
                            true, true, true);
                    if(notDiscardedErrors)
                    {
                        emit setStatusBarColor("All Test Procedures ended with discarded errors. See log",
                                255, 255, 0);
                    }
                    else
                    {
                        emit setStatusBarColor("All Test Procedures ended",
                                0, 255, 0);
                        allTestsFinished = true;
                    }
                }
                else
                {
                    /* go for next tp*/
                    emit SelectTP(currentTp, false);
                    emit SelectTP(nextTp, true);
                    /* force currentTest in case SelectTP goes late */
                    pGssStruct->setCurrentTest(nextTp);
                    launchNextTp = true;
                }
            }
            else
            {
                emit setLoadLaunchAllItalic(false);
                emit setEnabledControls(true, true, true, true);
                emit setStatusBarColor("Not All Test Procedures ended. See log",
                        255, 255, 0);
            }
            pGssStruct->currentTestMutexUnlock();
            if((allTestsFinished) && (pGssStruct->getAutoTest()))
            {
                QApplication::quit();
            }

            if(launchNextTp)
            {
                askReqAction(true);
                QThread * workerThread = new QThread();
                TPWorker * worker = new TPWorker(pGssStruct, pLogs, pInitialConfig,
                        pTestMgr, pMainGui, pTxRxTabs, pSpecials, pMonitors, this);
                worker->moveToThread(workerThread);
                connect(workerThread, SIGNAL(started()), worker, SLOT(LoadAndLaunchTP()));
                connect(workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
                workerThread->start();
            }
        }
    }
    else
    {
        emit setEnabledControls(true, true, pTestMgr->getAnyTestLoaded(), true);
        emit setEnabledLaunchAllStop(true);
        if(pGssStruct->getAutoTest())
            QApplication::quit();
    }
}

bool TestButtons::askReqAction(bool perform)
{
    bool ret = true;
    QMessageBox::StandardButton btnClicked;
    test_proc * pProcedure =
            pGssStruct->getPointerToProcedure(pGssStruct->getCurrentTest());
    if(pProcedure->prevAction != 0)
    {
        std::string message = reqActionNames[pProcedure->prevAction-1];
        int actionParam = -1;
        if(pProcedure->prevActionParam[0] == '\0')
        {
            message += "s";
        }
        else
        {
            actionParam = strtol(pProcedure->prevActionParam, NULL, 0);
            message += " " + std::to_string(actionParam) + " (" + 
                    pGssStruct->getGlobalVarName(actionParam) + ")";
        }
        /* if perform is true (from Full Test) don't ask */
        if(perform == false)
        {
            btnClicked = QMessageBox::question(pMainGui,
                    QString("Test prior action"),
                    QString::fromStdString(message),
                    QMessageBox::Yes|QMessageBox::No);
            if(btnClicked == QMessageBox::Yes)
                perform = true;
            else
                ret = false; /* test to be cancelled */
        }
        /* now performByDefault is false only if clicked "No" */
        if(perform)
        {
            switch((actions)pProcedure->prevAction)
            {
                case RESET_GLOBAL_VARS:
                    if(actionParam == -1)
                    {
                        pGssStruct->resetGlobalVarsValues();
                    }
                    else
                    {
                        pGssStruct->resetGlobalVarValue(actionParam);
                    }
                    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(),
                            message.c_str(), true, false, false);
                    break;

                default:
                    break;
            }
        }
    }
    else if((perform == false) && (pProcedure->prevMsg[0] != '\0'))
    {
        btnClicked = QMessageBox::question(pMainGui, QString("Test prior message"),
                QString(pProcedure->prevMsg),
                QMessageBox::Ok|QMessageBox::Cancel|QMessageBox::Discard);
        if(btnClicked == QMessageBox::Cancel)
            ret = false; /* test to be cancelled */
        else if(btnClicked == QMessageBox::Discard)
            pProcedure->prevMsg[0] = '\0';
    }
    return ret;
}