/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   selectRawFileForm.cpp
 * Author: Aaron
 *
 * Created on 13 de julio de 2017, 15:55
 */
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>
#include <mutex>
#include "selectRawFileForm.h"
#include "../TesterClasses/sendRaw.h"

selectRawFileForm::selectRawFileForm(gssStructs * origGssStruct,
        Logs * origLogs, InitialConfig * origInitialConfig,
        TestManager * origTestMgr, mainForm * origMainGui,
        RxTxInfo * origTxRxTabs, MonitorPlots * origMonitors)
{
    widget.setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    
    portAction = NULL;
    
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pInitialConfig = origInitialConfig;
    pTestMgr = origTestMgr;
    pMainGui = origMainGui;
    pMonitors = origMonitors;
    pTxRxTabs = origTxRxTabs;
    
    connect(widget.selectFileButton, SIGNAL(released()),
            this, SLOT(selectFile()));
    connect(widget.intervalBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateIntervalMs(int)));
    connect(widget.dicBox, SIGNAL(stateChanged(int)),
            this, SLOT(updateDIC(int)));
    connect(widget.gvBox, SIGNAL(stateChanged(int)),
            this, SLOT(updateGlobalVars(int)));
    connect(widget.loopBox, SIGNAL(stateChanged(int)),
            this, SLOT(updateLoop(int)));
    connect(widget.okButton, SIGNAL(released()), this, SLOT(processSendRaw()));
    connect(widget.cancelButton, SIGNAL(released()), this, SLOT(reject()));
    
    connect(this, SIGNAL(setPeriodicalSpWTCText(const QString &)),
            pMainGui, SLOT (setPeriodicalSpWTCText(const QString &)));
    
    connect(this, SIGNAL(setActionsAtPort(unsigned int, bool)),
            pMainGui, SLOT (setActionsAtPort(unsigned int, bool)));
}

void selectRawFileForm::createSendRawMenu()
{
    portAction = new QAction*[pGssStruct->getNumberOfPorts()];
    
    for(unsigned int port=0; port<pGssStruct->getNumberOfPorts(); ++port)
    {
        portAction[port] = new QAction(pMainGui->mainContent.menuSend_Raw_Commands);
        portAction[port]->setObjectName(QString::fromUtf8(pGssStruct->getPortName(port)));
        pMainGui->mainContent.menuSend_Raw_Commands->addAction(portAction[port]);
        portAction[port]->setText(pGssStruct->getPortName(port));
        connect(portAction[port], SIGNAL(triggered()), this, SLOT(selectRawMenu()));
    }
}

void selectRawFileForm::destroySendRawMenu()
{
    if(portAction != NULL)
    {
        for(unsigned int port=0; port<pGssStruct->getNumberOfPorts(); ++port)
        {
            pMainGui->mainContent.menuSend_Raw_Commands->removeAction(portAction[port]);
            delete portAction[port];
        }
        delete [] portAction;
        portAction = NULL;
    }
}

void selectRawFileForm::selectRawMenu()
{
    QAction * action = qobject_cast<QAction *>(sender());
    for(port=0; port<pGssStruct->getNumberOfPorts(); ++port)
    {
        if(action == portAction[port])
            break;
    }
    
    loop = false;
    intervalMs = 50;
    DIC = true;
    GlobalVars = true;
    widget.fileLineEdit->clear();
    widget.commandsCount->display(0);
    widget.intervalBox->setEnabled(true);
    widget.intervalBox->setValue(50);
    if(pGssStruct->getPortType(port) == UART_PORT)
    {
        widget.dicBox->setChecked(false);
        widget.gvBox->setChecked(false);
        widget.loopBox->setChecked(true);
    }
    else
    {
        widget.dicBox->setChecked(true);
        widget.gvBox->setChecked(true);
        widget.loopBox->setChecked(false);
    }
    widget.okButton->setEnabled(false);
    memset(commandFilename, 0, strMaxLen);
    this->setWindowTitle(QString("Send Raw Command via ") +
            QString(pGssStruct->getPortName(port)));
    this->exec();
}

void selectRawFileForm::selectFile()
{
    widget.fileLineEdit->setEnabled(false);
    widget.selectFileButton->setEnabled(false);
    widget.loopBox->setEnabled(false);
    widget.intervalBox->setEnabled(false);
    widget.dicBox->setEnabled(false);
    widget.gvBox->setEnabled(false);
    widget.okButton->setEnabled(false);
    widget.cancelButton->setEnabled(false);
    
    QString filenameString = QFileDialog::getOpenFileName(this,
        tr("Choose a file to get data"), tr(pInitialConfig->GetFile(GSS_LAST)),
        tr("text (*.txt *.dat)"));
    if(filenameString.isNull())
    {
        if(commandFilename[0] == 0)
            widget.okButton->setEnabled(false);
        else
            widget.okButton->setEnabled(true);
        widget.fileLineEdit->setEnabled(true);
        widget.selectFileButton->setEnabled(true);
        widget.loopBox->setEnabled(true);
        widget.intervalBox->setEnabled(true);
        widget.dicBox->setEnabled(true);
        widget.gvBox->setEnabled(true);
        widget.cancelButton->setEnabled(true);
        return;
    }

    QByteArray array = filenameString.toLatin1 ();
    strncpy(commandFilename, array.constData(), strMaxLen);
    pInitialConfig->SetFile(GSS_LAST, commandFilename);
    
    FILE * fp;
    int aux = 0, aux_prev = 0;
    unsigned int commands = 0, lines = 0, line_len = 0;
    fp = fopen(commandFilename, "r");
    do {
        aux_prev = aux;
        aux = fgetc(fp);
        if((aux == EOF) || (aux == '\n'))
        {
            lines++;
            if(line_len != 0)
            {
                commands++;
                line_len = 0;
            }
        }
        else if(aux == '#')
        {
            do {
                aux = fgetc(fp);
            } while((aux != EOF) && (aux != '\n'));
            lines++;
            if(line_len != 0)
                commands++;
            line_len = 0;
        }
        else if((aux == '0') && (aux_prev == '\n'))
        {
            aux = fgetc(fp);
            if(aux == '0')
            do {
                aux = fgetc(fp);
            } while((aux != EOF) && (aux != '\n'));
            lines++;
            commands++;
            line_len = 0;
        }
        else
        {
            if((aux < '0') || ((aux > '9') && (aux < 'A')) ||
                    ((aux > 'F') && (aux < 'a')) || (aux > 'f'))
            {
                char msg[MAX_MSG_SIZE];
                snprintf(msg, MAX_MSG_SIZE, "Line %d: Not valid number %c (0-9, a-f)",
                        lines+1, aux);
                QMessageBox::warning(this, "File command not valid", tr(msg));
                break;
            }
            line_len++;
            if(line_len > (MAX_STR_OPERAND_LEN*2))
            {
                char msg[MAX_MSG_SIZE];
                snprintf(msg, MAX_MSG_SIZE, "Line %d: Command too long (%d > %d)",
                        lines+1, line_len, MAX_STR_OPERAND_LEN*2);
                QMessageBox::warning(this, "File command not valid", tr(msg));
                break;
            }
        }
    } while (aux != EOF);
    fclose(fp);
    
    if(commands == 0)
    {
        QMessageBox::warning(this, "Warning", tr("No commands found"));
    }
    else
    {
        widget.fileLineEdit->setText(tr(commandFilename));
        //get number of digits of "commands"
        int digits = 0, number = commands;
        while (number) {
            number /= 10;
            digits++;
        }
        if(digits > 5)
            widget.commandsCount->setDigitCount(digits);
        else
            widget.commandsCount->setDigitCount(5);
        widget.commandsCount->display((int)commands);
        widget.okButton->setEnabled(true);
    }
    widget.fileLineEdit->setEnabled(true);
    widget.selectFileButton->setEnabled(true);
    widget.loopBox->setEnabled(true);
    widget.intervalBox->setEnabled(true);
    widget.dicBox->setEnabled(true);
    widget.gvBox->setEnabled(true);
    widget.cancelButton->setEnabled(true);
}

void selectRawFileForm::updateIntervalMs(int newPeriodMs)
{
    intervalMs = (unsigned int)newPeriodMs;
}

void selectRawFileForm::updateLoop(int newState)
{
    if(newState == Qt::Unchecked)
        loop = false;
    else loop = true;
    widget.intervalBox->setEnabled(loop);
}

void selectRawFileForm::updateDIC(int newState)
{
    if(newState == Qt::Unchecked)
        DIC = false;
    else DIC = true;
}

void selectRawFileForm::updateGlobalVars(int newState)
{
    if(newState == Qt::Unchecked)
        GlobalVars = false;
    else
        GlobalVars = true;
}

void selectRawFileForm::setSendRawMenuEnable(unsigned int selPort, bool newState)
{
    portAction[selPort]->setEnabled(newState);
    emit setActionsAtPort(selPort, newState);
}

void selectRawFileForm::processSendRaw()
{      
    sendRaw * worker = new sendRaw(pGssStruct, pLogs, pTestMgr, pMainGui,
            pMonitors, this, port, commandFilename, loop, intervalMs, DIC,
            GlobalVars);
    QThread * workerThread = new QThread();
    worker->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), worker, SLOT(send()));
    connect(workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    
    connect(worker, SIGNAL(TxDataInc(unsigned int)), 
            pTxRxTabs, SLOT(TxDataInc(unsigned int)), Qt::BlockingQueuedConnection);
    connect(worker, SIGNAL(TxDataPrintPacket(unsigned int, const unsigned char *, int, unsigned)), 
            pTxRxTabs, SLOT(TxDataPrintPacket(unsigned int, const unsigned char *, int, unsigned)),
            Qt::BlockingQueuedConnection);
    setSendRawMenuEnable(port, false);
    pMainGui->setEnabledControls(false, false, true, false);
    
    workerThread->start();
    this->close();
}

void selectRawFileForm::modifyRawPeriodicalTC(unsigned char value)
{
    /* periodical tcs, 0xFE enable, 0xFF disable */
    if(value == 0xFE)
    {
        pTestMgr->setPeriodicalSpWTCStatus(IN_PROGRESS);
        emit setPeriodicalSpWTCText("Disable periodical SpW TC");
    }
    else if(value == 0xFF)
    {
        pTestMgr->setPeriodicalSpWTCStatus(FINISHED);
        emit setPeriodicalSpWTCText("Enable periodical SpW TC");
    }
}