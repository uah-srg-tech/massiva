/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   AboutDialog.cpp
 * Author: user
 *
 * Created on 13 de enero de 2020, 13:29
 */

#include "AboutDialog.h"

AboutDialog::AboutDialog(const QString & versionText)
{
    widget.setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    widget.version->setText(versionText);
}

void AboutDialog::showAboutDialog()
{
    if(this->isVisible())
        this->hide();
    this->show();
}

void AboutDialog::closeAboutDialog()
{
    this->hide();
}