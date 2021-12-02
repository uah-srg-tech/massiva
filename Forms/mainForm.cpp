/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   mainForm.cpp
 * Author: Aaron
 *
 * Created on 8 de junio de 2017, 12:16
 */

#include "mainForm.h"
#include <QToolTip>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <unistd.h>			/* chdir */
#include "../CommonClasses/gssStructs.h"
#include "../CommonClasses/InitialConfig.h"

mainForm::mainForm(gssStructs * origGssStruct, Logs * origLogs,
        InitialConfig * origInitialConfig, TestManager * origTestMgr,
        resetGlobalVariables * origRgv, const char * gssVersionStr)
{
    mainContent.setupUi(this);
    
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pInitialConfig = origInitialConfig;
    pTestMgr = origTestMgr;
    pRgv = origRgv;
    
    gssRxStep = -1;
    popUpBtnClickedYes = false;
    resetGlobalVarsStatus = false;
    
    pAboutDialog = new AboutDialog(gssVersionStr);
    connect(mainContent.actionAbout, SIGNAL(triggered()),
            pAboutDialog, SLOT(showAboutDialog()));
    connect(this, SIGNAL (closeAboutDialog()),
            pAboutDialog, SLOT(closeAboutDialog()));
    
    connect(mainContent.proceduresList, SIGNAL(cellClicked(int,int)),
        this, SLOT(proceduresListClickedSlot(int,int)));
    connect(mainContent.proceduresList, SIGNAL(cellDoubleClicked(int,int)),
        this, SLOT(proceduresListDoubleClickedSlot(int,int)));
    connect(mainContent.logButton, SIGNAL(released()),
            this, SLOT(showLog()));
    connect(mainContent.rawLogButton, SIGNAL(released()),
            this, SLOT(showLog()));
    connect(mainContent.actionEdit_current_GSS_config_file, SIGNAL(triggered()),
            this, SLOT(EditConfigGSS()));
    connect(mainContent.actionSwitch_GSS_config_file, SIGNAL(triggered()),
            this, SLOT(ChooseConfigGSS()));
    connect(mainContent.actionReset_GSS, SIGNAL(triggered()),
            this, SLOT(resetGSS()));
    connect(mainContent.actionOpen_Logs_folder, SIGNAL(triggered()),
            this, SLOT(openCurrentLogFolder()));
    connect(mainContent.actionSelect_Logs_folder, SIGNAL(triggered()),
            this, SLOT(selectLogFolder()));
    connect(mainContent.actionUserManual, SIGNAL (triggered()),
            this, SLOT(openUserManual()));
    connect(mainContent.resetGlobalVars, SIGNAL (released()),
            pRgv, SLOT(showRGVDialog()));
    connect(this, SIGNAL (closeRGVDialog()), pRgv, SLOT (closeRGVDialog()));
    mainContent.barTxStep->hide();
    mainContent.barInput->hide();
    mainContent.barRxStep->hide();
    mainContent.barOutput->hide();
    connect(this, SIGNAL(maximumTxStep(int)), mainContent.barTxStep, SLOT(setMaximum(int)));
    connect(this, SIGNAL(maximumInput(int)), mainContent.barInput, SLOT(setMaximum(int)));
    connect(this, SIGNAL(maximumRxStep(int)), mainContent.barRxStep, SLOT(setMaximum(int)));
    connect(this, SIGNAL(maximumOutput(int)), mainContent.barOutput, SLOT(setMaximum(int)));
    connect(this, SIGNAL(valueTxStep(int)), mainContent.barTxStep, SLOT(setValue(int)));
    connect(this, SIGNAL(valueInput(int)), mainContent.barInput, SLOT(setValue(int)));
    connect(this, SIGNAL(valueRxStep(int)), mainContent.barRxStep, SLOT(setValue(int)));
    connect(this, SIGNAL(valueOutput(int)), mainContent.barOutput, SLOT(setValue(int)));
    connect(this, SIGNAL(visibleTxStep(bool)), mainContent.barTxStep, SLOT(setVisible(bool)));
    connect(this, SIGNAL(visibleInput(bool)), mainContent.barInput, SLOT(setVisible(bool)));
    connect(this, SIGNAL(visibleRxStep(bool)), mainContent.barRxStep, SLOT(setVisible(bool)));
    connect(this, SIGNAL(visibleOutput(bool)), mainContent.barOutput, SLOT(setVisible(bool)));
    QFont tableFont(mainContent.proceduresList->font());
    tableFont.setPointSize(8);
    mainContent.proceduresList->setFont(tableFont);
    mainContent.proceduresList->installEventFilter(this);
    mainContent.specialName_0->hide();
    mainContent.specialCount_0->hide();
    mainContent.specialEnabled_0->hide();
    mainContent.specialPrint_0->hide();
    mainContent.specialPeriodCount_0->hide();
    mainContent.specialName_1->hide();
    mainContent.specialCount_1->hide();
    mainContent.specialEnabled_1->hide();
    mainContent.specialPrint_1->hide();
    mainContent.specialPeriodCount_1->hide();
    mainContent.specialName_2->hide();
    mainContent.specialCount_2->hide();
    mainContent.specialEnabled_2->hide();
    mainContent.specialPrint_2->hide();
    mainContent.specialPeriodCount_2->hide();
    mainContent.specialName_3->hide();
    mainContent.specialCount_3->hide();
    mainContent.specialEnabled_3->hide();
    mainContent.specialPrint_3->hide();
    mainContent.specialPeriodCount_3->hide();
    mainContent.specialName_4->hide();
    mainContent.specialCount_4->hide();
    mainContent.specialEnabled_4->hide();
    mainContent.specialPrint_4->hide();
    mainContent.specialPeriodCount_4->hide();
    mainContent.specialName_5->hide();
    mainContent.specialCount_5->hide();
    mainContent.specialEnabled_5->hide();
    mainContent.specialPrint_5->hide();
    mainContent.specialPeriodCount_5->hide();
    mainContent.specialName_6->hide();
    mainContent.specialCount_6->hide();
    mainContent.specialEnabled_6->hide();
    mainContent.specialPrint_6->hide();
    mainContent.specialPeriodCount_6->hide();
    mainContent.specialName_7->hide();
    mainContent.specialCount_7->hide();
    mainContent.specialEnabled_7->hide();
    mainContent.specialPrint_7->hide();
    mainContent.specialPeriodCount_7->hide();
    mainContent.specialName_8->hide();
    mainContent.specialCount_8->hide();
    mainContent.specialEnabled_8->hide();
    mainContent.specialPrint_8->hide();
    mainContent.specialPeriodCount_8->hide();
    mainContent.specialName_9->hide();
    mainContent.specialCount_9->hide();
    mainContent.specialEnabled_9->hide();
    mainContent.specialPrint_9->hide();
    mainContent.specialPeriodCount_9->hide();
    mainContent.specialName_10->hide();
    mainContent.specialCount_10->hide();
    mainContent.specialEnabled_10->hide();
    mainContent.specialPrint_10->hide();
    mainContent.specialPeriodCount_10->hide();
    mainContent.specialName_11->hide();
    mainContent.specialCount_11->hide();
    mainContent.specialEnabled_11->hide();
    mainContent.specialPrint_11->hide();
    mainContent.specialPeriodCount_11->hide();
    mainContent.specialName_12->hide();
    mainContent.specialCount_12->hide();
    mainContent.specialEnabled_12->hide();
    mainContent.specialPrint_12->hide();
    mainContent.specialPeriodCount_12->hide();
    mainContent.specialName_13->hide();
    mainContent.specialCount_13->hide();
    mainContent.specialEnabled_13->hide();
    mainContent.specialPrint_13->hide();
    mainContent.specialPeriodCount_13->hide();
    mainContent.specialName_14->hide();
    mainContent.specialCount_14->hide();
    mainContent.specialEnabled_14->hide();
    mainContent.specialPrint_14->hide();
    mainContent.specialPeriodCount_14->hide();
    mainContent.specialName_15->hide();
    mainContent.specialCount_15->hide();
    mainContent.specialEnabled_15->hide();
    mainContent.specialPrint_15->hide();
    mainContent.specialPeriodCount_15->hide();
    mainContent.specialName_16->hide();
    mainContent.specialCount_16->hide();
    mainContent.specialEnabled_16->hide();
    mainContent.specialPrint_16->hide();
    mainContent.specialPeriodCount_16->hide();
    mainContent.alarmValName_0->hide();
    mainContent.alarmVal0->hide();
    mainContent.alarmValName_1->hide();
    mainContent.alarmVal1->hide();
    mainContent.alarmValName_2->hide();
    mainContent.alarmVal2->hide();
    mainContent.specialFieldsTab->hide();
    mainContent.statusbar->setSizeGripEnabled(false);
    statusBarLabel = new QLabel(mainContent.statusbar);
    statusBarLabel->setObjectName(QString::fromUtf8("statusBarLabel"));
    statusBarLabel->setFixedWidth(980);
    mainContent.statusbar->addWidget(statusBarLabel);
    statusBarButton = new DoubleClickButton(mainContent.statusbar);
    statusBarButton->setObjectName(QString::fromUtf8("statusBarButton"));
    mainContent.statusbar->addPermanentWidget(statusBarButton);
    connect(statusBarButton, SIGNAL(released()),
            this, SLOT(clearStatusBarColor()));
}

void mainForm::createProceduresList()
{
    unsigned int numberOfProcedures = pGssStruct->getNumberOfProcedures();
    test_proc * procedures = pGssStruct->getPointerToProceduresList();

    mainContent.proceduresList->horizontalHeader()->hide();
    mainContent.proceduresList->verticalHeader()->hide();
    mainContent.proceduresList->setRowCount(numberOfProcedures);
    mainContent.proceduresList->setColumnCount(2);

    for(unsigned int row = 0; row < numberOfProcedures; ++row)
    {
        mainContent.proceduresList->setItem(row, 0,
            new QTableWidgetItem(QString("OFF")));
        mainContent.proceduresList->setItem(row, 1,
            new QTableWidgetItem(QString(procedures[row].name)));
        QFont offFont(mainContent.proceduresList->font());
        offFont.setPointSize(7);
        mainContent.proceduresList->item(row, 0)->setFont(offFont);
    }
    mainContent.proceduresList->setColumnWidth(0, 25);
    mainContent.proceduresList->horizontalHeader()->setStretchLastSection(true);
    
    /* as this method is called at the begginning, initialize buttons and bars */
    clearStatusBarColor();
    mainContent.tpLoaded->clear();
    mainContent.tpLaunchButton->setEnabled(false);
    return;
}

bool mainForm::LaunchAllIsChecked()
{
    return mainContent.tpLoadLaunchAllStop->isChecked();
}

bool mainForm::getPopUpBtnClickedYes()
{
    return popUpBtnClickedYes;
}

void mainForm::setResetGlobalVarsStatus(bool status)
{
    mainContent.resetGlobalVars->setEnabled(status);
    resetGlobalVarsStatus = status;
}
    
void mainForm::setMaximumTxStep(int maxValue) {emit maximumTxStep(maxValue);}
void mainForm::setMaximumInput(int maxValue) {emit maximumInput(maxValue);}
void mainForm::setMaximumRxStep(int maxValue) {emit maximumRxStep(maxValue);}
void mainForm::setMaximumOutput(int maxValue) {emit maximumOutput(maxValue);}
void mainForm::setValueTxStep(int value) {emit valueTxStep(value);}
void mainForm::setValueInput(int value) {emit valueInput(value);}
void mainForm::setValueRxStep(int value) {emit valueRxStep(value);}
void mainForm::setValueOutput(int value) {emit valueOutput(value);}
void mainForm::setVisibleTxStep(bool visible) {emit visibleTxStep(visible);}
void mainForm::setVisibleInput(bool visible) {emit visibleInput(visible);}
void mainForm::setVisibleRxStep(bool visible) {emit visibleRxStep(visible);}
void mainForm::setVisibleOutput(bool visible) {emit visibleOutput(visible);}

void mainForm::setEnabledControls(bool status, bool setMenu, bool setLaunch,
        bool setLogs)
{
    unsigned int numberOfProcedures = pGssStruct->getNumberOfProcedures();
    for(unsigned int row = 0; row < numberOfProcedures; ++row)
    {
        QTableWidgetItem * item = mainContent.proceduresList->item(row, 1);
        if(status)
            item->setFlags(item->flags() | Qt::ItemIsEnabled);
        else
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }
    if((status == true) && (pGssStruct->getCurrentTest() != -1))
    {
        mainContent.tpLoadButton->setEnabled(status);
        mainContent.tpLoadLaunchButton->setEnabled(status);
        mainContent.tpLoadLaunchAllButton->setEnabled(status);
    }
    else
    {
        mainContent.tpLoadButton->setEnabled(false);
        mainContent.tpLoadLaunchButton->setEnabled(false);
        mainContent.tpLoadLaunchAllButton->setEnabled(false);
    }
    mainContent.cancelButton->setEnabled(!status);
    mainContent.resetGlobalVars->setEnabled(status && resetGlobalVarsStatus);
    if(setMenu)
    {
        mainContent.menuBar->setEnabled(status);
    }
    if((setLaunch) && ((status == false) ||
            ((status == true) && (pGssStruct->getCurrentTest() != -1))))
    {
        mainContent.tpLaunchButton->setEnabled(status);
    }
    if(setLogs)
    {
        emit setEnabledLogPortsSignal(status);
        mainContent.logButton->setEnabled(status);
        mainContent.rawLogButton->setEnabled(status);
    }
}

void mainForm::setEnabledLaunch(bool status)
{
    if(pGssStruct->getCurrentTest() != -1)
        mainContent.tpLaunchButton->setEnabled(status);
}

void mainForm::setEnabledLaunchAllStop(bool status)
{
    mainContent.tpLoadLaunchAllStop->setEnabled(status);
}

void mainForm::setLoadLaunchAllItalic(bool status)
{
    QFont prFont(mainContent.tpLoadLaunchAllButton->font());
    prFont.setItalic(status);
    mainContent.tpLoadLaunchAllButton->setFont(prFont);
    mainContent.tpLoadLaunchAllStop->setChecked(status);
}
    
void mainForm::SelectTP(unsigned int index, bool status)
{
    QFont cFont(mainContent.proceduresList->item(index, 1)->font());
    cFont.setBold(status);
    cFont.setItalic(status);
    mainContent.proceduresList->item(index, 1)->setFont(cFont);
    if(status)
    {
        pGssStruct->setCurrentTest(index);
        mainContent.proceduresList->scrollToItem(mainContent.proceduresList->item(index, 1));
    }
}

void mainForm::setStatusBar(const QString &text)
{
    statusBarLabel->setText(text);
}

void mainForm::setStatusBarColor(const QString &text, int r, int g, int b)
{
    char color[40];
    snprintf(color, 40, "background-color: rgb(%d, %d, %d);", r, g, b);
    statusBarLabel->setStyleSheet(color);
    statusBarLabel->clear();
    statusBarLabel->setText(text);
}

void mainForm::clearStatusBarColor()
{
    statusBarLabel->setStyleSheet("background-color: rgb(240, 240, 240);");
    statusBarLabel->clear();
}

void mainForm::ShowAlarmMessageBox(const QString &text)
{
    QMessageBox::information(this, "Alarm",  text);
}

void mainForm::showLog(void)
{
    QPushButton * origin = qobject_cast<QPushButton *>(sender());
    if(origin == mainContent.logButton)
        pLogs->ShowLog(MAIN_LOG_IDX);
    else if(origin == mainContent.rawLogButton)
        pLogs->ShowLog(RAW_LOG_IDX);
    return;
}

void mainForm::EditConfigINI()
{
    pInitialConfig->ShowConfigFile(true);
}

void mainForm::EditConfigGSS()
{
    pInitialConfig->ShowConfigFile(false);
}

void mainForm::ChooseConfigGSS()
{
    if(SwitchConfigGSS() == 0)
    {
        resetGSS();
    }
}

int mainForm::SwitchConfigGSS()
{
    int status = 0;
    /* this function changes dir and file in gss_config.ini file */
    QFileDialog dialog;
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/nuts.ico"), QSize(), QIcon::Normal,
            QIcon::Off);
    dialog.setWindowIcon(icon);
    int strLen = InitialConfig::strMaxLen;
    char auxCharArray[strLen];
    snprintf(auxCharArray, strLen,
            "Choose MASSIVA config file (current: %s)",
            pInitialConfig->GetFile(XML_CONFIG_FILE));
    QString filenameString = dialog.getOpenFileName(this,
            QObject::tr(auxCharArray),
            QObject::tr(pInitialConfig->GetFile(WORKSPACE_LAST)),
            QObject::tr("XML/XMI files (*.xml *.xmi)"),
            0, QFileDialog::DontUseNativeDialog); 
    if(filenameString == NULL)
    {
        chdir(pInitialConfig->GetFile(GSS));
        return -1;
    }
    QByteArray filenameArray = filenameString.toLatin1();
    memset(auxCharArray, 0, strLen);
    strncpy(auxCharArray, filenameArray.constData(), strLen);
    if((status = pInitialConfig->UpdateConfigGSS(auxCharArray, strLen)) != 0)
    {
        QMessageBox::information(this, "Error", QObject::tr(auxCharArray));
    }
    return status;
}

void mainForm::setPeriodicalSpWTCText(const QString &text)
{
    mainContent.actionEnable_periodical_SpW_TC->setText(text);
}

void mainForm::showPopUp(const QString &title, const QString &text,
        PopUpType type)
{
    if(type == PopUpType::INFO)
    {
        QMessageBox::information(this, title, text);
    }
    else if(type == PopUpType::QUESTION)
    {
        popUpBtnClickedYes = false;
        if(QMessageBox::question(this, title, text,
                QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            popUpBtnClickedYes = true;
        }
    }
}

void mainForm::setRxOutput(unsigned int currentStepNumber,
        unsigned int numberOfOutputs, unsigned int outputsReceived)
{
    if(gssRxStep != (int)currentStepNumber)
    {
        valueRxStep(currentStepNumber+1);
        maximumOutput(numberOfOutputs);
        gssRxStep = (int)(currentStepNumber);
    }
    valueOutput(outputsReceived);
}

void mainForm::setActionsAtPort(unsigned int port, bool status)
{
    mainContent.menuSend_Steps->setEnabled(status);
    if(port == 0)//MainPort
    {
        mainContent.menuSend_Protocol_Packets->setEnabled(status);
    }
    if(mainContent.actionPeriodic_TCs->isEnabled() == true)
    {
        emit setEnabledActionAtPort(port, status);
    }
}
    
void mainForm::removeFormatTestProcedure(unsigned int test)
{
    QFont prFont(mainContent.proceduresList->item(test, 1)->font());
    prFont.setBold(false);
    prFont.setItalic(false);
    mainContent.proceduresList->item(test, 1)->setFont(prFont);
}
    
#ifdef PLOTS
void mainForm::replot(QwtPlot * pPlot)
{
    pPlot->replot();
}
#endif
    
bool mainForm::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == (QObject*)mainContent.proceduresList)
    {
        if (event->type() == QEvent::ToolTip)
        {
            const QPoint position = mainContent.proceduresList->mapFromGlobal(QCursor::pos());
            QTableWidgetItem *item = NULL;
            item = mainContent.proceduresList->itemAt(position);
            if((item != NULL) && (item->column() == 1))
                QToolTip::showText(QCursor::pos(), item->text());
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

void mainForm::closeEvent(QCloseEvent *event)
{
    char msg[MAX_MSG_SIZE];
    if(pTestMgr->getAnythingInProgress(msg, MAX_MSG_SIZE) == true)
    {
        QMessageBox::warning(this, "Warning!", tr(msg), QMessageBox::Ok);
        event->ignore();
    }
    else
    {
        emit closePlotForm();
        emit closePTCsDialog();
        emit closeAboutDialog();
        emit closeRGVDialog();
        pInitialConfig->SetCloseGss(true);
        event->accept();
    }
}
void mainForm::proceduresListClicked(int row, int column)
{
    int currentTest = pGssStruct->getCurrentTest();
    test_proc * procedures = pGssStruct->getPointerToProceduresList();
    if(column == 0)
    {
       /* if clicked OFF -> disable / enable test */
        if(procedures[row].enabled)
        {
            /* if it was enabled -> disable test */
            mainContent.proceduresList->item(row, 0)->setBackground(Qt::gray);
            mainContent.proceduresList->item(row, 1)->setBackground(Qt::gray);
            procedures[row].enabled = 0;
            if((currentTest == row) && (pTestMgr->getSessionStatus() != IN_PROGRESS))
            {
                /* remove bold and italic */
                removeFormatTestProcedure(currentTest);
                /* if disabled test was current test, disable buttons */
                pGssStruct->setCurrentTest(-1);
                mainContent.tpLoadButton->setEnabled(false);
                mainContent.tpLaunchButton->setEnabled(false);
                mainContent.tpLoadLaunchButton->setEnabled(false);
                mainContent.tpLoadLaunchAllButton->setEnabled(false);
                mainContent.tpLoadLaunchAllStop->setEnabled(false);
            }
        }
        else
        {
            /* if it was disabled -> enable test */
            mainContent.proceduresList->item(row, 0)->setBackground(Qt::NoBrush);
            mainContent.proceduresList->item(row, 1)->setBackground(Qt::NoBrush);
            procedures[row].enabled = 1;
        }
        QCoreApplication::processEvents();
    }
    else if((pTestMgr->getSessionStatus() != IN_PROGRESS) &&
            (pTestMgr->getParsingStatus() != IN_PROGRESS))
    {
        /* if clicked test name -> select as current test */
        if(procedures[row].enabled)
        {
            if(currentTest == -1)
            {
                /* if there were no previous test -> enable buttons */
                mainContent.tpLoadButton->setEnabled(true);
                mainContent.tpLoadLaunchButton->setEnabled(true);
                mainContent.tpLoadLaunchAllButton->setEnabled(true);
                mainContent.tpLoadLaunchAllStop->setEnabled(true);
            }
            else
            {
                /* if there were any previous test -> unselect it */
                SelectTP(currentTest, false);
            }
            SelectTP(row, true);
        }
    }
}

void mainForm::proceduresListClickedSlot(int row, int column)
{
    pGssStruct->currentTestMutexLock();
    proceduresListClicked(row, column);
    pGssStruct->currentTestMutexUnlock();
}

void mainForm::proceduresListDoubleClickedSlot(int row, int column)
{
    test_proc * procedures = pGssStruct->getPointerToProceduresList();
    if(column == 0)
    {
       /* if double clicked OFF -> toggle all OFF */
        pGssStruct->currentTestMutexLock();
        for(unsigned int tp = 0; tp<pGssStruct->getNumberOfProcedures(); ++tp)
        {
            proceduresListClicked(tp, 0);
        }
        pGssStruct->currentTestMutexUnlock();
    }
    else
    {
       /* if double clicked test name -> open test file */
        QString configFilenameStr(QString(pInitialConfig->GetFile(WORKSPACE)) + "/" +
                procedures[row].filename);
        QDesktopServices::openUrl(QUrl::fromLocalFile(configFilenameStr));
    }
}

void mainForm::resetGSS()
{
    pInitialConfig->SetCloseGss(false);
    QApplication::quit();
}

void mainForm::openCurrentLogFolder()
{
    pLogs->OpenLogFolder();
}

void mainForm::selectLogFolder()
{
    if(pLogs->UpdateLogFolder())
    {
        /* close currently open logs folder and open/create in new folder */
        pLogs->closeOpenLogs(false);
        unsigned int numberOfPorts = pGssStruct->getNumberOfPorts();
        const char * portNames[MAX_INTERFACES];
        for(unsigned int idx=0; idx<numberOfPorts; ++idx)
            portNames[idx] = pGssStruct->getPortName(idx);
        pLogs->openCreateLogs(numberOfPorts, portNames);
    }
}

void mainForm::openUserManual()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString(pInitialConfig->GetFile(MANUAL))));
}