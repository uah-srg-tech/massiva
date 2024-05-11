/**
 * \file	MonitorPlots.cpp
 * \brief	functions for timing in the interface (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/04/2013 at 14:18:34
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <cstdio>                               /* snprintf */
#include <cstring>                              /* strlen */
#include <ctime>                                /* clock_gettime */
#include "MonitorPlots.h"
#include "../CheckTools/CheckFilterTools.h"     /* CheckBoolVars, CheckFilters */

#ifdef NO_QT
int MonitorPlots::ConfigMonitors(gssStructs * origGssStruct, Logs * origLogs)
{
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    return 0;
}
#else
int MonitorPlots::ConfigMonitors(gssStructs * origGssStruct, Logs * origLogs,
            mainForm * origMainGui, SpecialPackets * origSpecials)
{
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    unsigned int numberOfPlots = 0;
    connect(this, SIGNAL (ShowAlarmMessageBox(QString)),
            origMainGui, SLOT (ShowAlarmMessageBox(QString)));
    connect(this, SIGNAL (setStatusBarColor(QString, int, int, int)),
            origMainGui, SLOT (setStatusBarColor(QString, int, int, int)));
    connect(this, SIGNAL (SetAlarmVal(int, int)),
            origSpecials, SLOT (SetAlarmValSlot(int, int)));

#ifdef PLOTS
    pPlots = new plotForm;
    numberOfPlots = pPlots->ConfigurePlots(pGssStruct->getPointerToCharts(),
            pGssStruct->getNumberOfCharts(), pGssStruct->getPointerToMonitors(),
            pGssStruct->getNumberOfMonitors());
    connect(origMainGui, SIGNAL (closePlotForm()),
            pPlots, SLOT (closePlotForm()));
    connect(pPlots, SIGNAL (replot(QwtPlot *)),
            origMainGui, SLOT (replot(QwtPlot *)));
    
    struct timespec ref;
    clock_gettime(CLOCK_MONOTONIC, &ref);
    initTimeS = ref.tv_sec + (ref.tv_nsec / 1000000000.0);
#endif
    return numberOfPlots;
}
#endif /* NO_QT */

void MonitorPlots::UnconfigMonitors(void)
{
#ifdef PLOTS
    pPlots->UnconfigPlots(pGssStruct->getPointerToCharts(),
            pGssStruct->getNumberOfCharts(),
            pGssStruct->getPointerToMonitors(),
            pGssStruct->getNumberOfMonitors());
    delete pPlots;
#endif
}

int MonitorPlots::CheckApplyMonitors (typeOfGlobalVarRef type,
        unsigned char * packet, unsigned int ifRef, int specialIndex,
        char * msg, unsigned int msgSize)
{
    unsigned int mnt = 0;
    int perform = 1, filterErrorAt = -1, errorType = -1;
    monitor * monitors = pGssStruct->getPointerToMonitors();
    globalVar * globalVars = pGssStruct->getPointerToGlobalVars();
    
    for(mnt=0; mnt<pGssStruct->getNumberOfMonitors(); ++mnt)
    {
        if(monitors[mnt].typeOfRef == GVR_PERIODIC)
            continue;//it is done in its own thread
        
        if(globalVars[monitors[mnt].globarVarRef].Interface != ifRef)
            continue;
        
        if((globalVars[monitors[mnt].globarVarRef].commandType == globalVar::GLOBAL_TM_SPECIAL) &&
                (globalVars[monitors[mnt].globarVarRef].idRef != specialIndex))
            continue;

        perform = 1;
        if(monitors[mnt].typeOfRef == type)
        {
            if((monitors[mnt].typeOfRef != GVR_PERIODIC) &&
                    (monitors[mnt].monitorRef.globalVarFilter.boolVars != NULL))
            {
                //GVFiltered filter != "DEFAULT"
                unsigned int bv = 0, fld = 0;
                uint64_t * numbers = new uint64_t[monitors[mnt].monitorRef.globalVarFilter.numberOfBoolVars];
                uint8_t ** strings = new unsigned char *[monitors[mnt].monitorRef.globalVarFilter.numberOfBoolVars];
                for(bv=0; bv<monitors[mnt].monitorRef.globalVarFilter.numberOfBoolVars; ++bv)
                {
                    strings[bv] = new uint8_t[MAX_STR_OPERAND_LEN];
                }
                for(fld=0; fld<monitors[mnt].monitorRef.globalVarFilter.numberOfTMFields; ++fld)
                {
                    if(monitors[mnt].monitorRef.globalVarFilter.TMFields[fld].type == VSFIELD)
                    {
                        level * defaultLevel =
                                pGssStruct->getPointerToLevel(globalVars[monitors[mnt].globarVarRef].Interface,
                                globalVars[monitors[mnt].globarVarRef].level);
                        monitors[mnt].monitorRef.globalVarFilter.TMFields[fld].totalSizeInBits =
                                defaultLevel->out.TMFields[fld].totalSizeInBits;
                        if((unsigned int)monitors[mnt].monitorRef.globalVarFilter.TMFields[fld].totalSizeInBits >
                                globalVars[monitors[mnt].globarVarRef].size * 8)
                        {
                            monitors[mnt].monitorRef.globalVarFilter.TMFields[fld].totalSizeInBits =
                                    globalVars[monitors[mnt].globarVarRef].size * 8;
                        }
                    }
                }

                /* CHECK monitor globalVarFilter FILTER */
                perform = CheckBoolVars(
                        &packet[monitors[mnt].monitorRef.globalVarFilter.importInBytes],
                        numbers, strings, monitors[mnt].monitorRef.globalVarFilter.boolVars,
                        monitors[mnt].monitorRef.globalVarFilter.numberOfBoolVars,
                        monitors[mnt].monitorRef.globalVarFilter.TMFields,
                        monitors[mnt].monitorRef.globalVarFilter.numberOfTMFields,
                        monitors[mnt].monitorRef.globalVarFilter.crcTMFieldRefs,
                        monitors[mnt].monitorRef.globalVarFilter.numberOfcrcTMFields);
                if(perform < 0)
                {
                    errorType = 0;
                    break;
                }

                perform = CheckFilters(monitors[mnt].monitorRef.globalVarFilter.filters,
                        monitors[mnt].monitorRef.globalVarFilter.typeOfFilters,
                        monitors[mnt].monitorRef.globalVarFilter.numberOfFilters,
                        monitors[mnt].monitorRef.globalVarFilter.boolVars, &filterErrorAt);		
                if(perform < 0)
                {
                    errorType = 1;
                    break;
                }
                for(bv=0; bv<monitors[mnt].monitorRef.globalVarFilter.numberOfBoolVars; ++bv)
                {
                    if(strings[bv] != NULL)
                    {
                        delete(strings[bv]);
                        strings[bv] = NULL;
                    }
                }
                if(numbers != NULL)
                {
                    delete(numbers);
                    numbers = NULL;
                }
                if(strings != NULL)
                {
                    delete(strings);
                    strings = NULL;
                }
            }
            if(perform)
            {
                ApplyMonitor(&monitors[mnt], mnt);
            }
        }
    }
    if(perform < 0)
    {
        char source[2][10] = {"BoolVar\0", "Filter\0"};
        snprintf(msg, msgSize, "%s error in Monitor %d: ", source[errorType], mnt);
        ParseCheckBoolVarError(perform, &msg[strlen(msg)], msgSize-strlen(msg));
        pLogs->saveMsgToLog(MAIN_LOG_IDX, "\t", msg);
    }
    return perform;
}

void MonitorPlots::ApplyMonitor(monitor * pMonitor, int index)
{
    switch(pMonitor->type)
    {
        case monitor::PLOT:
        {
#ifdef PLOTS
            struct timespec ref;
            double currentTimeS = 0.0;
            clock_gettime(CLOCK_MONOTONIC, &ref);
            currentTimeS = ref.tv_sec + (ref.tv_nsec / 1000000000.0);
            pPlots->Plot(index, currentTimeS - initTimeS,
                    pGssStruct->getGlobalVarValue(pMonitor->globarVarRef));
#endif
            break;
        }

        case monitor::ALARM_MSG:
        {
#ifndef NO_QT
            switch(pMonitor->dataType.alarmType)
            {
                case ALARM1:
                    emit ShowAlarmMessageBox(QString(pMonitor->data.msg));
                    break;

                case ALARM2:
                    emit setStatusBarColor(QString(pMonitor->data.msg),
                            240, 240, 240);
                    break;

                case ALARM3:
                    printf("%s\n", pMonitor->data.msg);
                    break;
            }
#endif
            pLogs->SetTimeInLog(MAIN_LOG_IDX, pMonitor->data.msg, true);
            break;
        }

        case monitor::MODIFY:
        {
            switch(pMonitor->dataType.modifyType)
            {
                case INCREMENT:
                    pGssStruct->incrementGlobalVarValue(pMonitor->globarVarRef,
                            pMonitor->data.value);
                    break;

                case INCREMENT_1_WRAP:
                    pGssStruct->incrementGlobalVarValue(pMonitor->globarVarRef,
                            1);
                    if(pGssStruct->getGlobalVarValue(pMonitor->globarVarRef) == pMonitor->data.value)
                    {
                        pGssStruct->resetGlobalVarValue(pMonitor->globarVarRef);
                    }
                    break;
            }
            break;
        }

        case monitor::ALARM_VAL:
        {
#ifndef NO_QT
            switch(pMonitor->dataType.alarmType)
            {
                case ALARM1:
                    emit SetAlarmVal(pMonitor->data.controlIdx,
                            (int)pGssStruct->getGlobalVarValue(pMonitor->globarVarRef));
                    break;

                default:
                    break;
            }
#endif
            break;
        }
            
        default:
            break;
    }
    return;
}

void MonitorPlots::showPlotsDialog(void)
{
#ifdef PLOTS
    if(pPlots->isVisible())
        pPlots->hide();
    pPlots->show();
#endif
}
