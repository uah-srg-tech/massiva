/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PeriodicTCsDialog.h
 * Author: Aaron
 *
 * Created on 27 de septiembre de 2017, 12:24
 */

#ifndef _PERIODICTCSDIALOG_H
#define _PERIODICTCSDIALOG_H

#include "ui_PeriodicTCsDialog.h"
#include "../CommonClasses/gssStructs.h"
#include "../CommonClasses/Logs.h"
#include "../GuiClasses/RxTxInfo.h"
#include "../TesterClasses/PeriodicTC.h"
#include "mainForm.h"
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLCDNumber>

class PeriodicTCsDialog : public QDialog
{
    Q_OBJECT
public:
    PeriodicTCsDialog(gssStructs * origGssStruct, mainForm * origMainGui, 
            RxTxInfo * origTxRxTabs, MonitorPlots * origMonitors,
            Logs * origLogs);
    ~PeriodicTCsDialog();

public slots:
    void showPeriodicTCsDialog();
    void setEnabledPeriodicTC(int newState);
    void setPeriodPeriodicTC(double newValue);
    void setCheckEnabled(unsigned int idx, bool state);
    void updatePeriod(unsigned int idx, double lastPeriodS, bool start);
    void closePTCsDialog();
    void setEnabledActionAtPort(unsigned int port, bool status);
    
private:
    Ui::PeriodicTCsDialog widget;
    gssStructs * pGssStruct;
    mainForm * pMainGui;
    RxTxInfo * pTxRxTabs;
    MonitorPlots * pMonitors;
    Logs * pLogs;
    
    unsigned int ptcs;
    PeriodicTC ** periodicTCWorker;
    QThread ** periodicTCThread;
    QLabel ** name;
    QCheckBox ** selectEnabled;
    QDoubleSpinBox ** selectPeriod;
    QLCDNumber ** counter, ** period;
};

#endif /* _PERIODICTCSDIALOG_H */