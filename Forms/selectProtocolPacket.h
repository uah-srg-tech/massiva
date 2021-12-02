/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   selectProtocolPacket.h
 * Author: Aaron
 *
 * Created on 27 de julio de 2017, 18:30
 */

#ifndef _SELECTPROTOCOLPACKET_H
#define _SELECTPROTOCOLPACKET_H

#include "ui_selectProtocolPacket.h"
#include "ui_selectProtocolPacket.h"
#include "../CommonClasses/Logs.h"
#include "../GuiClasses/HexSpinBox.h"
#include "../GuiClasses/RxTxInfo.h"
#include "../GuiClasses/MonitorPlots.h"
#include "mainForm.h"
#include <QAction>

class selectProtocolPacket : public QDialog
{
    Q_OBJECT
public:
    selectProtocolPacket(gssStructs * origGssStruct, Logs * origLogs,
            TestManager * origTestMgr, mainForm * origMainGui, RxTxInfo * origTxRxTabs,
            SpecialPackets * origSpecials, MonitorPlots * origMonitors);
    void createProtocolPacketMenu();
    void destroyProtocolPacketMenu();
    
private:
    Ui::selectProtocolPacket widget;
    gssStructs * pGssStruct;
    Logs * pLogs;
    TestManager * pTestMgr;
    mainForm * pMainGui;
    SpecialPackets * pSpecials;
    MonitorPlots * pMonitors;
    RxTxInfo * pTxRxTabs;
    
    QAction ** ppAction;
    unsigned int pps;
    protocolPacket * pProtPacket;
    input * pStepIn;
    unsigned int numberOfSpinBoxes;
    HexSpinBox ** hexSpinBoxes;
    QLabel ** labels;
    unsigned int * sbFieldMap;
    unsigned int * sbIndexMap;
    bool * isFirstCopy;
    bool extraInfoField;
        
    static const unsigned int MAX_COPIES=8;
    void sendProtocolPacket();
    void clearSpinBoxes();
    
private slots:
    void processProtocolPacket();
    void selectProtocolPacketMenu();
    void updateArrayField(qulonglong newValue);
    void updateVariableField(qulonglong newValue);
};

#endif /* _SELECTPROTOCOLPACKET_H */
