/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mainForm.h
 * Author: Aaron
 *
 * Created on 8 de junio de 2017, 12:16
 */

#ifndef MAINFORM_H
#define MAINFORM_H

#include "ui_mainForm.h"
#include "../CommonClasses/gssStructs.h"
#include "../CommonClasses/Logs.h"
#include "../CommonClasses/InitialConfig.h"
#include "../TesterClasses/TestManager.h"
#include "../GuiClasses/DoubleClickButton.h"
#include "resetGlobalVariables.h"
#include "AboutDialog.h"
#ifdef PLOTS
#include <qwt_plot.h>
#endif

class mainForm : public QMainWindow
{
Q_OBJECT

public:
   enum class PopUpType
   {
       INFO,
       QUESTION
   };
    
    Ui::mainForm mainContent;
    DoubleClickButton *statusBarButton;
    
    mainForm(gssStructs * origGssStruct, Logs * origLogs,
            InitialConfig * origInitialConfig, TestManager * origTestMgr,
            resetGlobalVariables * origRgv, const char * gssVersionStr);
    void createProceduresList();
    bool LaunchAllIsChecked();
    bool getPopUpBtnClickedYes();
    void setResetGlobalVarsStatus(bool status);
    void setMaximumTxStep(int maxValue);
    void setMaximumInput(int maxValue);
    void setMaximumRxStep(int maxValue);
    void setMaximumOutput(int maxValue);
    void setValueTxStep(int value);
    void setValueInput(int value);
    void setValueOutput(int value);
    void setValueRxStep(int value);
    void setVisibleTxStep(bool visible);
    void setVisibleInput(bool visible);
    void setVisibleRxStep(bool visible);
    void setVisibleOutput(bool visible);
    int SwitchConfigGSS();

signals:
    void setEnabledLogPortsSignal(bool status);
    void ClearRxTxData();
    void maximumTxStep(int maxValue);
    void maximumInput(int maxValue);
    void maximumRxStep(int maxValue);
    void maximumOutput(int maxValue);
    void valueTxStep(int value);
    void valueInput(int value);
    void valueRxStep(int value);
    void valueOutput(int value);
    void visibleTxStep(bool visible);
    void visibleInput(bool visible);
    void visibleRxStep(bool visible);
    void visibleOutput(bool visible);
    void closePlotForm();
    void closePTCsDialog();
    void closeAboutDialog();
    void closeRGVDialog();
    void setEnabledActionAtPort(unsigned int port, bool status);

public slots:
    void setEnabledControls(bool status, bool setMenu, bool setLaunch,
            bool setLogs);
    void setEnabledLaunch(bool status);
    void setEnabledLaunchAllStop(bool status);
    void setLoadLaunchAllItalic(bool status);
    void SelectTP(unsigned int index, bool status);
    void setStatusBar(const QString &text);
    void setStatusBarColor(const QString &text, int r, int g, int b);
    void clearStatusBarColor();
    void removeFormatTestProcedure(unsigned int test);
    void ShowAlarmMessageBox(const QString &text);
    void EditConfigINI();
    void EditConfigGSS();
    void ChooseConfigGSS();
    void setPeriodicalSpWTCText(const QString &text);
    void showPopUp(const QString &title, const QString &text,
            mainForm::PopUpType type);
    void setRxOutput(unsigned int currentStepNumber,
        unsigned int numberOfOutputs, unsigned int outputsReceived);
    void setActionsAtPort(unsigned int port, bool status);
    
#ifdef PLOTS
    void replot(QwtPlot * pPlot);
#endif

private:
    gssStructs * pGssStruct;
    Logs * pLogs;
    InitialConfig * pInitialConfig;
    TestManager * pTestMgr;
    resetGlobalVariables * pRgv;
        
    AboutDialog * pAboutDialog = NULL;
    QLabel *statusBarLabel;
    int gssRxStep;
    bool popUpBtnClickedYes;
    bool resetGlobalVarsStatus;
    
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);
    void changeBarColor(QProgressBar * bar, QColor color);
    void proceduresListClicked(int row, int column);

private slots:
    void proceduresListClickedSlot(int row, int column);
    void proceduresListDoubleClickedSlot(int row, int column);
    void showLog();
    void resetGSS();
    void openCurrentLogFolder();
    void selectLogFolder();
    void openUserManual();
};

#endif /* MAINFORM_H */