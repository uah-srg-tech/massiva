/**
 * \file	EnqueueWorker.h
 * \brief	functions for send and receive packets
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/01/2014 at 15:30:23
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef ENQUEUEWORKER_H
#define ENQUEUEWORKER_H

#include <QtCore>
#include "../Forms/mainForm.h"
#include "../TesterClasses/TestManager.h"
#include "../GuiClasses/TestButtons.h"
#include "../GuiClasses//MonitorPlots.h"
#include "../GuiClasses/SpecialPackets.h"
#include "../GuiClasses/RxTxInfo.h"

class EnqueueWorker : public QObject
{
Q_OBJECT;

public:
    EnqueueWorker(gssStructs * origGssStruct, Logs * origLogs,
            TestManager * origTestMgr, mainForm * pMainGui, RxTxInfo * origTabs,
            MonitorPlots * origMonitors, SpecialPackets * pSpecials,
            TestButtons * origTestButtons, unsigned int port);

public slots:
    void EnqueuePackets();

signals:
    void finished();

    void setStatusBar(const QString &);
    void setStatusBarColor(const QString &, int r, int g, int b);
    void setRxOutput(unsigned int currentStepNumber,
            unsigned int numberOfOutputs, unsigned int outputsReceived);
    
    void IncreaseDisplaySpecialValue(unsigned int idx);
    void SetEnabledSpecialPeriod(unsigned int idx, int state);
    void DisplaySpecialPeriod(unsigned int idx, double value, QColor color);
    void DisplaySpecialField(unsigned int idx, unsigned int fld, int value);
    void DisplaySpecialField(unsigned int idx, unsigned int fld, double value);
    
    void RxDataInc(unsigned int tab);
    void RxDataPrintPacket(unsigned int tab, const unsigned char * packet,
            int length, unsigned int portPhyHeaderOffset);
    
    void RequestTestFinish(testEndStatus status, unsigned int step,
            unsigned int input);
    
private:
    gssStructs * pGssStruct;
    Logs * pLogs;
    TestManager * pTestMgr;
    MonitorPlots * pMonitors;
    unsigned int rxPort;
    
    void ShowSpecialFields(int idx, levelOut * spLevels,
            int levelRef, int firstFld, const unsigned char * packet);
};

#endif /* ndef ENQUEUEWORKER_H */
