/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PeriodicTC.h
 * Author: Aaron
 *
 * Created on 26 de septiembre de 2017, 16:54
 */

#ifndef _PERIODICTC_H
#define _PERIODICTC_H

#include <QLabel>
#include <QSlider>
#include <QLCDNumber>
#include <QWaitCondition>
#include "../Forms/mainForm.h"
#include "../CommonClasses/gssStructs.h"
#include "../GuiClasses/MonitorPlots.h"

class PeriodicTC : public QObject
{
    Q_OBJECT
public:
    PeriodicTC(gssStructs * origGssStruct, MonitorPlots * origMonitors, 
            Logs * origLogs, unsigned int idRef);
    void setPeriodicEnabled(bool state);

public slots:
    void performPeriodicTC();
    
signals:
    void setCheckEnabled(unsigned int idx, bool state);
    void updatePeriod(unsigned int idx, double lastPeriodS, bool start);
    void setStatusBarColor(const QString &, int r, int g, int b);
    void TxDataInc(unsigned int tab);
    void finished();

private:
    gssStructs * pGssStruct;
    MonitorPlots * pMonitors;
    Logs * pLogs;

    unsigned int id;
    unsigned int value;
    bool periodicEnabled;
    QWaitCondition sleepWaitCondition;
};

#endif /* _PERIODICTCFORM_H */