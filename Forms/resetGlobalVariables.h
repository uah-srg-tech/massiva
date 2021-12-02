/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   resetGlobalVariables.h
 * Author: Aaron
 *
 * Created on 18 de marzo de 2020, 18:33
 */

#ifndef _RESETGLOBALVARIABLES_H
#define _RESETGLOBALVARIABLES_H

#include "ui_resetGlobalVariables.h"
#include "../CommonClasses/gssStructs.h"
#include "../CommonClasses/Logs.h"

class resetGlobalVariables : public QDialog {
    Q_OBJECT

public:
    resetGlobalVariables(gssStructs * origGssStruct, Logs * origLogs);
    void setNumberOfGlobalVars();
    
public slots:
    void showRGVDialog();
    void closeRGVDialog();

private:
    Ui::resetGlobalVariables widget;
    gssStructs * pGssStruct;
    Logs * pLogs;
    
private slots:
    void enableDisableSingleGlobalBV(int newStatus);
    void changeGlobalBVName(int newIndex);
    void resetGlobalBV();
};

#endif /* _RESETGLOBALVARIABLES_H */
