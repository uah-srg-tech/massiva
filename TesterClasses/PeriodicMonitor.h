/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PeriodicMonitor.h
 * Author: Aaron
 *
 * Created on 26 de septiembre de 2017, 13:15
 */

#ifndef PERIODICMONITOR_H
#define PERIODICMONITOR_H

#include <QtCore>
#include "../CommonClasses/gssStructs.h"
#include "../GuiClasses/MonitorPlots.h"

class PeriodicMonitor : public QObject
{
    Q_OBJECT
    
public:
    PeriodicMonitor(gssStructs * origGssStruct, MonitorPlots * origMonitors,
            unsigned int idRef);
    void setPeriodicMonitorEnabled(bool state);

public slots:
    void PerformPeriodicMonitor();

signals:
    void finished();
    
private:
    unsigned int id;
    bool periodicMonitorEnabled;
    gssStructs * pGssStruct;
    MonitorPlots * pMonitors;
    QWaitCondition sleepWaitCondition;
};

#endif /* PERIODICMONITORS_H */

