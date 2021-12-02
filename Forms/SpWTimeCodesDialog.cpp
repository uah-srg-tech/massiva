/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SpWTimeCodesDialog.cpp
 * Author: user
 *
 * Created on 9 de julio de 2019, 10:50
 */

#include "SpWTimeCodesDialog.h"
#include "../PortTools/raw.h"

SpWTimeCodesDialog::SpWTimeCodesDialog(gssStructs * origGssStruct,
        Logs * origLogs, TestManager * origTestMgr, mainForm * origMainGui,
        unsigned int port)
{
    widget.setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pMainGui = origMainGui;
    pTestMgr = origTestMgr;
    spw_tc_port = port;
    pSpWTCPort = pGssStruct->getPointerToPort(spw_tc_port);
    
    tcValueBox = new HexSpinBox(this, 1);
    tcValueBox->setObjectName(QString::fromUtf8("tcValueBox"));
    tcValueBox->setGeometry(QRect(40, 50, 81, 21));
    tcValueBox->setValue(0);
    tcValueBox->setMinimum(0);
    tcValueBox->setMaximum(63, true);
    tcValueBox->stepBy(1);
    periodicalFreq = new QSpinBox(this);
    periodicalFreq->setObjectName(QString::fromUtf8("periodicalFreq"));
    periodicalFreq->setGeometry(QRect(40, 50, 51, 21));
    periodicalFreq->setValue(1);
    hertz = new QLabel(this);
    hertz->setObjectName(QString::fromUtf8("hertz"));
    hertz->setGeometry(QRect(95, 50, 26, 21));
    hertz->setText("hertz");
    spwTC_button = new QPushButton(this);
    spwTC_button->setObjectName(QString::fromUtf8("spwTC_button"));
    spwTC_button->setGeometry(QRect(130, 50, 130, 21));
    
    last_value = 0;
    last_freq_hertz = 1;
    dialog_is_single_spw_tc = true;
    
    connect(this->spwTC_button, SIGNAL(released()),
            this, SLOT(sendSingleOrEnablePeriodicalSpWTCs()));
    
    connect(this, SIGNAL(setStatusBarColor(const QString &, int, int, int)),
            pMainGui, SLOT (setStatusBarColor(const QString &, int, int, int)));
    connect(this, SIGNAL(setPeriodicalSpWTCText(const QString &)),
            pMainGui, SLOT (setPeriodicalSpWTCText(const QString &)));
}

SpWTimeCodesDialog::~SpWTimeCodesDialog()
{
    delete tcValueBox;
    delete periodicalFreq;
    delete spwTC_button;
}

void SpWTimeCodesDialog::ShowSendSingleSpWTCDialog()
{
    tcValueBox->setValue(last_value);
    tcValueBox->show();
    periodicalFreq->hide();
    hertz->hide();
    spwTC_button->setText(QApplication::translate("mainForm",
            "Send single TC", 0, QApplication::UnicodeUTF8));
    spwTC_button->show();
    dialog_is_single_spw_tc = true;
    this->show();
}

void SpWTimeCodesDialog::ShowEnableDisableSpWTCDialog()
{
    if(pTestMgr->getPeriodicalSpWTCStatus() == IN_PROGRESS)
    {
        int status = 0;
        char infoMsg[MAX_MSG_SIZE];
        if((status = PeriodicalTickIns(pSpWTCPort, 0, 0)) == 0)
        {
            emit setPeriodicalSpWTCText("Enable periodical SpW TC");
            snprintf(infoMsg, MAX_MSG_SIZE, "Periodical SpW TC disabled");
            pLogs->SetTimeInLog(MAIN_LOG_IDX, infoMsg, false);
            pLogs->SetTimeInLog(spw_tc_port, infoMsg, false);
            emit setStatusBarColor(QString(infoMsg), 240, 240, 240);
            pTestMgr->setPeriodicalSpWTCStatus(FINISHED);
        }
        else
        {
            snprintf(infoMsg, MAX_MSG_SIZE, 
                    "%s port SpW TC Error when disabling periodical TCs: ",
                    pSpWTCPort->name);
            RawRWError(status, &infoMsg[strlen(infoMsg)],
                    MAX_MSG_SIZE-strlen(infoMsg), pSpWTCPort);
            pLogs->SetTimeInLog(MAIN_LOG_IDX, infoMsg, false);
            pLogs->SetTimeInLog(spw_tc_port, infoMsg, false);
            emit setStatusBarColor(QString(infoMsg), 255, 0, 0);
        }
    }
    else
    {
        tcValueBox->hide();
        periodicalFreq->setValue(last_freq_hertz);
        periodicalFreq->show();
        hertz->show();
        spwTC_button->setText(QApplication::translate("mainForm",
                "Enable periodical TCs", 0, QApplication::UnicodeUTF8));
        spwTC_button->show();
        dialog_is_single_spw_tc = false;
        this->show();
    }
}

void SpWTimeCodesDialog::sendSingleOrEnablePeriodicalSpWTCs()
{
    this->hide();
    int status = 0;
    char infoMsg[MAX_MSG_SIZE];
    
    if(dialog_is_single_spw_tc)
    {
        if((status = SingleTickIn(pSpWTCPort, last_value, 1)) == 0)
        {
            snprintf(infoMsg, MAX_MSG_SIZE, "Sent single SpW TC value %d",
                    last_value);
            pLogs->SetTimeInLog(MAIN_LOG_IDX, infoMsg, false);
            pLogs->SetTimeInLog(RAW_LOG_IDX, infoMsg, false);
            pLogs->SetTimeInLog(spw_tc_port, infoMsg, false);
            emit setStatusBarColor(QString(infoMsg), 240, 240, 240);
            last_value++;
        }
        else
        {
            snprintf(infoMsg, MAX_MSG_SIZE,
                    "%s port SpW TC Error when enabling periodical TCs: ",
                    pSpWTCPort->name);
            RawRWError(status, &infoMsg[strlen(infoMsg)],
                    MAX_MSG_SIZE-strlen(infoMsg), pSpWTCPort);
            pLogs->SetTimeInLog(MAIN_LOG_IDX, infoMsg, false);
            pLogs->SetTimeInLog(RAW_LOG_IDX, infoMsg, false);
            pLogs->SetTimeInLog(spw_tc_port, infoMsg, false);
            emit setStatusBarColor(QString(infoMsg), 255, 0, 0);
        }
    }
    else
    {
        if((status = PeriodicalTickIns(pSpWTCPort, 1, last_freq_hertz)) == 0)
        {
            emit setPeriodicalSpWTCText("Disable periodical SpW TC");
            snprintf(infoMsg, MAX_MSG_SIZE, "Periodical SpW TC enabled at %d hertz",
                    last_freq_hertz);
            pLogs->SetTimeInLog(MAIN_LOG_IDX, infoMsg, false);
            pLogs->SetTimeInLog(RAW_LOG_IDX, infoMsg, false);
            pLogs->SetTimeInLog(spw_tc_port, infoMsg, false);
            emit setStatusBarColor(QString(infoMsg), 240, 240, 240);
            pTestMgr->setPeriodicalSpWTCStatus(IN_PROGRESS);
        }
        else
        {
            snprintf(infoMsg, MAX_MSG_SIZE,
                    "%s port SpW TC Error when enabling periodical SpW TCs: ",
                    pSpWTCPort->name);
            RawRWError(status, &infoMsg[strlen(infoMsg)],
                    MAX_MSG_SIZE-strlen(infoMsg), pSpWTCPort);
            pLogs->SetTimeInLog(MAIN_LOG_IDX, infoMsg, false);
            pLogs->SetTimeInLog(RAW_LOG_IDX, infoMsg, false);
            pLogs->SetTimeInLog(spw_tc_port, infoMsg, false);
            emit setStatusBarColor(QString(infoMsg), 255, 0, 0);
        }
    }
}