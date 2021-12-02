/**
 * \file	RxTxInfo.h
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

#ifndef RXTXINFO_H
#define RXTXINFO_H

#include "../Forms/mainForm.h"

class RxTxInfo : public QObject
{
Q_OBJECT

public:
    int ConfigTabs(gssStructs * pGssStruct, mainForm * origMainGui,
            Logs * origLogs);
    void UnconfigTabs(void);

public slots:
    void TxDataInc(unsigned int tab);
    void TxDataPrintPacket(unsigned int tab, const unsigned char * packet,
        int length, unsigned int portPhyHeaderOffset);
    void RxDataInc(unsigned int tab);
    void RxDataPrintPacket(unsigned int tab, const unsigned char * packet,
        int length, unsigned int portPhyHeaderOffset);
    void showLog();
    void setEnabledLogPorts(bool status);
    void ClearRxTxData();
    void showSentPacket(unsigned int tab, int length,
            unsigned char * packet);

private:
    gssStructs * pGssStruct;
    mainForm * pMainGui;
    Logs * pLogs;
    
    unsigned int tabsNo;
    QWidget ** tab;
    QLCDNumber *txCount[MAX_INTERFACES], *rxCount[MAX_INTERFACES];
    QLineEdit *txPrint[MAX_INTERFACES], *rxPrint1[MAX_INTERFACES],
            *rxPrint2[MAX_INTERFACES], *rxPrint3[MAX_INTERFACES];
    QPushButton *logButton[MAX_INTERFACES];

    void fillRxTxTab(unsigned int idx, ioTypes currentType);
};
#endif /* RXTXINFO_H */