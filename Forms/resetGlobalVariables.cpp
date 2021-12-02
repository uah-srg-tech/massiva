/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   resetGlobalVariables.cpp
 * Author: Aaron
 *
 * Created on 18 de marzo de 2020, 18:33
 */

#include "resetGlobalVariables.h"
#include <string.h>

resetGlobalVariables::resetGlobalVariables(gssStructs * origGssStruct, Logs * origLogs)
{
    widget.setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    
    connect(widget.resetAll, SIGNAL(stateChanged(int)),
            this, SLOT(enableDisableSingleGlobalBV(int)));
    connect(widget.globalVarIdx, SIGNAL(valueChanged(int)),
            this, SLOT(changeGlobalBVName(int)));
    connect(widget.okButton, SIGNAL(released()),
            this, SLOT(resetGlobalBV()));
}

void resetGlobalVariables::setNumberOfGlobalVars()
{
    widget.resetAll->setCheckState(Qt::CheckState::Checked);
    widget.globalVarIdx->setMaximum(pGssStruct->getNumberOfGlobalVars()-1);
    widget.globalVarIdx->setValue(0);
    widget.globalVar->setText(QString(pGssStruct->getGlobalVarName(0)));
}

void resetGlobalVariables::showRGVDialog()
{
    if(this->isVisible())
        this->hide();
    this->show();
}

void resetGlobalVariables::closeRGVDialog()
{
    this->hide();
}

void resetGlobalVariables::enableDisableSingleGlobalBV(int newStatus)
{
    bool singleStatus = true;
    if(newStatus == Qt::CheckState::Checked)
        singleStatus = false;
    widget.globalVarIdx->setEnabled(singleStatus);
    widget.globalVar->setEnabled(singleStatus);
}

void resetGlobalVariables::changeGlobalBVName(int newIndex)
{
    widget.globalVar->setText(QString(pGssStruct->getGlobalVarName(newIndex)));
}

void resetGlobalVariables::resetGlobalBV()
{
    std::string message;
    if(widget.resetAll->checkState() == Qt::CheckState::Checked)
    {
        pGssStruct->resetGlobalVarsValues();
        message = "Reset global variables";   
    }
    else
    {
        pGssStruct->resetGlobalVarValue(widget.globalVarIdx->value());
        message = "Reset global variable " +
                std::to_string(widget.globalVarIdx->value()) + " ("
                + pGssStruct->getGlobalVarName(widget.globalVarIdx->value()) + ")";
    }
    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), message.c_str(),
            true, false, false);
    this->hide();
}
