/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SpWTimeCodesDialog.h
 * Author: user
 *
 * Created on 9 de julio de 2019, 10:50
 */

#ifndef _SPWTIMECODESDIALOG_H
#define _SPWTIMECODESDIALOG_H

#include "ui_SpWTimeCodesDialog.h"
#include "mainForm.h"
#include <QPushButton>
#include <QSpinBox>
#include "../GuiClasses/HexSpinBox.h"

class SpWTimeCodesDialog : public QDialog {
    
    Q_OBJECT
public:
    SpWTimeCodesDialog(gssStructs * origGssStruct, Logs * origLogs,
           TestManager * origTestMgr, mainForm * origMainGui,  unsigned int port);
    ~SpWTimeCodesDialog();
    
signals:
    void setStatusBarColor(const QString, int r, int g, int b);
    void setPeriodicalSpWTCText(const QString &text);
    
public slots:
    void ShowSendSingleSpWTCDialog();
    void ShowEnableDisableSpWTCDialog();

private slots:
    void sendSingleOrEnablePeriodicalSpWTCs();
    
private:
    Ui::SpWTimeCodesDialog widget;
    gssStructs * pGssStruct;
    Logs * pLogs;
    TestManager * pTestMgr;
    mainForm * pMainGui;
    
    unsigned int spw_tc_port;
    portConfig * pSpWTCPort;
    unsigned int last_value;
    unsigned int last_freq_hertz;
    bool dialog_is_single_spw_tc;
    
    HexSpinBox * tcValueBox;
    QSpinBox * periodicalFreq;
    QPushButton * spwTC_button;
    QLabel * hertz;
};

#endif /* _SPWTIMECODESDIALOG_H */
