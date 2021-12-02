/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TPWorker.cpp
 * Author: Aaron
 * 
 * Created on 8 de junio de 2017, 16:03
 */
#ifndef _WIN32
#pragma GCC diagnostic ignored "-Wformat-truncation="
#endif

#include <dirent.h>
#include <cstdio>
#include <vector>
#include <QFuture>
#include "TPWorker.h"
#include "../CommonClasses/Logs.h"
#include "../XMLParsingTools/XMLTPTools.h"
#include "../XMLParsingTools/XMLTPStepTools.h"
#include "../XMLParsingTools/XMLTools.h"

TPWorker::TPWorker(gssStructs * origGssStruct, Logs * origLogs,
        InitialConfig * origInitialConfig, TestManager * origTestMgr,
        mainForm * origMainGui, RxTxInfo * origTxRxTabs,
        SpecialPackets * origSpecials, MonitorPlots * origMonitors,
        TestButtons * origTestButtons)
{
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pInitialConfig = origInitialConfig;
    pTestMgr = origTestMgr;
    pMainGui = origMainGui;
    pSpecials = origSpecials;
    pTxRxTabs = origTxRxTabs;
    pMonitors = origMonitors;
    pTestButtons = origTestButtons;
    
    memset(filename, 0, strMaxLen);
    
    connect(this, SIGNAL(setEnabledControls(bool, bool, bool, bool)),
            pMainGui, SLOT(setEnabledControls(bool, bool, bool, bool)));
    connect(this, SIGNAL(setEnabledLaunch(bool)),
            pMainGui, SLOT(setEnabledLaunch(bool)));
    connect(this, SIGNAL(setEnabledLaunchAllStop(bool)),
            pMainGui, SLOT(setEnabledLaunchAllStop(bool)));
    connect(this, SIGNAL(setStatusBar(const QString &)),
            pMainGui, SLOT (setStatusBar(const QString &)));
    connect(this, SIGNAL(setStatusBarColor(const QString &, int, int, int)),
            pMainGui, SLOT (setStatusBarColor(const QString &, int, int, int)));
    connect(this, SIGNAL(clearStatusBarColor()),
            pMainGui, SLOT(clearStatusBarColor()));
    connect(this, SIGNAL(removeFormatTestProcedure(unsigned int)), 
            pMainGui, SLOT(removeFormatTestProcedure(unsigned int)));
    connect(this, SIGNAL(setTpLoaded(const QString &)), 
            pMainGui->mainContent.tpLoaded, SLOT(setText(const QString &)));
    
    connect(this, SIGNAL(RequestTestFinish(testEndStatus, unsigned int, unsigned int)),
            origTestButtons, SLOT(RequestTestFinish(testEndStatus, unsigned int, unsigned int)),
            Qt::BlockingQueuedConnection);
}

void TPWorker::setFilename(const char * origFilename)
{
    strncpy(filename, origFilename, strMaxLen);
    /* add \0 if origFilename is longer than strMaxLen */
    if(strlen(origFilename) > strMaxLen)
        filename[strMaxLen-1] = '\0';
}

void TPWorker::LoadTP()
{
    int currentTest = pGssStruct->getCurrentTest();
    int status = 0;
    char msg[MAX_MSG_SIZE];
    
    emit clearStatusBarColor();
    emit setEnabledControls(false, true, true, true);
    pMainGui->setVisibleTxStep(false);
    pMainGui->setVisibleInput(false);
    pMainGui->setVisibleRxStep(false);
    pMainGui->setVisibleOutput(false);
    emit setTpLoaded("");
    pGssStruct->freeSteps();
    pTestMgr->setAnyTestLoaded(false);
    pTestMgr->setParsingStatus(IN_PROGRESS);
    snprintf(msg, MAX_MSG_SIZE, "Test %s loading",
            pGssStruct->getProcedureName(currentTest));
    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, false, false, false);
    
    status = ParseTestProcedure(pGssStruct->getProcedureFilename(currentTest),
            pGssStruct->getPointerToConfig(),
            pGssStruct->getPointerToProtocols(),
            pGssStruct->getPointerToPorts(),
            pGssStruct->getPointerToPointerToSteps(),
            pGssStruct->getPointerToNumberOfSteps(),
            pTestMgr->getPointerToParsingStatus());

    emit setEnabledControls(true, true, false, true);
    emit setEnabledLaunchAllStop(true);
    if(pTestMgr->getParsingStatus() == CANCELED)
    {
        snprintf(msg, MAX_MSG_SIZE, "Test load canceled");
    }
    if(status != 0) /* this status is from ParseTestProcedure */
    {
        DisplayParseTestProcedureError(status, msg, MAX_MSG_SIZE);
        emit setStatusBarColor(tr(msg), 255, 255, 0);
    }
    if((pTestMgr->getParsingStatus() == CANCELED) || (status != 0))
    {
        pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, true, false, false);
        pGssStruct->freeSteps();
        pTestMgr->setParsingStatus(CANCELED);
        return;
    }
    
    emit setTpLoaded(pGssStruct->getProcedureName(currentTest));
    emit setEnabledLaunch(true);
    snprintf(msg, MAX_MSG_SIZE, "Test %s loaded",
            pGssStruct->getProcedureName(currentTest));
    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, true, false, false);
    pTestMgr->setParsingStatus(FINISHED);
    pTestMgr->setAnyTestLoaded(true);
    return;
}

void TPWorker::LaunchTP()
{
    emit setEnabledControls(false, true, true, false);
    
    TxStep::SendStatus status = TxStep::SendStatus::OK;
    bool noOutputs = false;
    unsigned int numberOfSteps = pGssStruct->getNumberOfSteps();
    unsigned int curTxStep = 0, curInput = 0;
    stepStruct * steps = pGssStruct->getPointerToSteps();
    
    noOutputs = pTestButtons->RequestTestInit(filename);
    do {
        if(steps[curTxStep].mode == stepStruct::ACTION)
        {
            TxStep tx(pGssStruct, pLogs, pTestMgr, pMainGui, pSpecials,
                    pTxRxTabs, pMonitors, numberOfSteps, &curTxStep, NULL, NULL);
            if((status = tx.performAction(steps[curTxStep].action,
                    &steps[curTxStep], pTestButtons->checkIsFullTest())) != TxStep::SendStatus::OK)
            {
                break;
            }
            curTxStep++;
            curInput = 0;
        }
        if(steps[curTxStep].mode == stepStruct::CONTINUOUS)
        {
            TxStep tx(pGssStruct, pLogs, pTestMgr, pMainGui, pSpecials,
                    pTxRxTabs, pMonitors, numberOfSteps, &curTxStep, &curInput,
                    NULL);
            if((status = tx.send(true)) != TxStep::SendStatus::OK)
            {
                break;
            }
            curTxStep++;
            curInput = 0;
        }
        else if(steps[curTxStep].mode == stepStruct::CONCURRENT)
        {
            /* create threads for concurrent steps */
            unsigned int idx=0, concurrents = steps[curTxStep].concurrent.number;
            unsigned int * concurTxStep = new unsigned int[concurrents];
            unsigned int * concurInput = new unsigned int[concurrents];
            QThread ** concurThread = new QThread *[concurrents];
            TxStep::SendStatus * sendStatus = new TxStep::SendStatus[concurrents];
            
            for(idx=0; idx<concurrents; ++idx)
            {
                concurTxStep[idx] = steps[curTxStep].concurrent.list[idx];
                concurInput[idx] = 0;
                sendStatus[idx] = TxStep::SendStatus::OK;
                
                concurThread[idx] = new QThread();
                TxStep * txWorker = new TxStep(pGssStruct, pLogs, pTestMgr,
                        pMainGui, pSpecials, pTxRxTabs, pMonitors, numberOfSteps,
                        &concurTxStep[idx], &concurInput[idx], &sendStatus[idx]);
                txWorker->moveToThread(concurThread[idx]);
                connect(concurThread[idx], SIGNAL(started()),
                        txWorker, SLOT(sendConcurrent()), Qt::DirectConnection);
                connect(txWorker, SIGNAL(finished()),
                        concurThread[idx], SLOT(quit()), Qt::DirectConnection);
                connect(concurThread[idx], SIGNAL(finished()),
                        concurThread[idx], SLOT(deleteLater()));
                concurThread[idx]->start();
            }
            /* use this thread for first concurrent step (current step) */
            unsigned int launchingTxStep = curTxStep;
            TxStep txWorker(pGssStruct, pLogs, pTestMgr, pMainGui, pSpecials,
                    pTxRxTabs, pMonitors, numberOfSteps, &curTxStep, &curInput,
                    &status);
            txWorker.sendConcurrent();
            
            /* wait for concurrent steps to finish */
            for(idx=0; idx<concurrents; ++idx)
            {
                concurThread[idx]->wait();//blocking wait
            }      

            /* check return values and next steps from concurrent threads */
            for(idx=0; idx<concurrents; ++idx)
            {
                /* check return values are not < 0 */
                if(sendStatus[idx] != TxStep::SendStatus::OK)
                {
                    /* error from TxStep::send is logged inside method itself*/
                    /* get step and input of the error - from sendConcurrent */
                    curTxStep = concurTxStep[idx];
                    curInput = concurInput[idx];
                    status = sendStatus[idx];
                    break;
                }
                /* check all next steps are the same */
                /* comparing them with next step at first concurrent step */
                if((pTestMgr->getSessionStatus() != CANCELED) &&
                        (concurTxStep[idx] != curTxStep))
                {
                    char msg[MAX_MSG_SIZE];
                    snprintf(msg, MAX_MSG_SIZE, "Concurrency convergence error "
                            "between steps %d (%d) and %d (%d)", idx,
                            concurTxStep[idx], launchingTxStep, curTxStep);
                    pLogs->saveMsgToLog(MAIN_LOG_IDX, NULL, msg);
                    /* get step and input which differs - from sendConcurrent */
                    curTxStep = concurTxStep[idx];
                    curInput = concurInput[idx];
                    status = TxStep::SendStatus::TX_ERROR;
                    break;
                }
            }
            delete[] concurTxStep;
            delete[] concurInput;
            delete[] sendStatus;
            
            /* if error produced in concurrents or in first thread (status) */
            /* break curTxStep do-while */
            if((idx != concurrents) || (status != TxStep::SendStatus::OK))
            {
                break;
            }
        }
    } while(curTxStep < numberOfSteps);

    if(status == TxStep::SendStatus::CANCELED)
    {
        emit RequestTestFinish(TEST_CANCELED, curTxStep, curInput);
    }
    else if(status == TxStep::SendStatus::TX_ERROR)//from send or sendConcurrent
    {
        emit RequestTestFinish(TEST_ENDED_WITH_TX_ERROR, curTxStep, curInput);
    }
    else if(status == TxStep::SendStatus::ACTION_CHECK_KO)//from performAction
    {
        emit RequestTestFinish(TEST_ENDED_ACTION_CHECK_KO, curTxStep, curInput);
    }
    else if(noOutputs)
    {
        emit RequestTestFinish(TEST_ENDED, 0, 0);
    }
    else if(curTxStep == numberOfSteps)
    {
        if(pTestMgr->getAllRxStepFinished())
            emit RequestTestFinish(TEST_ENDED, 0, 0);
        else
            pTestMgr->setAllTxStepFinished();
    }
    return;
}

void TPWorker::LoadAndLaunchTP()
{
    LoadTP();
    if(pTestMgr->getParsingStatus() == CANCELED)
    {
        return;
    }
    LaunchTP();
    return;
}

void TPWorker::LoadAndLaunchOneStep()
{
    int status = 0;
    unsigned int port = 0, stepNumber = 0;
    char msg[MAX_MSG_SIZE];
    char auxFilename[strMaxLen];
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL;
    levelTypeError levelError = TP_STEP;
    
    emit clearStatusBarColor();
    emit setEnabledControls(false, true, true, true);
    emit setTpLoaded("");
    if(pGssStruct->getCurrentTest() != -1)
    {
        emit removeFormatTestProcedure(pGssStruct->getCurrentTest());
        pGssStruct->setCurrentTest(-1);
        pGssStruct->freeSteps();
    }
    
    /* create step struct */
    stepStruct ** ppSteps = pGssStruct->getPointerToPointerToSteps();
    
    /* create 1 step */
    *ppSteps = (stepStruct*)calloc(1, sizeof(stepStruct));
    pGssStruct->setNumberOfSteps(1);
    
    snprintf(msg, MAX_MSG_SIZE, "Test step %s loading", filename);
    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, false, false, false);
    emit setStatusBar(tr(msg));
    
    /* Open XML Document */
    doc = xmlParseFile(filename);
    if(doc == NULL)
    {
        strncpy(msg, "Error while parsing document", MAX_MSG_SIZE);
    }
    else
    {
        /* get step (root) handle */
        root = xmlDocGetRootElement(doc);
        if (root == NULL)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            strncpy(msg, "Error while getting root handle", MAX_MSG_SIZE);
        }
    }
    if((root == NULL) || (doc == NULL))
    {
        pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, false, false, false);
        emit setStatusBarColor(tr(msg), 255, 255, 0);
        emit setEnabledControls(true, true, false, true);
        return;
    }
    if(strncmp((const char*)root->name, "step", 4))
    {
        strncpy(msg, "Not a valid step file", MAX_MSG_SIZE);
        pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, false, false, false);
        emit setStatusBarColor(tr(msg), 255, 255, 0);
        emit setEnabledControls(true, true, false, true);
        return;
    }
    
    /* Initialize step struct */
    ppSteps[0]->name[0] = 0;
    ppSteps[0]->prevStepIdRef = -1;
    ppSteps[0]->outIdRefFromPrevStep = -1;
    ppSteps[0]->mode = stepStruct::CONTINUOUS;
    ppSteps[0]->intervalInMs = 0.0;
    ppSteps[0]->outputsCheckmode = stepStruct::ALL;
    ppSteps[0]->outputs = NULL;
    ppSteps[0]->numberOfOutputs = 0;
    ppSteps[0]->outputsReceived = 0;
    for(port=0; port<MAX_INTERFACES; ++port)
    {
        ppSteps[0]->numberOfOutputsAtPort[port] = 0;
        ppSteps[0]->outputsReceivedAtPort[port] = 0;
        ppSteps[0]->nextStepWithOutputsAtPort[port] = -1;
    }
    ppSteps[0]->specials = NULL;
    ppSteps[0]->numberOfSpecials = 0;
    ppSteps[0]->concurrent.list = NULL;
    ppSteps[0]->concurrent.number = 0;
    ppSteps[0]->concurrent.next = 0;
    ppSteps[0]->concurrent.nextIsNotConc = 0;
    ppSteps[0]->replays = 1;
    
    pTestMgr->setAnyTestLoaded(false);
    pTestMgr->setParsingStatus(IN_PROGRESS);
    status = ParseStep(doc, root, pGssStruct->getPointerToConfig(),
            pGssStruct->getPointerToProtocols(),
            pGssStruct->getPointerToPorts(), ppSteps, stepNumber, auxFilename,
            &levelError, pTestMgr->getPointerToParsingStatus(), 1,
            pGssStruct->getNumberOfSteps(), NULL);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    
    if(pTestMgr->getParsingStatus() == CANCELED)
    {
        snprintf(msg, MAX_MSG_SIZE, "Test step load canceled");
    }
    if(status != 0) /* this status is from ParseStep */
    {
        DisplayParseStepError(status, 0, filename, levelError, msg, MAX_MSG_SIZE);
        emit setStatusBarColor(tr(msg), 255, 255, 0);
        pTestMgr->setParsingStatus(FINISHED);
    }
    if((pTestMgr->getParsingStatus() == CANCELED) || (status != 0))
    {
        pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, true, false, false);
        pGssStruct->freeSteps();
        emit setEnabledControls(true, true, false, true);
        return;
    }
    
    snprintf(msg, MAX_MSG_SIZE, "Test step %s loaded", filename);
    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, true, false, false);
    emit setStatusBar(tr(msg));
    emit setTpLoaded(filename);
    ppSteps[0]->mode = stepStruct::CONTINUOUS; //to force it if xml file was not
    pTestMgr->setParsingStatus(FINISHED);
    LaunchTP();
}

void TPWorker::LoadAndLaunchSeveralSteps()
{
    int status = 0, lastStepWithOutputsAtPort[MAX_INTERFACES];
    unsigned int port = 0, stepsNumber = 0, currStep = 0;
    char auxString[130];
    char msg[MAX_MSG_SIZE], stepFilename[strMaxLen];
    unsigned int * stepList = NULL;
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL;
    levelTypeError levelError = TP_STEP;
    unsigned int prevStepIdRefSaved = -1, outIdRefFromPrevStepSaved = -1;
    DIR *dirp;
    struct dirent *dp;
    bool stepFound = false;
    
    emit clearStatusBarColor();
    emit setEnabledControls(false, true, true, true);
    emit setTpLoaded("");
    
    snprintf(msg, MAX_MSG_SIZE, "Test step list %s loading", filename);
    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, false, false, false);
    emit setStatusBar(tr(msg));
    
    /* Parse step list file */
    FILE * stepListFile = fopen(filename, "r");
    if(stepListFile == NULL)
    {
        strncpy(msg, "Error while opening step list file", MAX_MSG_SIZE);
        status = -1;
    }
    else
    {
        /* first pass for checking list consistency */
        while(1)
        {
            status = fscanf (stepListFile, "%d;", &currStep);
            if(status != 1)
                break;
            stepsNumber++;
        }
        if(status != EOF)
        {
            fclose(stepListFile);
            strncpy(msg, "Error while parsing step list file", MAX_MSG_SIZE);
            status = -1;
        }
        else
        {
            stepList = new unsigned int[stepsNumber];
            rewind(stepListFile);

            /* second pass for saving step numbers*/
            for(currStep=0; currStep<stepsNumber; ++currStep)
            {
                status = fscanf (stepListFile, "%d;", &stepList[currStep]);
                if(status != 1)
                    break;
            }
            fclose(stepListFile);
            if(status != 1)
            {
                strncpy(msg, "Error while parsing step list file", MAX_MSG_SIZE);
                status = -1;
            }
        }
    }
    if(status == -1)
    {
        emit setStatusBarColor(tr(msg), 255, 255, 0);
        emit setEnabledControls(true, true, false, true);
        return;
    }
    status = 0;
    
    if(pGssStruct->getCurrentTest() != -1)
    {
        emit removeFormatTestProcedure(pGssStruct->getCurrentTest());
        pGssStruct->setCurrentTest(-1);
        pGssStruct->freeSteps();
    }
    
    /* create step struct */
    stepStruct ** ppSteps = pGssStruct->getPointerToPointerToSteps();
    *ppSteps = (stepStruct*)calloc(stepsNumber, sizeof(stepStruct));
    pGssStruct->setNumberOfSteps(stepsNumber);
    
    /* initialize step struct */
    for(currStep=0; currStep<stepsNumber; ++currStep)
    {
        stepStruct * pStep = pGssStruct->getPointerToStep(currStep);
        pStep->prevStepIdRef = -1;
        pStep->outIdRefFromPrevStep = -1;
        pStep->mode = stepStruct::CONTINUOUS;
        pStep->inputs = NULL;
        pStep->numberOfInputs = 0;
        pStep->intervalInMs = 0.0;
        pStep->outputsCheckmode = stepStruct::ALL;
        pStep->outputs = NULL;
        pStep->numberOfOutputs = 0;
        pStep->outputsReceived = 0;
        for(port=0; port<MAX_INTERFACES; ++port)
        {
            pStep->numberOfOutputsAtPort[port] = 0;
            pStep->outputsReceivedAtPort[port] = 0;
            pStep->nextStepWithOutputsAtPort[port] = -1;
        }
        pStep->specials = NULL;
        pStep->numberOfSpecials = 0;
        pStep->concurrent.list = NULL;
        pStep->concurrent.number = 0;
        pStep->concurrent.next = 0;
        pStep->concurrent.nextIsNotConc = 0;
        pStep->replays = 1;
    }

    memset(lastStepWithOutputsAtPort, -1, MAX_INTERFACES*sizeof(int));
    pTestMgr->setAnyTestLoaded(false);
    pTestMgr->setParsingStatus(IN_PROGRESS);
    
    for(currStep=0; currStep<stepsNumber; ++currStep)
    {
        memset(stepFilename, 0, strMaxLen);
        snprintf(stepFilename, strMaxLen, "%04u", stepList[currStep]);
        stepFound = false;
        
        dirp = opendir("STEPS_XML/");    
        do {
            if((dp = readdir(dirp)) != NULL)
            {
                if((!strncmp(dp->d_name, stepFilename, 4)) &&
                        (!strncmp(&dp->d_name[strlen(dp->d_name)-4], ".xml", 4)))
                {
                    stepFound = true;
                    snprintf(stepFilename, strMaxLen, "STEPS_XML/%s",
                            dp->d_name);
                    break;
                }
            }
        } while (dp != NULL);
        closedir(dirp);
        
        if(!stepFound)
        {
            snprintf(msg, MAX_MSG_SIZE,
                    "Step %u (%u) not found", currStep, stepList[currStep]);
            status = -1;
            break;
        }
            
        /* Open XML Document */
        doc = xmlParseFile(stepFilename);
        if (doc == NULL)
        {
            snprintf(msg, MAX_MSG_SIZE, "Error while opening %s", stepFilename);
            status = -1;
            break;
        }
        /* get step (root) handle */
        root = xmlDocGetRootElement(doc);
        if (root == NULL)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            snprintf(msg, MAX_MSG_SIZE,
                     "Error while getting root handle of %s", stepFilename);
            status = -1;
            break;
        }
        status = ParseStep(doc, root, pGssStruct->getPointerToConfig(),
                pGssStruct->getPointerToProtocols(),
                pGssStruct->getPointerToPorts(), ppSteps, currStep, auxString,
                &levelError, pTestMgr->getPointerToParsingStatus(), 1, stepsNumber,
                NULL);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        
        if(pTestMgr->getParsingStatus() == CANCELED)
        {
            snprintf(msg, MAX_MSG_SIZE, "Test step load canceled");
        }
        if(status != 0) /* this status is from ParseStep */
        {
            DisplayParseStepError(status, 0, stepFilename, levelError, msg,
                    MAX_MSG_SIZE);
            pTestMgr->setParsingStatus(FINISHED);
        }
        if((pTestMgr->getParsingStatus() == CANCELED) || (status != 0))
        {
            break;
        }
        
        /* create valid waiting for previous step */
        stepStruct * pStep = pGssStruct->getPointerToStep(currStep);
        pStep->prevStepIdRef = prevStepIdRefSaved;
        pStep->outIdRefFromPrevStep = outIdRefFromPrevStepSaved;
        /* look for number of outputs of current XML file */
        /* it its stored in the begining of filename - after id */
        unsigned int outposPos = 0;
        //for(outposPos=stepsPathLen; outposPos<sizeof(stepFilename); ++outposPos)
        for(outposPos=10; outposPos<sizeof(stepFilename); ++outposPos)
        {
            if(stepFilename[outposPos] == '_')
            {
                break;
            }
        }
        status = sscanf(&stepFilename[outposPos+1], "%u",
                &outIdRefFromPrevStepSaved);
        if((status != 1) && (status != 2))
        {
            snprintf(msg, MAX_MSG_SIZE,
                    "Error while getting previous step for Step %d (%d)",
                    currStep, stepList[currStep]);
            status = -1;
            break;
        }
        status = 0;

        if(outIdRefFromPrevStepSaved == 0)
        {
            outIdRefFromPrevStepSaved = -1;
            prevStepIdRefSaved = -1;
        }
        else
        {
            prevStepIdRefSaved = currStep;
        }
        pStep->mode = stepStruct::CONTINUOUS;

        /* create valid next step with outputs for previous step */
        for(int port=0; port<MAX_INTERFACES; ++port)
        {
            if(pStep->numberOfOutputsAtPort[port] != 0)
            {
                pGssStruct->setNextStepWithoutOutputs(lastStepWithOutputsAtPort[port], port, currStep);
                lastStepWithOutputsAtPort[port] = currStep;
            }
        }
    }
    delete [] stepList;
    if(status != 0)
    {
        pTestMgr->setParsingStatus(FINISHED);
        pGssStruct->freeSteps();        
        pGssStruct->setNumberOfSteps(0);
        pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, true, false, false);
        emit setStatusBar(tr(msg));
        emit setEnabledControls(true, true, false, true);
        return;
    }
    
    snprintf(msg, MAX_MSG_SIZE, "Test step list %s loaded", filename);
    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, true, false, false);
    emit setStatusBar(tr(msg));
    emit setTpLoaded(filename);
    pTestMgr->setParsingStatus(FINISHED);
    LaunchTP();
}