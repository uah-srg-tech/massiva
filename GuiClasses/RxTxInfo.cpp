/**
 * \file	RxTxInfo.cpp
 * \brief	functions for info about sent and received packets (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		14/06/2017 at 13:11:01
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c)2017, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <QProcess>
#include <cstdio>				/* snprintf */	
#include "RxTxInfo.h"

int RxTxInfo::ConfigTabs(gssStructs * origGssStruct, mainForm * origMainGui,
        Logs * origLogs)
{
    pGssStruct = origGssStruct;
    pMainGui = origMainGui;
    pLogs = origLogs;
    
    const portConfig * ports = pGssStruct->getPointerToPorts();
    tabsNo = pGssStruct->getNumberOfPorts();
    tab = new QWidget*[tabsNo];
    
    for(unsigned int idx=0; idx<tabsNo; ++idx)
    {
        /* first remove example tab from UI (can't be removed in Qt Designer */
        if(idx == 0)
            pMainGui->mainContent.portsTab->removeTab(0);
        
        tab[idx] = new QWidget();
        tab[idx]->setObjectName(QString("tab" + QString::number(idx)));
        fillRxTxTab(idx, ports[idx].ioType);
        pMainGui->mainContent.portsTab->addTab(tab[idx], QString::fromUtf8(ports[idx].name));
        connect(logButton[idx], SIGNAL (released()), this, SLOT (showLog()));
        connect(pMainGui, SIGNAL (setEnabledLogPortsSignal(bool)), this,
                SLOT (setEnabledLogPorts(bool)));
    }
    ClearRxTxData();
    connect(pMainGui->statusBarButton, SIGNAL(doubleClicked()),
            this, SLOT(ClearRxTxData()));
    return 0;
}


void RxTxInfo::UnconfigTabs(void)
{
    for(unsigned int idx=0; idx<tabsNo; ++idx)
    {
        delete tab[idx];
        pMainGui->mainContent.portsTab->removeTab(idx);
    }
    delete [] tab;
}

void RxTxInfo::fillRxTxTab(unsigned int idx, ioTypes currentType)
{
    QGridLayout * gridLayout = new QGridLayout(tab[idx]);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));    
    QString tabIdxStr = QString::number(idx);
    
    if(currentType != OUT_PORT)
    {
        txCount[idx] = new QLCDNumber(tab[idx]);
        txCount[idx]->setObjectName(QString("tx" + tabIdxStr + "Count"));
        txCount[idx]->setGeometry(QRect(10, 10, 81, 21));
        txCount[idx]->setFrameShadow(QFrame::Plain);
        txCount[idx]->setSmallDecimalPoint(true);
        txCount[idx]->setMode(QLCDNumber::Dec);
        txCount[idx]->setSegmentStyle(QLCDNumber::Flat);
        txCount[idx]->setNumDigits(6);
        txCount[idx]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        gridLayout->addWidget(txCount[idx], 0, 0, 1, 1);
        txPrint[idx] = new QLineEdit(tab[idx]);
        txPrint[idx]->setObjectName(QString("tx" + tabIdxStr + "Print"));
        txPrint[idx]->setGeometry(QRect(100, 10, 391, 21));
        txPrint[idx]->setDragEnabled(false);
        txPrint[idx]->setReadOnly(true);
        txPrint[idx]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        gridLayout->addWidget(txPrint[idx], 0, 1, 1, 1);
    }
    
    QFrame *line = new QFrame(tab[idx]);
    line->setObjectName(QString("line" + tabIdxStr));
    line->setGeometry(QRect(10, 30, 491, 16));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    gridLayout->addWidget(line, 1, 0, 1, 2);
    
    if(currentType != IN_PORT)
    {
        rxCount[idx] = new QLCDNumber(tab[idx]);
        rxCount[idx]->setObjectName(QString("rx" + tabIdxStr + "Count"));
        rxCount[idx]->setGeometry(QRect(10, 60, 81, 21));
        rxCount[idx]->setFrameShadow(QFrame::Plain);
        rxCount[idx]->setSmallDecimalPoint(true);
        rxCount[idx]->setMode(QLCDNumber::Dec);
        rxCount[idx]->setSegmentStyle(QLCDNumber::Flat);
        rxCount[idx]->setNumDigits(6);
        rxCount[idx]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        gridLayout->addWidget(rxCount[idx], 2, 0, 2, 1);
        rxPrint1[idx] = new QLineEdit(tab[idx]);
        rxPrint1[idx]->setObjectName(QString("rx" + tabIdxStr + "Print1"));
        rxPrint1[idx]->setGeometry(QRect(100, 50, 391, 21));
        rxPrint1[idx]->setDragEnabled(false);
        rxPrint1[idx]->setReadOnly(true);
        rxPrint1[idx]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        gridLayout->addWidget(rxPrint1[idx], 2, 1, 1, 1);
        rxPrint2[idx] = new QLineEdit(tab[idx]);
        rxPrint2[idx]->setObjectName(QString("rx" + tabIdxStr + "Print2"));
        rxPrint2[idx]->setGeometry(QRect(100, 80, 391, 21));
        rxPrint2[idx]->setDragEnabled(false);
        rxPrint2[idx]->setReadOnly(true);
        rxPrint2[idx]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        gridLayout->addWidget(rxPrint2[idx], 3, 1, 1, 1);
        rxPrint3[idx] = new QLineEdit(tab[idx]);
        rxPrint3[idx]->setObjectName(QString("rx" + tabIdxStr + "Print3"));
        rxPrint3[idx]->setGeometry(QRect(100, 110, 391, 21));
        rxPrint3[idx]->setDragEnabled(false);
        rxPrint3[idx]->setReadOnly(true);
        rxPrint3[idx]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        gridLayout->addWidget(rxPrint3[idx], 4, 1, 1, 1);
    }
    logButton[idx] = new QPushButton(tab[idx]);
    logButton[idx]->setObjectName(QString("log" + tabIdxStr + "Button"));
    logButton[idx]->setGeometry(QRect(10, 100, 81, 21));
    logButton[idx]->setText(QApplication::translate("mainForm", "Log", 0,
            QApplication::UnicodeUTF8));
    gridLayout->addWidget(logButton[idx], 4, 0, 1, 1);
    return;
}

void RxTxInfo::TxDataInc(unsigned int tab)
{
    if(pGssStruct->getPortIoType(tab) != OUT_PORT)
    {
        if(txCount[tab]->intValue() == 999999)
        {
            QPalette pal = txCount[tab]->palette();
            pal.setColor(QPalette::WindowText, Qt::darkRed);
            txCount[tab]->setPalette(pal);
            txCount[tab]->display(0);
        }
        else
        {
            txCount[tab]->display(txCount[tab]->intValue() + 1);
        }
    }
}

void RxTxInfo::TxDataPrintPacket(unsigned int tab, const unsigned char * packet,
        int length, unsigned int portPhyHeaderOffset)
{
    if(pGssStruct->getPortIoType(tab) != OUT_PORT)
    {
        const unsigned char * printPacket = packet;
        if(pGssStruct->getPhyHeaderShowGss() == DISABLED)
        {
            printPacket = &packet[portPhyHeaderOffset];
            length -= portPhyHeaderOffset;
        }
        QByteArray printingPacket((const char*)printPacket, length);
        printingPacket = printingPacket.toHex();
        txPrint[tab]->setText(QString(printingPacket.toUpper()));
    }
    return;
}

void RxTxInfo::RxDataInc(unsigned int tab)
{
    if(pGssStruct->getPortIoType(tab) != IN_PORT)
    {
        if(rxCount[tab]->intValue() == 999999)
        {
            QPalette pal = rxCount[tab]->palette();
            pal.setColor(QPalette::WindowText, Qt::darkRed);
            rxCount[tab]->setPalette(pal);
            rxCount[tab]->display(0);
        }
        else
        {
            rxCount[tab]->display(rxCount[tab]->intValue() + 1);
        }
    }
}

void RxTxInfo::RxDataPrintPacket(unsigned int tab, const unsigned char * packet,
        int length, unsigned int portPhyHeaderOffset)
{
    if(pGssStruct->getPortIoType(tab) != IN_PORT)
    {
        const unsigned char * printPacket = packet;
        if(pGssStruct->getPhyHeaderShowGss() == DISABLED)
        {
            printPacket = &packet[portPhyHeaderOffset];
            length -= portPhyHeaderOffset;
        }
        rxPrint3[tab]->setText(rxPrint2[tab]->text());
        rxPrint2[tab]->setText(rxPrint1[tab]->text());
        QByteArray printingPacket((const char*)printPacket, length);
        printingPacket = printingPacket.toHex();
        rxPrint1[tab]->setText(QString(printingPacket.toUpper()));
    }
}

void RxTxInfo::showLog(void)
{
    unsigned int tab;
    QPushButton * origin = qobject_cast<QPushButton *>(sender());
    for(tab=0; tab<tabsNo; ++tab)
    {
        if(origin == logButton[tab])
            break;
    }
    pLogs->ShowLog(tab);
    return;
}

void RxTxInfo::setEnabledLogPorts(bool status)
{
    for(unsigned int tab=0; tab<tabsNo; ++tab)
    {
        logButton[tab]->setEnabled(status);
    }
    return;
}

void RxTxInfo::ClearRxTxData(void)
{
    for(unsigned int tab=0; tab<tabsNo; ++tab)
    {
        if(pGssStruct->getPortIoType(tab) != OUT_PORT)
        {
            QPalette pal = txCount[tab]->palette();
            pal.setColor(QPalette::WindowText, Qt::black);
            txCount[tab]->setPalette(pal);
            txCount[tab]->display(0);
            txPrint[tab]->clear();
        }
        if(pGssStruct->getPortIoType(tab) != IN_PORT)
        {
            QPalette pal = rxCount[tab]->palette();
            pal.setColor(QPalette::WindowText, Qt::black);
            rxCount[tab]->setPalette(pal);
            rxCount[tab]->display(0);
            rxPrint1[tab]->clear();
            rxPrint2[tab]->clear();
            rxPrint3[tab]->clear();
        }
    }
    return;
}

void RxTxInfo::showSentPacket(unsigned int tab, int length,
        unsigned char * packet)
{
    if(tab >= tabsNo)
        return;

    char tempPacket[8300];
    for(int i=0; i<length; ++i)
        snprintf(&tempPacket[2*i], 3, "%02X", packet[tab]);
    txPrint[tab]->setText(tempPacket);
    return;
}