/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AboutDialog.h
 * Author: user
 *
 * Created on 13 de enero de 2020, 13:29
 */

#ifndef _ABOUTDIALOG_H
#define _ABOUTDIALOG_H

#include <QString>
#include "ui_AboutDialog.h"


class AboutDialog : public QDialog {
    Q_OBJECT
public:
    AboutDialog(const QString & version);

public slots:
    void showAboutDialog();
    void closeAboutDialog();
    
private:
    Ui::AboutDialog widget;
};

#endif /* _ABOUTDIALOG_H */
