/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: Aaron
 *
 * Created on 22 de mayo de 2017, 13:39
 */
#pragma GCC diagnostic ignored "-Wunused-result"

#include <cstdio>       /* snprintf*/
#include <cstdlib>      /* srand, memset */
#include <unistd.h>     /* chdir */
#include <pthread.h>
#include <QApplication>
#include <QMessageBox>
#include <QThread>
#include "XMLParsingTools/XMLEgseConfig.h"
#include "Forms/selectRawFileForm.h"
#include "Forms/selectProtocolPacket.h"
#include "Forms/mainForm.h"
#include "Forms/PeriodicTCsDialog.h"
#include "Forms/SpWTimeCodesDialog.h"
#include "CommonClasses/gssStructs.h"
#include "CommonClasses/Logs.h"
#include "CommonClasses/InitialConfig.h"
#include "CommonTools/crc16.h"
#include "GuiClasses/RxTxInfo.h"
#include "GuiClasses/SpecialPackets.h"
#include "GuiClasses/MonitorPlots.h"
#include "PortTools/configScenario.h"
#include "PortTools/raw.h"
#include "TesterClasses/TestManager.h"
#include "GuiClasses/TestButtons.h"
#include "TesterClasses/EnqueueWorker.h"
#include "TesterClasses/PeriodicMonitor.h"
#include "TesterClasses/AutoTester.h"

#define GSS_VERSION     "MASSIVA 4.1.0.0"
#define GSS_TITLE_BAR   "MASSIVA"

using namespace std;

static int Init (int argc, char *argv[], pthread_t * pRs232_rx_thread,
        gssStructs * pGssStruct, Logs * pLogs, InitialConfig * pInitialConfig,
        TestManager * pTestMgr, mainForm * pMainGui, RxTxInfo * pRxTxTabs,
        resetGlobalVariables * pRgv, SpecialPackets * pSpecials,
        MonitorPlots * pMonitorPlots, selectRawFileForm * pSendRawGui,
        selectProtocolPacket * pSendPPGui, TestButtons * pTestButtons,
        QThread ** rxThread, QThread ** periodicMonThread,
        PeriodicMonitor ** periodicMonWorker, PeriodicTCsDialog ** ppPTCsDialog,
        SpWTimeCodesDialog ** ppSpWTCDialog, bool firstTimeOpened);
static void Finish(pthread_t * pRs232_rx_thread, gssStructs * pGssStruct,
        Logs * pLogs, InitialConfig * pInitialConfig, 
        mainForm * pMainGui, RxTxInfo * pTabs, SpecialPackets * pSpecials, 
        MonitorPlots * pMonitorPlots, selectRawFileForm * pSendRawGui,
        selectProtocolPacket * pSendPPGui, QThread ** rxThread,
        QThread ** periodicMonThread, PeriodicMonitor ** periodicMonWorker,
        PeriodicTCsDialog * pPTCsDialog, SpWTimeCodesDialog * pSpWTCDialog);
static int MessageBoxErrorIni(const char * title, const char * text,
        bool showGssButton, mainForm * pMainGui,
        InitialConfig * pInitialConfig);
static int checkConfigParameter(int argc, char * argv[], int argc_off,
        char * auxMsg, unsigned int strMaxLen);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    pthread_t rs232_rx_thread;
    gssStructs GssStruct;
    InitialConfig initConf;
    Logs logs(&initConf);
    TestManager testMgr(&GssStruct, &logs, &initConf);
    resetGlobalVariables rgv(&GssStruct, &logs);
    mainForm mainGui(&GssStruct, &logs, &initConf, &testMgr, &rgv, GSS_VERSION); 
    RxTxInfo rxTxTabs;
    SpecialPackets specials;
    MonitorPlots monitorPlots;
    selectRawFileForm sendRawGui(&GssStruct, &logs, &initConf, &testMgr,
            &mainGui, &rxTxTabs, &monitorPlots);
    selectProtocolPacket sendPPGui(&GssStruct, &logs, &testMgr, &mainGui,
            &rxTxTabs, &specials, &monitorPlots);
    TestButtons testButtons(&GssStruct, &logs, &initConf, &testMgr, &mainGui,
            &rxTxTabs, &specials, &monitorPlots);
            
    QThread * rxThread[MAX_INTERFACES];
    QThread * periodicMonThread[MAX_MONITORINGS];
    PeriodicMonitor * periodicMonWorker[MAX_MONITORINGS];
    PeriodicTCsDialog * pPTCsDialog = NULL;
    SpWTimeCodesDialog * pSpWTCDialog = NULL;
    bool firstTimeOpened = true;
    int status = 0;
    
    do {
        if((status = Init(argc, argv, &rs232_rx_thread, &GssStruct, &logs,
                &initConf, &testMgr, &mainGui, &rxTxTabs, &rgv, &specials,
                &monitorPlots, &sendRawGui, &sendPPGui, &testButtons, rxThread,
                periodicMonThread, periodicMonWorker, &pPTCsDialog,
                &pSpWTCDialog, firstTimeOpened)) != 0)
        {
            if(status > 0)
                continue;
            else
                return -1;
        }
        mainGui.setEnabledControls(true, true, false, true);
        
        if(GssStruct.getAutoTest() == true)
        {
            initConf.SetCloseGss(true);
            QThread * testAutoThread = new QThread();
            AutoTester * tester = new AutoTester(&GssStruct, &testButtons);
            tester->moveToThread(testAutoThread);
            QObject::connect(testAutoThread, SIGNAL(started()), tester, SLOT(startAutoTest()));
            QObject::connect(testAutoThread, SIGNAL(finished()), tester, SLOT(deleteLater()));
            testAutoThread->start();
        }
        mainGui.show();
        app.exec();
        
        Finish(&rs232_rx_thread, &GssStruct, &logs, &initConf,
                &mainGui, &rxTxTabs, &specials, &monitorPlots, &sendRawGui,
                &sendPPGui, rxThread, periodicMonThread, periodicMonWorker,
                pPTCsDialog, pSpWTCDialog);
        if(chdir(initConf.GetFile(GSS)) != 0)
            break;
        firstTimeOpened = false;
    }while(!initConf.GetCloseGss());
    return 0;
}

static int Init (int argc, char *argv[], pthread_t * pRs232_rx_thread,
        gssStructs * pGssStruct, Logs * pLogs, InitialConfig * pInitialConfig,
        TestManager * pTestMgr, mainForm * pMainGui, RxTxInfo * pRxTxTabs,
        resetGlobalVariables * pRgv, SpecialPackets * pSpecials,
        MonitorPlots * pMonitorPlots, selectRawFileForm * pSendRawGui,
        selectProtocolPacket * pSendPPGui, TestButtons * pTestButtons,
        QThread ** rxThread, QThread ** periodicMonThread,
        PeriodicMonitor ** periodicMonWorker, PeriodicTCsDialog ** ppPTCsDialog,
        SpWTimeCodesDialog ** ppSpWTCDialog, bool firstTimeOpened)
{
    int status = 0;
    unsigned int idx = 0, numberOfPorts = 0;
    static const unsigned int strMaxLen = InitialConfig::strMaxLen;
    char auxMsg[strMaxLen];
    int spw_tc_defined = -1;
    
    serialConfig serials[MAX_INTERFACES];
    memset(serials, 0, sizeof(serialConfig)*MAX_INTERFACES);
    
    init_CRC_word16_table();
    srand(time(NULL));
    
    memset(auxMsg, 0, strMaxLen);
    char * auto_test_name = NULL;
    
    if(argc >= 2)
    {
        if((strncmp(argv[1], "--config=", 9) == 0) ||
                (strncmp(argv[1], "-c", 2) == 0))
        {
            if(checkConfigParameter(argc, argv, 1, auxMsg, strMaxLen) != 0)
            {
                return MessageBoxErrorIni("Error parsing option", 
                        "Wrong --config: Missing config file",
                        false, pMainGui, pInitialConfig);
            }
        }
        else if(strncmp(argv[1], "--test=", 7) == 0)
        {
            if(strlen(argv[1]) <= 7)
            {
                return MessageBoxErrorIni("Error parsing option", 
                        "Wrong --test: Missing test name",
                        false, pMainGui, pInitialConfig);
            }
            auto_test_name = &argv[1][7];
            pGssStruct->setAutoTest(true);
            if(checkConfigParameter(argc, argv, 2, auxMsg, strMaxLen) != 0)
            {
                return MessageBoxErrorIni("Error parsing option", 
                        "Wrong --config: Missing config file",
                        false, pMainGui, pInitialConfig);
            }
        }
        else if(strncmp(argv[1], "-t", 2) == 0)
        {
            if(argc == 2)
            {
                return MessageBoxErrorIni("Error parsing option", 
                        "Wrong --test: Missing test name",
                        false, pMainGui, pInitialConfig);
            }
            auto_test_name = &argv[2][0];
            pGssStruct->setAutoTest(true);
            if(checkConfigParameter(argc, argv, 3, auxMsg, strMaxLen) != 0)
            {
                return MessageBoxErrorIni("Error parsing option", 
                        "Wrong --config: Missing config file",
                        false, pMainGui, pInitialConfig);
            }
        }
        else if((strncmp(argv[1], "--test-all", 10) == 0) ||
                (strncmp(argv[1], "-a", 2) == 0))
        {
            pGssStruct->setAutoTest(true);
            if(checkConfigParameter(argc, argv, 2, auxMsg, strMaxLen) != 0)
            {
                return MessageBoxErrorIni("Error parsing option", 
                        "Wrong --config: Missing config file",
                        false, pMainGui, pInitialConfig);
            }
        }
        else
        {
            status = snprintf(auxMsg, strMaxLen,
                    "Wrong option %s", argv[1]);
            for(idx=2; idx<(unsigned int)argc; ++idx)
            {
                status += snprintf(&auxMsg[status], strMaxLen-status, 
                        " %s", argv[idx]);
            }
            return MessageBoxErrorIni("Error parsing option", auxMsg,
                    false, pMainGui, pInitialConfig);
        }
    }
    /* first of all, try if there is a writable folder */
    if((status = pInitialConfig->CheckGssFolderWritable(auxMsg, strMaxLen)) != 0)
    {
        return MessageBoxErrorIni("Error in writable folder", auxMsg, false,
            pMainGui, pInitialConfig);
    }

    /* if any, in auxMsg should be the XML configuration file */
    if(auxMsg[0] == 0)
    {
        /* no XML configuration file as parameter 1) find INI */
        if((status = pInitialConfig->FindIni(auxMsg, strMaxLen)) != 0)
        {
            return MessageBoxErrorIni("Error finding INI file", auxMsg, false,
                                      pMainGui, pInitialConfig);
        }
        /* no XML configuration file as parameter 2) parse INI */
        if((status = pInitialConfig->ConfigWorkspaceParseIni(auxMsg,
                strMaxLen)) != 0)
        {
            //if any error rises it will be at configIni
            return MessageBoxErrorIni("Error parsing INI file", auxMsg, false,
                                      pMainGui, pInitialConfig);
        }
    }
    else
    {
        /* XML configuration file as parameter:
         * set workspace in current folder if possible, in home either */
        if((status = pInitialConfig->ConfigWorkspaceCommandOption(auxMsg,
                strMaxLen)) != 0)
        {
            //if any error rises it will be at configIni
            return MessageBoxErrorIni("Error in options", auxMsg, false,
                    pMainGui, pInitialConfig);
        }
    }
    
    /* parse gss_config file working directory */
    if(chdir(pInitialConfig->GetFile(WORKSPACE)) != 0)
    {
        snprintf(auxMsg, strMaxLen,
                "Error going to workspace directory \"%s\"",
                pInitialConfig->GetFile(WORKSPACE));
        return MessageBoxErrorIni("Error changing directory", auxMsg, false,
                pMainGui, pInitialConfig);
    }
    status = EgseConfig(pInitialConfig->GetFile(XML_CONFIG_FILE),
            pGssStruct->getPointerToConfig(),
            pGssStruct->getPointerToPorts(), serials,
            pGssStruct->getPointerToOptions(),
            pGssStruct->getPointerToProtocols());
    if(chdir(pInitialConfig->GetFile(GSS)) != 0)
    {
        snprintf(auxMsg, strMaxLen,
                "Error selecting MASSIVA working directory \"%s\"",
                pInitialConfig->GetFile(GSS));
        return MessageBoxErrorIni("Error changing directory", auxMsg, false,
                pMainGui, pInitialConfig);
    }
    if(status != 0)
    {
        char errorTitleMsg[strMaxLen];
        snprintf(errorTitleMsg, strMaxLen, "Error while parsing configuration files: ");
        status = DisplayEgseConfigError(status, pGssStruct->getPointerToPorts(),
                auxMsg, strMaxLen);
        if(status)
        {
            return MessageBoxErrorIni(errorTitleMsg, auxMsg, false, pMainGui,
                    pInitialConfig);
        }
        else
        {
            return MessageBoxErrorIni(errorTitleMsg, auxMsg, true, pMainGui,
                    pInitialConfig);
        }
    }
    numberOfPorts = pGssStruct->getNumberOfPorts();
    
    /* create logs */
    pLogs->setPrintPhyHeaderOptions(pGssStruct->getPrintPhyHeaderOptions());
    
#ifdef _WIN32
    char slashChar = '\\';
#else
    char slashChar = '/';
#endif
    const char * portNames[MAX_INTERFACES];
    for(idx=0; idx<numberOfPorts; ++idx)
    {
        portNames[idx] = pGssStruct->getPortName(idx);
    }
    if((status = pLogs->openCreateLogs(numberOfPorts, portNames)) < 0)
    {
        if(status == -MAIN_LOG_IDX)
            snprintf(auxMsg, strMaxLen, "Can not create main log file ");
        else if(status == -RAW_LOG_IDX)
            snprintf(auxMsg, strMaxLen, "Can not create raw log file ");
        else
            snprintf(auxMsg, strMaxLen, "Can not create %d log file ", -status);
        QMessageBox::information(pMainGui, "Error", auxMsg);
        return -1;
    }
    portConfig * pPorts = pGssStruct->getPointerToPorts();
    
    /* configure Scenario */
    for(idx=0; idx<numberOfPorts; ++idx)
    {
        if((status = ConfigPort(idx, pPorts, &serials[idx])) < 0)
        {
            DisplayConfigPortError(status, &pPorts[idx],
                    auxMsg, strMaxLen);
            
            /* unconfigure ports already configured */
            for(unsigned int ports=0; ports<idx; ++ports)
            {
                UnconfigPort(ports, pGssStruct->getPointerToPort(ports));
            }
            UnconfigServer();
            char errMsg[MAX_MSG_SIZE];
            snprintf(errMsg, MAX_MSG_SIZE,
                    "%s configuration error. Config file \"%s%c%s\"",
                    GSS_VERSION, pInitialConfig->GetFile(WORKSPACE),
                    slashChar, pInitialConfig->GetFile(XML_CONFIG_FILE));
            pLogs->SetTimeInAllLogs(numberOfPorts, errMsg,
                    false, false, true);
            
            /* needed to close logs to force writing */
            pLogs->closeOpenLogs(false);
            
            snprintf(errMsg, MAX_MSG_SIZE,
                    "Configuration error. Config file \"%s%c%s\"",
                    pInitialConfig->GetFile(WORKSPACE),
                    slashChar, pInitialConfig->GetFile(XML_CONFIG_FILE));
            return MessageBoxErrorIni(errMsg, auxMsg, true, pMainGui,
                    pInitialConfig);
        }
    }
    
    /* start processing at serial idxs which can receive */
    bool protocolThread = false;
    for(idx=0; idx<numberOfPorts; ++idx)
    {
        if((protocolThread == false) && (pGssStruct->getPortIoType(idx) != IN_PORT)
                && ((pGssStruct->getPortType(idx) == UART_PORT) ||
                (pGssStruct->getPortType(idx) == TCP_SOCKET_SRV_PORT) ||
                (pGssStruct->getPortType(idx) == TCP_SOCKET_CLI_PORT)))
        {
            /* create a second thread which executes inc_x(&x) */
            if((status = pthread_create(pRs232_rx_thread, NULL,
                    prepareReadProtocolPacketsBuffer,
                    pGssStruct->getPointerToPorts())) != 0)
            {
                snprintf(auxMsg, strMaxLen, "pthread_create error %d",
                        status);
                QMessageBox::information(pMainGui,
                        "Error creating port protocol thread", auxMsg);
                return -1;
            }
            protocolThread = true;
        }
        else if((spw_tc_defined == -1) && 
                (pGssStruct->getPortType(idx) == SPW_TC_PORT))
        {
            spw_tc_defined = idx;
        }
    }

#if defined(NOT_MK2_DEV) && defined(NOT_USB_DEV) && defined(NOT_PCI_DEV)
    pMainGui->mainContent.actionSend_single_SpW_TC->setVisible(false);
    pMainGui->mainContent.actionEnable_periodical_SpW_TC->setVisible(false);
#else
    pMainGui->mainContent.actionSend_single_SpW_TC->setVisible(true);
    pMainGui->mainContent.actionEnable_periodical_SpW_TC->setVisible(true);
    if(spw_tc_defined == -1)
    {
        pMainGui->mainContent.actionSend_single_SpW_TC->setEnabled(false);
        pMainGui->mainContent.actionEnable_periodical_SpW_TC->setEnabled(false);
    }
    else
    {
        //create and connect SpW TimeCodes actions
        *ppSpWTCDialog = new SpWTimeCodesDialog(pGssStruct, pLogs, pTestMgr,
                pMainGui, spw_tc_defined);
        QObject::connect(pMainGui->mainContent.actionSend_single_SpW_TC, SIGNAL(triggered()),
                *ppSpWTCDialog, SLOT(ShowSendSingleSpWTCDialog()));
        pMainGui->mainContent.actionSend_single_SpW_TC->setEnabled(true);
        QObject::connect(pMainGui->mainContent.actionEnable_periodical_SpW_TC, SIGNAL(triggered()),
                *ppSpWTCDialog, SLOT(ShowEnableDisableSpWTCDialog()));  
        pMainGui->mainContent.actionEnable_periodical_SpW_TC->setEnabled(true);      
    }
#endif

    /* configure Rx/Tx info tabs */
    pRxTxTabs->ConfigTabs(pGssStruct, pMainGui, pLogs);

    /* configure Special Packets tabs */
    pSpecials->ConfigSpecialPackets(pGssStruct, pMainGui);

    /* configure monitors */
    if((status = pMonitorPlots->ConfigMonitors(pGssStruct, pLogs, pMainGui,
            pSpecials)) < 0)//returns number of plots
    {
        QMessageBox::information(pMainGui, "Error in monitors",
                "Can't configure monitors");
        return -1;
    }

#ifdef PLOTS
    /* configure Plot tabs */
    if(status == 0) //from monitors
    {
        pMainGui->mainContent.actionPlots->setEnabled(false);
    }
    else
    {
        QObject::connect(pMainGui->mainContent.actionPlots, SIGNAL(triggered()),
                pMonitorPlots, SLOT(showPlotsDialog()));
        pMainGui->mainContent.actionPlots->setEnabled(true);
    }
#else
    pMainGui->mainContent.actionPlots->setEnabled(false);
#endif
    
    /* print GSS config version and info */
    if(firstTimeOpened)
    {
        snprintf(auxMsg, strMaxLen, "%s opened. Config file \"%s%c%s\"",
                GSS_VERSION, pInitialConfig->GetFile(WORKSPACE),
                slashChar, pInitialConfig->GetFile(XML_CONFIG_FILE));
    }
    else
    {
        snprintf(auxMsg, strMaxLen, "%s resetted. Config file \"%s%c%s\"",
                GSS_VERSION, pInitialConfig->GetFile(WORKSPACE),
                slashChar, pInitialConfig->GetFile(XML_CONFIG_FILE));
    }
    pLogs->SetTitleInAllLogs(numberOfPorts, auxMsg,
            pGssStruct->getPointerToOptions());

    /* listen in each port configured */
    for(idx=0; idx<numberOfPorts; ++idx)
    {
        status = snprintf(auxMsg, strMaxLen, "Using %d as %s port",
                idx, pGssStruct->getPortName(idx));
        if(pGssStruct->getUartName(idx) != NULL)
        {
            status += snprintf(&auxMsg[status], strMaxLen-status,
                    " via \"%s\"", pGssStruct->getUartName(idx));
        }
        if(strlen(pGssStruct->getPortDeviceInfo(idx)) != 0)
        {
            snprintf(&auxMsg[status], strMaxLen-status,
                    " (%s)", pGssStruct->getPortDeviceInfo(idx));
        }
        pLogs->saveMsgToLog(idx, NULL, auxMsg);
        if(pGssStruct->getPortIoType(idx) != IN_PORT)
        {
            rxThread[idx] = new QThread();
            EnqueueWorker * rxWorker = new EnqueueWorker(pGssStruct, pLogs,
                    pTestMgr, pMainGui, pRxTxTabs, pMonitorPlots, pSpecials,
                    pTestButtons, idx);
            rxWorker->moveToThread(rxThread[idx]);
            QObject::connect(rxThread[idx], SIGNAL(started()), rxWorker,
                    SLOT(EnqueuePackets()));
            QObject::connect(rxWorker, SIGNAL(finished()), rxThread[idx],
                    SLOT(quit()), Qt::DirectConnection);
            QObject::connect(rxThread[idx], SIGNAL(finished()), rxThread[idx],
                    SLOT(deleteLater()));
            rxThread[idx]->start();
        }
        else
        {
            rxThread[idx] = NULL;
        }
    }
    
    if(pGssStruct->getNumberOfGlobalVars() == 0)
    {
        pMainGui->setResetGlobalVarsStatus(false);
    }
    else
    {
        pMainGui->setResetGlobalVarsStatus(true);
        pRgv->setNumberOfGlobalVars();
    }
    
    if(pGssStruct->getNumberOfMonitors())
    {
        /* monitor all periodic global variables */
        for(idx=0; idx<pGssStruct->getNumberOfMonitors(); ++idx)
        {
            if(pGssStruct->getGlobalVarType(idx) == GVR_PERIODIC)
            {
                periodicMonThread[idx] = new QThread();
                periodicMonWorker[idx] = new PeriodicMonitor(pGssStruct,
                        pMonitorPlots, idx);
                periodicMonWorker[idx]->moveToThread(periodicMonThread[idx]);
                QObject::connect(periodicMonThread[idx], SIGNAL(started()),
                        periodicMonWorker[idx], SLOT(PerformPeriodicMonitor()));
                QObject::connect(periodicMonWorker[idx], SIGNAL(finished()),
                        periodicMonThread[idx], SLOT(quit()), Qt::DirectConnection);
                QObject::connect(periodicMonThread[idx], SIGNAL(finished()),
                        periodicMonThread[idx], SLOT(deleteLater()));
                periodicMonThread[idx]->start();
            }
            else
            {
                periodicMonThread[idx] = NULL;
                periodicMonWorker[idx] = NULL;
            }
        }
    }
    
    if(pGssStruct->getNumberOfPeriodicTCs() == 0)
    {
        pMainGui->mainContent.actionPeriodic_TCs->setEnabled(false);
    }
    else
    {
        //connect PeriodicTC action
        *ppPTCsDialog = new PeriodicTCsDialog(pGssStruct, pMainGui, pRxTxTabs,
                pMonitorPlots, pLogs);
        QObject::connect(pMainGui->mainContent.actionPeriodic_TCs, SIGNAL(triggered()),
                *ppPTCsDialog, SLOT(showPeriodicTCsDialog()));
        pMainGui->mainContent.actionPeriodic_TCs->setEnabled(true);
        QObject::connect(pMainGui, SIGNAL(setEnabledActionAtPort(unsigned int, bool)),
                *ppPTCsDialog, SLOT(setEnabledActionAtPort(unsigned int, bool)));
    }
    
    /* load procedures in interface */
    pMainGui->createProceduresList();
    
    pGssStruct->setCurrentTest(-1);
    if((pGssStruct->getAutoTest()) && (auto_test_name != NULL))//single auto test
    {
        //look for auto test name in list
        unsigned int idx = 0;
        for(idx=0; idx<pGssStruct->getNumberOfProcedures(); ++idx)
        {
            if(strncmp(pGssStruct->getProcedureName(idx), auto_test_name,
                    strlen(pGssStruct->getProcedureName(idx))) == 0)
            {
                pGssStruct->setCurrentTest(idx);
                break;
            }
        }
        if(idx == pGssStruct->getNumberOfProcedures())
        {
            idx = snprintf(auxMsg, strMaxLen,
                    "Wrong command line option -test/-t: %s not found", argv[2]);
            if(argc > 3)
                snprintf(&auxMsg[idx], strMaxLen-idx,
                        " in XML configured in \"%s\" file",
                        pInitialConfig->GetFile(INI_CONFIG_FILE));
            QMessageBox::information(pMainGui, "Error parsing command line",
                    auxMsg);
            return -1;
        }
    }
    /* set window title - without MASSIVA version */
    status = snprintf(auxMsg, strMaxLen, "%s", GSS_TITLE_BAR);
    if(pGssStruct->getAutoTest())
    {
        if(pGssStruct->getCurrentTest() == -1)
            snprintf(&auxMsg[status], strMaxLen-status, " [Auto Test : ALL]");
        else
            snprintf(&auxMsg[status], strMaxLen-status, " [Auto Test : %s]",
                    pGssStruct->getProcedureName(pGssStruct->getCurrentTest()));
    }
    pMainGui->setWindowTitle(auxMsg);
    
    /* create raw and protocol packet menus */
    pSendRawGui->createSendRawMenu();
    pSendPPGui->createProtocolPacketMenu();
    pInitialConfig->SetLastWorkingInfo();
    return 0;
}

static void Finish(pthread_t * pRs232_rx_thread, gssStructs * pGssStruct,
        Logs * pLogs, InitialConfig * pInitialConfig, 
        mainForm * pMainGui, RxTxInfo * pTabs, SpecialPackets * pSpecials, 
        MonitorPlots * pMonitorPlots, selectRawFileForm * pSendRawGui,
        selectProtocolPacket * pSendPPGui, QThread ** rxThread,
        QThread ** periodicMonThread, PeriodicMonitor ** periodicMonWorker,
        PeriodicTCsDialog * pPTCsDialog, SpWTimeCodesDialog * pSpWTCDialog)
{
    unsigned int idx = 0, numberOfPorts = pGssStruct->getNumberOfPorts();
    int status = 0;
    char auxMsg[MAX_MSG_SIZE];
    int spw_tc_defined = -1;
    
    pGssStruct->freeSteps();
    
    //hide bars
    pMainGui->setVisibleTxStep(false);
    pMainGui->setVisibleInput(false);
    pMainGui->setVisibleRxStep(false);
    pMainGui->setVisibleOutput(false);
    
    if(pGssStruct->getNumberOfMonitors())
    {
        /* stop periodic monitoring */
        for(idx=0; idx<pGssStruct->getNumberOfMonitors(); ++idx)
        {
            if(pGssStruct->getGlobalVarType(idx) == GVR_PERIODIC)
            {
                periodicMonWorker[idx]->setPeriodicMonitorEnabled(false);
                periodicMonThread[idx]->wait(5000);
                delete periodicMonWorker[idx];
            }
        }
    }
    
    /* stop periodic TCs */
    if((pGssStruct->getNumberOfPeriodicTCs()) && (pPTCsDialog != NULL))
    {
        QObject::disconnect(pMainGui->mainContent.actionPeriodic_TCs, SIGNAL(triggered()),
                pPTCsDialog, SLOT(showPeriodicTCsDialog()));
        delete pPTCsDialog;
    }
    
    /* destroy Packets menu options */
    pSendRawGui->destroySendRawMenu();
    pSendPPGui->destroyProtocolPacketMenu();
    
    /* unconfigure Special Packets tabs */
    if(pGssStruct->getNumberOfSpecialPackets())
        pSpecials->UnconfigSpecialPackets();
    
    /* unconfigure monitors */
    pMonitorPlots->UnconfigMonitors();

#ifdef PLOTS
    /* unconfigure Plot tabs */
    QObject::disconnect(pMainGui->mainContent.actionPlots, SIGNAL(triggered()),
            pMonitorPlots, SLOT(showPlotsDialog()));
#endif
    
    /* unconfigure Rx/Tx info tabs */
    pTabs->UnconfigTabs();
    
    /* unconfigure Scenario */
    for(idx=0; idx<numberOfPorts; ++idx)
    {
        if((status = UnconfigPort(idx, pGssStruct->getPointerToPort(idx))) < 0)
        {
            char auxMsg[MAX_MSG_SIZE];
            DisplayUnconfigPortError(status, pGssStruct->getPointerToPort(idx),
                    auxMsg, MAX_MSG_SIZE);
            QMessageBox::information(pMainGui, "Error unconfiguring MASSIVA", auxMsg);
            break;
        }
    }
    if((status = UnconfigServer()) < 0)
    {
        char auxMsg[MAX_MSG_SIZE];
        DisplayUnconfigPortError(status, pGssStruct->getPointerToPort(idx),
                auxMsg, MAX_MSG_SIZE);
        QMessageBox::information(pMainGui, "Error unconfiguring MASSIVA", auxMsg);
    }
    
    /* stop UART polling */
    /* only can be done after closing UART ports */
    for(idx=0; idx<numberOfPorts; ++idx)
    {
        if((pGssStruct->getPortType(idx) == UART_PORT) &&
                (pGssStruct->getPortIoType(idx) != IN_PORT))
        {
            if((status = pthread_join(*pRs232_rx_thread, NULL)) != 0)
            {
                char auxMsg[MAX_MSG_SIZE];
                snprintf(auxMsg, MAX_MSG_SIZE, "pthread_join error %d", status);
                QMessageBox::information(pMainGui, "Error joining RS232 thread",
                        auxMsg);
            }
            break;
        }
        else if((spw_tc_defined == -1) && 
                (pGssStruct->getPortType(idx) == SPW_TC_PORT))
        {
            spw_tc_defined = idx;
        }
    }
    
    /* stop threads listening in each port configured */
    for(idx=0; idx<numberOfPorts; ++idx)
    {
        if(pGssStruct->getPortIoType(idx) != IN_PORT)
        {
            rxThread[idx]->wait(5000);
        }
    }
    UnconfigEgseConfig(pGssStruct->getPointerToConfig());

#if !defined(NOT_MK2_DEV) || !defined(NOT_USB_DEV) || !defined(NOT_PCI_DEV)
    if(spw_tc_defined != -1)
    {
        QObject::disconnect(pMainGui->mainContent.actionSend_single_SpW_TC, SIGNAL(triggered()),
                pSpWTCDialog, SLOT(ShowSendSingleSpWTCDialog()));
        QObject::disconnect(pMainGui->mainContent.actionEnable_periodical_SpW_TC, SIGNAL(triggered()),
                pSpWTCDialog, SLOT(ShowEnableDisableSpWTCDialog()));
        delete pSpWTCDialog;
    }
#endif

    /* print GSS info */
    if(pInitialConfig->GetCloseGss())
    {
        snprintf(auxMsg, MAX_MSG_SIZE, "%s closed", GSS_VERSION);
    }
    else
    {
        snprintf(auxMsg, MAX_MSG_SIZE, "%s to be reset", GSS_VERSION);
    }
    pLogs->SetTimeInAllLogs(numberOfPorts, auxMsg, false, false, true);
    pLogs->countPrintPacketSummary(pGssStruct->getPointerToPorts(),
            numberOfPorts, pGssStruct->getPointerToSpecialInfos(),
            pGssStruct->getNumberOfSpecialPackets(),
            pGssStruct->getAllNotExpectedPackets());
    pLogs->closeOpenLogs(false);
    return;
}

static int MessageBoxErrorIni(const char * title, const char * text,
        bool showGssButton, mainForm * pMainGui,
        InitialConfig * pInitialConfig)
{
    int ret = 1;
    QPushButton *editGssFileButton = NULL, * restoreConfigButton = NULL;
    QMessageBox msgBox;
    msgBox.setWindowTitle(GSS_TITLE_BAR);
    msgBox.setText(title);
    msgBox.setInformativeText(text);
    QPushButton * switchButton = msgBox.addButton("Switch MASSIVA config file",
            QMessageBox::ActionRole);
    if(pInitialConfig->GetLastWorkingValid())
    {
        restoreConfigButton =
                msgBox.addButton("Try to restore last working MASSIVA file",
                QMessageBox::ActionRole);
    }
    if(showGssButton)
    {
        editGssFileButton = msgBox.addButton("Edit current MASSIVA config file",
                QMessageBox::ActionRole);
    }
    QPushButton * closeButton = msgBox.addButton("Close",
            QMessageBox::ActionRole);
    msgBox.setDefaultButton(switchButton);
    msgBox.setEscapeButton(closeButton);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/nuts.ico"), QSize(), QIcon::Normal,
            QIcon::Off);
    msgBox.setWindowIcon(icon);
    msgBox.exec();
    
    if(msgBox.clickedButton() == switchButton)
    {
        pMainGui->SwitchConfigGSS();
    }
    else if(msgBox.clickedButton() == closeButton)
    {
        ret = -1;
    }
    else if((pInitialConfig->GetLastWorkingValid()) &&
            (msgBox.clickedButton() == restoreConfigButton))
    {
        pInitialConfig->RestoreConfigGSS();
    }
    else if((showGssButton) && (msgBox.clickedButton() == editGssFileButton))
    {
        pInitialConfig->ShowConfigFile(false);
        QMessageBox::information(pMainGui, "Info",
                "When editing finished click OK to continue");
        chdir(pInitialConfig->GetFile(GSS));
    }
    return ret;
}

static int checkConfigParameter(int argc, char * argv[], int argc_off,
        char * auxMsg, unsigned int strMaxLen)
{
    if(argc > argc_off)
    {
        if(strncmp(argv[argc_off], "--config=", 9) == 0)
        {
            if(strlen(argv[argc_off]) <= 9)
            {
                return -1;
            }
            /* get user-defined config file */
            unsigned int idx = strlen(&argv[argc_off][9]);
            if(idx > strMaxLen)
                idx = strMaxLen;
            memcpy(auxMsg, &argv[argc_off][9], idx);
        }
        else if(strncmp(argv[argc_off], "-c", 2) == 0)
        {
            if(argc == (argc_off+1))
            {
                return -1;
            }
            /* get user-defined config file */
            unsigned int idx = strlen(argv[argc_off+1]);
            if(idx > strMaxLen)
                idx = strMaxLen;
            memcpy(auxMsg, argv[argc_off+1], idx);
        }
    }
    return 0;
}
