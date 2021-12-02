/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   selectRawFileForm.h
 * Author: Aaron
 *
 * Created on 13 de julio de 2017, 15:55
 */

#ifndef _SELECTRAWFILEFORM_H
#define _SELECTRAWFILEFORM_H

#include "ui_selectRawFileForm.h"
#include <QAction>
#include "mainForm.h"
#include "../CommonClasses/InitialConfig.h"
#include "../CommonClasses/Logs.h"
#include "../GuiClasses/RxTxInfo.h"
#include "../GuiClasses/MonitorPlots.h"
#include "../Forms/SpWTimeCodesDialog.h"

class selectRawFileForm : public QDialog
{
    Q_OBJECT
    
public:
    selectRawFileForm(gssStructs * origGssStruct, Logs * origLogs,
            InitialConfig * origInitialConfig, TestManager * origTestMgr,
            mainForm * origMainGui, RxTxInfo * origTxRxTabs,
            MonitorPlots * origMonitors);
    void createSendRawMenu();
    void destroySendRawMenu();
    
    void modifyRawPeriodicalTC(unsigned char value);

signals:
    void setPeriodicalSpWTCText(const QString &text);
    void setActionsAtPort(unsigned int port, bool status);
    
public slots:
    void setSendRawMenuEnable(unsigned int selPort, bool newState);
    
private:
    static const unsigned int strMaxLen = InitialConfig::strMaxLen;
    
    Ui::selectRawFileForm widget;
    QAction ** portAction;
    
    gssStructs * pGssStruct;
    Logs * pLogs;
    InitialConfig * pInitialConfig;
    TestManager * pTestMgr;
    mainForm * pMainGui;
    MonitorPlots * pMonitors;
    RxTxInfo * pTxRxTabs;
    
    unsigned int port;
    char commandFilename[strMaxLen];
    bool loop;
    unsigned int intervalMs;
    bool DIC;
    bool GlobalVars;
    
private slots:
    void selectRawMenu();
    void selectFile();
    void processSendRaw();
    void updateIntervalMs(int);
    void updateLoop(int);
    void updateDIC(int);
    void updateGlobalVars(int);
};

#endif /* _SELECTRAWFILEFORM_H */