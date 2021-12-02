/**
 * \file	MonitorPlots.h
 * \brief	functions for timing in the interface (declaration)
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
#ifndef MONITOR_PLOTS_H
#define MONITOR_PLOTS_H

#include <ctime>                        /* clock_t */
#include "../CommonClasses/gssStructs.h"
#include "../CommonClasses/Logs.h"

#ifdef NO_QT
class MonitorPlots
{
public:
    int ConfigMonitors(gssStructs * origGssStruct, Logs * origLogs);
#else
#include "../Forms/mainForm.h"
#include "../GuiClasses/SpecialPackets.h"
#ifdef PLOTS
#include "../Forms/plotForm.h"
#endif

class MonitorPlots : public QObject
{
    Q_OBJECT
    
public:
    int ConfigMonitors(gssStructs * origGssStruct, Logs * origLogs,
            mainForm * origMainGui, SpecialPackets * origSpecials);
#endif /* NO_QT */

    void UnconfigMonitors(void);
    int CheckApplyMonitors(typeOfGlobalVarRef type, unsigned char * packet,
            unsigned int ifRef, int specialIndex, char * msg,
            unsigned int msgSize);
    void ApplyMonitor(monitor * pMonitor, int index);

#ifndef NO_QT
signals:
    void ShowAlarmMessageBox(const QString);
    void setStatusBarColor(const QString, int r, int g, int b);
    void SetAlarmVal(int idx, int value);
    
public slots:
#endif
    void showPlotsDialog(void);
        
private:
    gssStructs * pGssStruct;
    Logs * pLogs;
#ifdef PLOTS
    plotForm * pPlots;
    double initTimeS;
#endif
};


#endif /* MONITOR_PLOTS_H */