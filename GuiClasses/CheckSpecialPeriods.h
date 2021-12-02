/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CheckSpecialPeriods.h
 * Author: Aaron
 *
 * Created on 22 de septiembre de 2017, 18:46
 */

#ifndef CHECKSPECIALPERIODS_H
#define CHECKSPECIALPERIODS_H

#include <QtCore>
#include <QColor>
#include "../CommonClasses/gssStructs.h"

class CheckSpecialPeriods : public QObject
{
    Q_OBJECT

public:
    CheckSpecialPeriods(gssStructs * origGssStruct);
    void SetEnabled(bool state);

signals:
    void DisplaySpecialPeriod(unsigned int idx, double value, QColor color);
    void finished();

private slots:
    void CheckSpecialPeriodsSlot();
    
private:
    gssStructs * pGssStruct;
    bool checkSpecialPeriodsEnabled;
    QWaitCondition sleepWaitCondition;

};

#endif /* CHECKSPECIALPERIODS_H */

