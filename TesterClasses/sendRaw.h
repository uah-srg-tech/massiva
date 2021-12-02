/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   sendRaw.h
 * Author: Aaron
 *
 * Created on 18 de julio de 2017, 10:35
 */

#include <QtCore>
#include "../Forms/selectRawFileForm.h"
#include "../CommonClasses/gssStructs.h"
#include "../CommonClasses/Logs.h"
#include "../GuiClasses/MonitorPlots.h"
#include "../GuiClasses/RxTxInfo.h"

#ifndef SENDRAW_H
#define SENDRAW_H

class sendRaw : public QObject
{
    Q_OBJECT
    
public:
    sendRaw(gssStructs * origGssStruct, Logs * origLogs, TestManager * origTestMgr, 
            mainForm * origMainGu, MonitorPlots * origMonitors, 
            selectRawFileForm * origSelectRawFileForm, unsigned int origPort,
            char * origFilename, bool origLoop, unsigned int origIntervalMs,
            bool origDIC, bool origGlobalVars);

signals:
    void setStatusBarColor(const QString &, int r, int g, int b);
    void TxDataInc(unsigned int tab);
    void TxDataPrintPacket(unsigned int tab, const unsigned char * packet, int length,
            unsigned int portPhyHeaderOffset);
    void setSendRawMenuEnable(unsigned int cPort, bool newState);
    void setEnabledControls(bool, bool, bool, bool);
    void showPopUp(const QString &title, const QString &text,
            mainForm::PopUpType type);
    
public slots:
    void send();
    
private:
    static const int strMaxLen = InitialConfig::strMaxLen;
    
    gssStructs * pGssStruct;
    Logs * pLogs;
    TestManager * pTestMgr;
    mainForm * pMainGui;
    RxTxInfo * pTabs;
    MonitorPlots * pMonitors;
    
    selectRawFileForm * pSelectRawFileForm;
    unsigned int port;
    char commandFilename[strMaxLen];
    bool loop;
    unsigned int intervalMs;
    bool DIC;
    bool GlobalVars;
};

#endif /* SENDRAW_H */

