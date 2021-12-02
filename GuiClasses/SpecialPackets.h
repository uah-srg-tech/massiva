/**
 * \file	SpecialPackets.h
 * \brief	functions for show info about structured special packets (declaration)
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
#ifndef SPECIALPACKETS_H
#define SPECIALPACKETS_H 

#include <QLCDNumber>
#include <QCheckBox>
#include <QScrollArea>
#include "../Forms/mainForm.h"
#include "CheckSpecialPeriods.h"

class SpecialPackets : public QObject
{
    Q_OBJECT
public:
    void ConfigSpecialPackets(gssStructs * origGssStruct, mainForm * origMainGui);
    void UnconfigSpecialPackets();
    
public slots:
    void ResetSpecialPackets(void);
    void ChangeSpecialEnabledSlot(int state);
    void ChangeSpecialPrintSlot(int state);
    void IncreaseDisplaySpecialValueSlot(unsigned int idx);
    void SetEnabledSpecialPeriodSlot(unsigned int idx, int state);
    void DisplaySpecialPeriodSlot(unsigned int idx, double value, QColor color);
    void SetAlarmValSlot(int idx, int value);
    void DisplaySpecialFieldSlot(unsigned int idx, unsigned int fld, int value);
    void DisplaySpecialFieldSlot(unsigned int idx, unsigned int fld, double value);
    void ToggleSpecial(const spec_in_step *special);
    
private:
    gssStructs * pGssStruct;
    mainForm * pMainGui;
    CheckSpecialPeriods * specialCheckWorker;
    QThread * specialCheckThread;
    
    unsigned int tabs;
    unsigned int fields[MAX_SPECIAL_PACKETS];
    QLabel *specialName[MAX_SPECIAL_PACKETS];
    QLCDNumber *specialCount[MAX_SPECIAL_PACKETS];
    QCheckBox *specialEnabled[MAX_SPECIAL_PACKETS];
    QCheckBox *specialPrint[MAX_SPECIAL_PACKETS];
    QLCDNumber *specialPeriodCount[MAX_SPECIAL_PACKETS];
    QLabel *alarmValName[MAX_NUMBER_OF_ALARM_VALS];
    QLCDNumber * alarmVal[MAX_NUMBER_OF_ALARM_VALS];
    QLCDNumber **specialField[MAX_SPECIAL_PACKETS];
    QWidget ** specialTab;
    
    int ConfigSpecialTabs(special_packet_info * infos, output * spPackets);
    void ChangeSpecialEnabled(unsigned int idx, int state);    
    void CreateSpecialArrays();
    void CreateAlarmVals();
};

#endif /* SPECIALPACKETS_H */
