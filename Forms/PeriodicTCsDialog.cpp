/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PeriodicTCsDialog.cpp
 * Author: Aaron
 *
 * Created on 27 de septiembre de 2017, 12:24
 */

#include <QThread>
#include "PeriodicTCsDialog.h"
#include "../TesterClasses/PeriodicTC.h"

PeriodicTCsDialog::PeriodicTCsDialog(gssStructs * origGssStruct,
        mainForm * origMainGui, RxTxInfo * origTxRxTabs,
        MonitorPlots * origMonitors, Logs * origLogs)
{
    widget.setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    
    pGssStruct = origGssStruct;
    pMainGui = origMainGui;
    pTxRxTabs = origTxRxTabs;
    pMonitors = origMonitors;
    pLogs = origLogs;
    ptcs = origGssStruct->getNumberOfPeriodicTCs();
    
    periodicTCWorker = new PeriodicTC * [ptcs];
    periodicTCThread = new QThread * [ptcs];
    name = new QLabel * [ptcs];
    selectEnabled = new QCheckBox * [ptcs];
    selectPeriod = new QDoubleSpinBox * [ptcs];
    counter = new QLCDNumber * [ptcs];
    period = new QLCDNumber * [ptcs];
    
    for(unsigned int idx=0; idx<ptcs; ++idx)
    {
        periodicTCWorker[idx] = NULL;
        name[idx] = new QLabel(tr(pGssStruct->getPeriodicTCName(idx)), this);
        name[idx]->setObjectName(QString::fromUtf8("name")+QString::number(idx));
        name[idx]->setMinimumSize(QSize(101, 21));
        widget.gridLayout->addWidget(name[idx], idx, 0);
        selectEnabled[idx] = new QCheckBox(this);
        selectEnabled[idx]->setObjectName(QString::fromUtf8("selectEnabled"));
        selectEnabled[idx]->setMinimumSize(QSize(16, 21));
        widget.gridLayout->addWidget(selectEnabled[idx], idx, 1);
        selectPeriod[idx] = new QDoubleSpinBox(this);
        selectPeriod[idx]->setObjectName(QString::fromUtf8("selectPeriod"));
        selectPeriod[idx]->setMinimumSize(QSize(51, 21));
        selectPeriod[idx]->setValue(pGssStruct->getPeriodicTCPeriodMs(idx)/1000.0);
        selectPeriod[idx]->setDecimals(3);
        selectPeriod[idx]->setMinimum(0.5);
        selectPeriod[idx]->setSingleStep(0.5);
        widget.gridLayout->addWidget(selectPeriod[idx], idx, 2);
        counter[idx] = new QLCDNumber(this);
        counter[idx]->setObjectName(QString::fromUtf8("counter"));
        counter[idx]->setMinimumSize(QSize(51, 21));
        counter[idx]->setFrameShadow(QFrame::Plain);
        counter[idx]->setSegmentStyle(QLCDNumber::Flat);
        counter[idx]->display(0);
        widget.gridLayout->addWidget(counter[idx], idx, 3);
        period[idx] = new QLCDNumber(this);
        period[idx]->setObjectName(QString::fromUtf8("period"));
        period[idx]->setMinimumSize(QSize(51, 21));
        period[idx]->setFrameShadow(QFrame::Plain);
        period[idx]->setSegmentStyle(QLCDNumber::Flat);
        period[idx]->display(0.0);
        widget.gridLayout->addWidget(period[idx], idx, 4);
        
        connect(this->selectEnabled[idx], SIGNAL(stateChanged(int)),
                this, SLOT(setEnabledPeriodicTC(int)));
        connect(this->selectPeriod[idx], SIGNAL(valueChanged(double)),
                this, SLOT(setPeriodPeriodicTC(double)));
    }
    connect(pMainGui, SIGNAL (closePTCsDialog()),
            this, SLOT (closePTCsDialog()));
}

void PeriodicTCsDialog::showPeriodicTCsDialog()
{
    if(this->isVisible())
        this->hide();
    this->show();
}

void PeriodicTCsDialog::setEnabledPeriodicTC(int newState)
{
    unsigned int idx = 0;
    QCheckBox * origin = qobject_cast<QCheckBox *>(sender());
    for(idx=0; idx<ptcs; ++idx)
    {
        if(origin == selectEnabled[idx])
        {
            break;
        }
    }
    if(newState == Qt::Checked)
    {
        periodicTCWorker[idx] = new PeriodicTC(pGssStruct, pMonitors, pLogs, idx);
        periodicTCThread[idx] = new QThread();
        periodicTCWorker[idx]->moveToThread(periodicTCThread[idx]);
        connect(periodicTCThread[idx], SIGNAL(started()),
                periodicTCWorker[idx], SLOT(performPeriodicTC()));
        connect(periodicTCWorker[idx], SIGNAL(finished()),
                periodicTCThread[idx], SLOT(quit()), Qt::DirectConnection);
        connect(periodicTCThread[idx], SIGNAL(finished()),
                periodicTCThread[idx], SLOT(deleteLater()));
        
        connect(periodicTCWorker[idx], SIGNAL(setCheckEnabled(unsigned int, bool)),
                this, SLOT(setCheckEnabled(unsigned int, bool)));
        connect(periodicTCWorker[idx], SIGNAL(updatePeriod(unsigned int, double, bool)),
                this, SLOT(updatePeriod(unsigned int, double, bool)));
        connect(periodicTCWorker[idx], SIGNAL(setStatusBarColor(const QString &, int, int, int)),
                pMainGui, SLOT (setStatusBarColor(const QString &, int, int, int)));
        connect(periodicTCWorker[idx], SIGNAL(TxDataInc (unsigned int)), 
                pTxRxTabs, SLOT(TxDataInc (unsigned int)));
        periodicTCThread[idx]->start();
    }
    else
    {
        periodicTCWorker[idx]->setPeriodicEnabled(false);
        periodicTCThread[idx]->wait(1000);
        delete periodicTCWorker[idx];
        periodicTCWorker[idx] = NULL;
    }
}

void PeriodicTCsDialog::setPeriodPeriodicTC(double newValue)
{
    unsigned int idx = 0;
    QDoubleSpinBox * origin = qobject_cast<QDoubleSpinBox *>(sender());
    for(idx=0; idx<ptcs; ++idx)
    {
        if(origin == selectPeriod[idx])
        {
            break;
        }
    }
    pGssStruct->setPeriodicTCPeriodMs(idx, (unsigned int)(newValue*1000));
}

PeriodicTCsDialog::~PeriodicTCsDialog()
{
    this->hide();
    
    for(unsigned int idx=0; idx<ptcs; ++idx)
    {
        if(periodicTCWorker[idx] != NULL)
        {
            periodicTCWorker[idx]->setPeriodicEnabled(false);
            periodicTCThread[idx]->wait(1000);
        }
        delete name[idx];
        delete selectEnabled[idx];
        delete selectPeriod[idx];
        delete counter[idx];
        delete period[idx];
        delete periodicTCWorker[idx];
    }
    delete [] periodicTCWorker;
    delete [] periodicTCThread;
    delete [] name;
    delete [] selectEnabled;
    delete [] selectPeriod;
    delete [] counter;
    delete [] period;
}

void PeriodicTCsDialog::setCheckEnabled(unsigned int idx, bool state)
{
    selectEnabled[idx]->setChecked(state);
}

void PeriodicTCsDialog::updatePeriod(unsigned int idx, double lastPeriodS, bool start)
{
    int newCounter = 0;
    if(!start)
        newCounter = counter[idx]->intValue()+1;
    counter[idx]->display(newCounter);
    period[idx]->display(round(lastPeriodS * 1000.0) / 1000.0);
}

void PeriodicTCsDialog::closePTCsDialog()
{
    this->hide();
}

void PeriodicTCsDialog::setEnabledActionAtPort(unsigned int port, bool status)
{
    for(unsigned int idx=0; idx<ptcs; ++idx)
    {
        if(pGssStruct->getPeriodicTCPort(idx) == port)
        {
            if(status == false)
            {
                selectEnabled[idx]->setCheckState(Qt::Unchecked);
            }
            selectEnabled[idx]->setEnabled(status);
        }
    }
}