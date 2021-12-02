/**
 * \file	SpecialPackets.cpp
 * \brief	functions for show info about struct special packets (definition)            
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		16/06/2017 at 14:46:35
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2017, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */ 
#include <cstdio>                           /* snprintf */
#include <ctime>                            /* clock */
#include <QThread>
#include <QScrollArea>
#include <QFormLayout>
#include "SpecialPackets.h"

void SpecialPackets::ConfigSpecialPackets(gssStructs * origGssStruct,
        mainForm * origMainGui)
{
    tabs = 0;
    pGssStruct = origGssStruct;
    pMainGui = origMainGui;
    
    CreateSpecialArrays();
    special_packet_info * infos = pGssStruct->getPointerToSpecialInfos();
    monitor * monitors = pGssStruct->getPointerToMonitors();
    unsigned int currentAlarmVal = 0;

    for(unsigned int idx=0; idx<pGssStruct->getNumberOfSpecialPackets(); ++idx)
    {
        specialName[idx]->setText((QString(infos[idx].name)));
        specialName[idx]->show();
        specialCount[idx]->show();
        specialEnabled[idx]->show();
        specialPrint[idx]->show();
        if(infos[idx].type == special_packet_info::SPECIAL_PERIODIC)
            specialPeriodCount[idx]->show();
        ChangeSpecialEnabled (idx, pGssStruct->getSpecialInfoEnabled(idx));
        specialEnabled[idx]->setChecked(pGssStruct->getSpecialInfoEnabled(idx));
        specialPrint[idx]->setChecked(pGssStruct->getSpecialInfoPrint(idx));
        
        connect(specialEnabled[idx], SIGNAL(stateChanged(int)), this,
                SLOT(ChangeSpecialEnabledSlot(int)));
        connect(specialPrint[idx], SIGNAL(stateChanged(int)), this,
                SLOT(ChangeSpecialPrintSlot(int)));

        if(infos[idx].struct_show != -1)
        {
            tabs++;
        }
    }
    if(tabs)
    {
        ConfigSpecialTabs(infos, pGssStruct->getPointerToSpecialPackets());
    }
    else
    {
        pMainGui->mainContent.specialFieldsTab->hide();
    }
    
    CreateAlarmVals();
    for(unsigned int idx=0; idx<pGssStruct->getNumberOfMonitors(); ++idx)
    {
        if(monitors[idx].type == monitor::ALARM_VAL)
        {
            alarmValName[currentAlarmVal]->setText((QString(monitors[idx].name)));
            alarmValName[currentAlarmVal]->show();
            alarmVal[currentAlarmVal]->show();
            alarmVal[currentAlarmVal]->display(0);
            currentAlarmVal++;
        }
    }
    ResetSpecialPackets();
    connect(pMainGui->statusBarButton, SIGNAL(doubleClicked()),
            this, SLOT(ResetSpecialPackets()));

    specialCheckThread = new QThread();
    specialCheckWorker = new CheckSpecialPeriods(pGssStruct);
    specialCheckWorker->moveToThread(specialCheckThread);
    connect (specialCheckWorker, SIGNAL (DisplaySpecialPeriod(unsigned int, double, QColor)),
            this, SLOT (DisplaySpecialPeriodSlot(unsigned int, double, QColor)));
    connect(specialCheckThread, SIGNAL(started()), specialCheckWorker, SLOT(CheckSpecialPeriodsSlot()));
    connect(specialCheckWorker, SIGNAL(finished()), specialCheckThread, SLOT(quit()), Qt::DirectConnection);
    connect(specialCheckThread, SIGNAL(finished()), specialCheckThread, SLOT(deleteLater()));
    specialCheckThread->start();
    return;
}

void SpecialPackets::UnconfigSpecialPackets()
{
    specialCheckWorker->SetEnabled(false);
    specialCheckThread->wait(5000);
 
    for(unsigned int idx=0; idx<pGssStruct->getNumberOfSpecialPackets(); ++idx)
    {
        specialName[idx]->setText("");
        specialName[idx]->hide();
        specialCount[idx]->hide();
        specialEnabled[idx]->hide();
        specialPrint[idx]->hide();
        specialPeriodCount[idx]->hide();
        ChangeSpecialEnabled (idx, pGssStruct->getSpecialInfoEnabled(idx));
        
        disconnect(specialEnabled[idx], SIGNAL(stateChanged(int)), this,
                SLOT(ChangeSpecialEnabledSlot(int)));
        disconnect(specialPrint[idx], SIGNAL(stateChanged(int)), this,
                SLOT(ChangeSpecialPrintSlot(int)));
    }

    if(tabs)
    {
        special_packet_info * infos = pGssStruct->getPointerToSpecialInfos();
        for(unsigned int idx=0; idx<pGssStruct->getNumberOfSpecialPackets(); ++idx)
        {
            if(infos[idx].struct_show == -1)
                continue;
            
            delete [] specialField[idx];
            delete specialTab[idx];
            pMainGui->mainContent.specialFieldsTab->removeTab(idx);
        }
        delete [] specialTab;
        specialTab = NULL;
        tabs = 0;
    }
}

void SpecialPackets::ToggleSpecial(const spec_in_step *special)
{
    switch(special->mode)
    {
        case spec_in_step::ENABLE:
            specialEnabled[special->id]->setChecked(1);
            ChangeSpecialEnabled(special->id, 1);
            break;

        case spec_in_step::DISABLE:
            specialEnabled[special->id]->setChecked(0);
            ChangeSpecialEnabled(special->id, 0);
            break;

        case spec_in_step::ENABLE_PRINT:
            specialPrint[special->id]->setChecked(1);
            pGssStruct->setSpecialInfoPrint(special->id, 1);
            break;

        case spec_in_step::DISABLE_PRINT:
            specialPrint[special->id]->setChecked(0);
            pGssStruct->setSpecialInfoPrint(special->id, 0);
            break;
    }
    return;
}

void SpecialPackets::ResetSpecialPackets(void)
{
    special_packet_info * specialInfo = pGssStruct->getPointerToSpecialInfos();
    for(unsigned int idx=0; idx<pGssStruct->getNumberOfSpecialPackets(); ++idx)
    {
        specialInfo[idx].counter = 0;
        specialInfo[idx].lastTimeMs = 0.0;
        specialInfo[idx].intervalTimeMs = 0.0;
        specialInfo[idx].period_enabled = 0;
        
        QPalette pal = specialCount[idx]->palette();
        pal.setColor(QPalette::WindowText, Qt::black);
        specialCount[idx]->setPalette(pal);
        specialCount[idx]->display(0);
        DisplaySpecialPeriodSlot(idx, 0, Qt::black);
        specialPeriodCount[idx]->display(0.0);
        specialPeriodCount[idx]->setEnabled(false);
    }
    return;
}

void SpecialPackets::ChangeSpecialEnabledSlot(int state)
{
    QCheckBox * origin = qobject_cast<QCheckBox *>(sender());
    for(unsigned int idx=0; idx<MAX_SPECIAL_PACKETS; ++idx)
    {
        if(origin == specialEnabled[idx])
        {
            ChangeSpecialEnabled(idx, state);
            break;
        }
    }
}

void SpecialPackets::ChangeSpecialPrintSlot (int state)
{
    QCheckBox * origin = qobject_cast<QCheckBox *>(sender());
    for(unsigned int idx=0; idx<MAX_SPECIAL_PACKETS; ++idx)
    {
        if(origin == specialPrint[idx])
        {
            pGssStruct->setSpecialInfoPrint(idx, state);
            break;
        }
    }
}

int SpecialPackets::ConfigSpecialTabs(special_packet_info * infos,
        output * spPackets)
{
    unsigned int idx=0;
    memset(fields, 0, sizeof(unsigned int)*MAX_SPECIAL_PACKETS);
        
    specialTab = new QWidget*[pGssStruct->getNumberOfSpecialPackets()];
    
    for(idx=0; idx<pGssStruct->getNumberOfSpecialPackets(); ++idx)
    {
        /* first remove example TAB from UI (can't be removed in Qt Designer */
        if(idx == 0)
            pMainGui->mainContent.specialFieldsTab->removeTab(0);
        
        if(infos[idx].struct_show == -1)
            continue;

        fields[idx] =
                spPackets[idx].level[infos[idx].struct_show].numberOfTMFields-infos[idx].struct_show_field;
        specialField[idx] = new QLCDNumber*[fields[idx]];
           
        QString idxStr = QString::number(idx);
        specialTab[idx] = new QWidget();
        specialTab[idx]->setObjectName(QString("special" + idxStr));
        QGridLayout * gridLayout = new QGridLayout(specialTab[idx]);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout")); 
        
        QScrollArea * specialScrollArea = new QScrollArea(specialTab[idx]);
        specialScrollArea->setObjectName(QString("special" + idxStr + "scrollArea"));
        specialScrollArea->setGeometry(QRect(0,0, 206, 491));
        specialScrollArea->setFrameShape(QFrame::NoFrame);
        specialScrollArea->setWidgetResizable(true);
        gridLayout->addWidget(specialScrollArea, 0, 0, 1, 1);
        QWidget *specialScrollContents = new QWidget(specialScrollArea);
        specialScrollContents->setObjectName(QString("special" + idxStr + "scrollContents"));
        specialScrollContents->setGeometry(QRect(0, 0, 206, 501));

        QFormLayout *specialLayout = new QFormLayout(specialScrollArea);
        specialLayout->setObjectName(QString("special" + idxStr + "layout"));
        specialLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        specialLayout->setContentsMargins(0, 0, 0, 0);
        
        for(unsigned int fld=0; fld<fields[idx]; ++fld)
        {
            QString fldStr = QString::number(fld);
            specialField[idx][fld] = new QLCDNumber(specialScrollArea);
            specialField[idx][fld]->setObjectName(QString("special" + idxStr + "field" + fldStr));
            specialField[idx][fld]->setFrameShadow(QFrame::Plain);
            specialField[idx][fld]->setSmallDecimalPoint(true);
            specialField[idx][fld]->setDigitCount(8);
            if(spPackets[idx].level[infos[idx].struct_show].TMFields[fld+infos[idx].struct_show_field].type == CSFORMULAFIELD)
                specialField[idx][fld]->setMode(QLCDNumber::Dec);
            else
                specialField[idx][fld]->setMode(QLCDNumber::Hex);
            specialField[idx][fld]->setSegmentStyle(QLCDNumber::Flat);
            specialField[idx][fld]->display(0);
            specialField[idx][fld]->setFixedSize(61, 21);
            QLabel *label = new QLabel(QString(spPackets[idx].level[infos[idx].struct_show].TMFields[fld+infos[idx].struct_show_field].descr),
                    specialScrollArea);
            label->setFixedSize(111, 21);
            specialLayout->addRow(label, specialField[idx][fld]);  
        }
        specialScrollContents->setLayout(specialLayout);
        specialScrollArea->setWidget(specialScrollContents);
        pMainGui->mainContent.specialFieldsTab->addTab(specialTab[idx], QString(infos[idx].name));
    }
    pMainGui->mainContent.specialFieldsTab->show();
    return 0;
}

void SpecialPackets::IncreaseDisplaySpecialValueSlot(unsigned int idx)
{
    if(specialCount[idx]->intValue() == 999999)
    {
        QPalette pal = specialCount[idx]->palette();
        pal.setColor(QPalette::WindowText, Qt::darkRed);
        specialCount[idx]->setPalette(pal);
        specialCount[idx]->display(0);
    }
    else
    {
        specialCount[idx]->display(specialCount[idx]->intValue() + 1);
    }
}

void SpecialPackets::SetEnabledSpecialPeriodSlot(unsigned int idx, int state)
{
    specialPeriodCount[idx]->setEnabled(state);
}

void SpecialPackets::DisplaySpecialPeriodSlot(unsigned int idx, double value, QColor color)
{
    specialPeriodCount[idx]->display(value);
    QPalette pal = specialPeriodCount[idx]->palette();
    pal.setColor(QPalette::WindowText, color);
    specialPeriodCount[idx]->setPalette(pal);
}

void SpecialPackets::SetAlarmValSlot(int idx, int value)
{
    alarmVal[idx]->display(value);
}

void SpecialPackets::DisplaySpecialFieldSlot(unsigned int idx, unsigned int fld,
        int value)
{
    if(specialField[idx] != NULL)
        specialField[idx][fld]->display(value);
}

void SpecialPackets::DisplaySpecialFieldSlot(unsigned int idx, unsigned int fld,
        double value)
{
    if(specialField[idx] != NULL)
        specialField[idx][fld]->display(value);
}

void SpecialPackets::ChangeSpecialEnabled(unsigned int idx, int state)
{
    specialCount[idx]->setEnabled(state);
    specialPeriodCount[idx]->setEnabled((state && pGssStruct->getSpecialInfoPeriod(idx)));
    pGssStruct->setSpecialInfoEnabled(idx, state);
    return;
}

void SpecialPackets::CreateSpecialArrays()
{
    specialName[0] = pMainGui->mainContent.specialName_0;
    specialCount[0] = pMainGui->mainContent.specialCount_0;
    specialEnabled[0] = pMainGui->mainContent.specialEnabled_0;
    specialPrint[0] = pMainGui->mainContent.specialPrint_0;
    specialPeriodCount[0] = pMainGui->mainContent.specialPeriodCount_0;
    specialName[1] = pMainGui->mainContent.specialName_1;
    specialCount[1] = pMainGui->mainContent.specialCount_1;
    specialEnabled[1] = pMainGui->mainContent.specialEnabled_1;
    specialPrint[1] = pMainGui->mainContent.specialPrint_1;
    specialPeriodCount[1] = pMainGui->mainContent.specialPeriodCount_1;
    specialName[2] = pMainGui->mainContent.specialName_2;
    specialCount[2] = pMainGui->mainContent.specialCount_2;
    specialEnabled[2] = pMainGui->mainContent.specialEnabled_2;
    specialPrint[2] = pMainGui->mainContent.specialPrint_2;
    specialPeriodCount[2] = pMainGui->mainContent.specialPeriodCount_2;
    specialName[3] = pMainGui->mainContent.specialName_3;
    specialCount[3] = pMainGui->mainContent.specialCount_3;
    specialEnabled[3] = pMainGui->mainContent.specialEnabled_3;
    specialPrint[3] = pMainGui->mainContent.specialPrint_3;
    specialName[4] = pMainGui->mainContent.specialName_4;
    specialPeriodCount[3] = pMainGui->mainContent.specialPeriodCount_3;
    specialCount[4] = pMainGui->mainContent.specialCount_4;
    specialEnabled[4] = pMainGui->mainContent.specialEnabled_4;
    specialPrint[4] = pMainGui->mainContent.specialPrint_4;
    specialPeriodCount[4] = pMainGui->mainContent.specialPeriodCount_4;
    specialName[5] = pMainGui->mainContent.specialName_5;
    specialCount[5] = pMainGui->mainContent.specialCount_5;
    specialEnabled[5] = pMainGui->mainContent.specialEnabled_5;
    specialPrint[5] = pMainGui->mainContent.specialPrint_5;
    specialPeriodCount[5] = pMainGui->mainContent.specialPeriodCount_5;
    specialName[6] = pMainGui->mainContent.specialName_6;
    specialCount[6] = pMainGui->mainContent.specialCount_6;
    specialEnabled[6] = pMainGui->mainContent.specialEnabled_6;
    specialPrint[6] = pMainGui->mainContent.specialPrint_6;
    specialPeriodCount[6] = pMainGui->mainContent.specialPeriodCount_6;
    specialName[7] = pMainGui->mainContent.specialName_7;
    specialCount[7] = pMainGui->mainContent.specialCount_7;
    specialEnabled[7] = pMainGui->mainContent.specialEnabled_7;
    specialPrint[7] = pMainGui->mainContent.specialPrint_7;
    specialPeriodCount[7] = pMainGui->mainContent.specialPeriodCount_7;
    specialName[8] = pMainGui->mainContent.specialName_8;
    specialCount[8] = pMainGui->mainContent.specialCount_8;
    specialEnabled[8] = pMainGui->mainContent.specialEnabled_8;
    specialPrint[8] = pMainGui->mainContent.specialPrint_8;
    specialPeriodCount[8] = pMainGui->mainContent.specialPeriodCount_8;
    specialName[9] = pMainGui->mainContent.specialName_9;
    specialCount[9] = pMainGui->mainContent.specialCount_9;
    specialEnabled[9] = pMainGui->mainContent.specialEnabled_9;
    specialPrint[9] = pMainGui->mainContent.specialPrint_9;
    specialPeriodCount[9] = pMainGui->mainContent.specialPeriodCount_9;
    specialName[10] = pMainGui->mainContent.specialName_10;
    specialCount[10] = pMainGui->mainContent.specialCount_10;
    specialEnabled[10] = pMainGui->mainContent.specialEnabled_10;
    specialPrint[10] = pMainGui->mainContent.specialPrint_10;
    specialPeriodCount[10] = pMainGui->mainContent.specialPeriodCount_10;
    specialName[11] = pMainGui->mainContent.specialName_11;
    specialCount[11] = pMainGui->mainContent.specialCount_11;
    specialEnabled[11] = pMainGui->mainContent.specialEnabled_11;
    specialPrint[11] = pMainGui->mainContent.specialPrint_11;
    specialPeriodCount[11] = pMainGui->mainContent.specialPeriodCount_11;
    specialName[12] = pMainGui->mainContent.specialName_12;
    specialCount[12] = pMainGui->mainContent.specialCount_12;
    specialEnabled[12] = pMainGui->mainContent.specialEnabled_12;
    specialPrint[12] = pMainGui->mainContent.specialPrint_12;
    specialPeriodCount[12] = pMainGui->mainContent.specialPeriodCount_12;
    specialName[13] = pMainGui->mainContent.specialName_13;
    specialCount[13] = pMainGui->mainContent.specialCount_13;
    specialEnabled[13] = pMainGui->mainContent.specialEnabled_13;
    specialPrint[13] = pMainGui->mainContent.specialPrint_13;
    specialName[14] = pMainGui->mainContent.specialName_14;
    specialPeriodCount[13] = pMainGui->mainContent.specialPeriodCount_13;
    specialCount[14] = pMainGui->mainContent.specialCount_14;
    specialEnabled[14] = pMainGui->mainContent.specialEnabled_14;
    specialPrint[14] = pMainGui->mainContent.specialPrint_14;
    specialPeriodCount[14] = pMainGui->mainContent.specialPeriodCount_14;
    specialName[15] = pMainGui->mainContent.specialName_15;
    specialCount[15] = pMainGui->mainContent.specialCount_15;
    specialEnabled[15] = pMainGui->mainContent.specialEnabled_15;
    specialPrint[15] = pMainGui->mainContent.specialPrint_15;
    specialPeriodCount[15] = pMainGui->mainContent.specialPeriodCount_15;
    specialName[16] = pMainGui->mainContent.specialName_16;
    specialCount[16] = pMainGui->mainContent.specialCount_16;
    specialEnabled[16] = pMainGui->mainContent.specialEnabled_16;
    specialPrint[16] = pMainGui->mainContent.specialPrint_16;
    specialPeriodCount[16] = pMainGui->mainContent.specialPeriodCount_16;
}

void SpecialPackets::CreateAlarmVals()
{
    pMainGui->mainContent.alarmValName_0->hide();
    pMainGui->mainContent.alarmVal0->hide();
    pMainGui->mainContent.alarmValName_1->hide();
    pMainGui->mainContent.alarmVal1->hide();
    pMainGui->mainContent.alarmValName_2->hide();
    pMainGui->mainContent.alarmVal2->hide();
    alarmValName[0] = pMainGui->mainContent.alarmValName_0;
    alarmVal[0] = pMainGui->mainContent.alarmVal0;
    alarmValName[1] = pMainGui->mainContent.alarmValName_1;
    alarmVal[1] = pMainGui->mainContent.alarmVal1;
    alarmValName[2] = pMainGui->mainContent.alarmValName_2;
    alarmVal[2] = pMainGui->mainContent.alarmVal2;
}