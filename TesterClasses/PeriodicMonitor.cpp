/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PeriodicMonitor.cpp
 * Author: Aaron
 * 
 * Created on 26 de septiembre de 2017, 13:15
 */

#include "PeriodicMonitor.h"
#include <time.h>				/* clock_gettime */

PeriodicMonitor::PeriodicMonitor(gssStructs * origGssStruct,
        MonitorPlots * origMonitors, unsigned int idRef)
{
    pGssStruct = origGssStruct;
    pMonitors = origMonitors;
    id = idRef;
    periodicMonitorEnabled = true;
}

void PeriodicMonitor::setPeriodicMonitorEnabled(bool state)
{
    periodicMonitorEnabled = state;
    if(state == false)
        sleepWaitCondition.wakeAll();
}

void PeriodicMonitor::PerformPeriodicMonitor()
{
    monitor * pMonitor = pGssStruct->getPointerToMonitor(id);
    QMutex mutex;
    timespec markIni, markEnd;
    double elapsedMs = 0.0;
    
    do {
        clock_gettime(CLOCK_MONOTONIC, &markIni);
        pMonitors->ApplyMonitor(pMonitor, id);
        
        if(!periodicMonitorEnabled)
            break;
        clock_gettime(CLOCK_MONOTONIC, &markEnd);
        elapsedMs = (markEnd.tv_sec - markIni.tv_sec) * 1000.0;
        elapsedMs += (markEnd.tv_nsec - markIni.tv_nsec) / 1000000.0;
        if(elapsedMs < pMonitor->monitorRef.periodInMs)
        {
            /* controlled sleep if needed */
            mutex.lock();
            sleepWaitCondition.wait(&mutex,
                    pMonitor->monitorRef.periodInMs - elapsedMs);
            mutex.unlock();
        }
    } while(periodicMonitorEnabled);
    emit finished();
}