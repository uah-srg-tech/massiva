/********************************************************************************
** Form generated from reading UI file 'AboutDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#if QT_VERSION < 0x050000
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#else
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QHeaderView>
#include <QLabel>
#endif

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QLabel *version;
    QLabel *logoUAH;
    QLabel *logoSRG;
    QLabel *logoGSS;
    QLabel *acronym;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName(QString::fromUtf8("AboutDialog"));
        AboutDialog->resize(400, 300);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/nuts.ico"), QSize(), QIcon::Normal, QIcon::Off);
        AboutDialog->setWindowIcon(icon);
        version = new QLabel(AboutDialog);
        version->setObjectName(QString::fromUtf8("version"));
        version->setGeometry(QRect(180, 20, 200, 31));
        QFont font;
        font.setPointSize(10);
        version->setFont(font);
        logoUAH = new QLabel(AboutDialog);
        logoUAH->setObjectName(QString::fromUtf8("logoUAH"));
        logoUAH->setGeometry(QRect(90, 210, 219, 64));
        QFont font1;
        font1.setPointSize(15);
        logoUAH->setFont(font1);
        logoUAH->setPixmap(QPixmap(QString::fromUtf8(":/main/logoAlcalaHoriz.png")));
        logoUAH->setScaledContents(true);
        logoSRG = new QLabel(AboutDialog);
        logoSRG->setObjectName(QString::fromUtf8("logoSRG"));
        logoSRG->setGeometry(QRect(30, 210, 60, 64));
        logoSRG->setFont(font1);
        logoSRG->setPixmap(QPixmap(QString::fromUtf8(":/main/logoSRG.png")));
        logoSRG->setScaledContents(true);
        logoGSS = new QLabel(AboutDialog);
        logoGSS->setObjectName(QString::fromUtf8("logoGSS"));
        logoGSS->setGeometry(QRect(20, 20, 150, 150));
        logoGSS->setFont(font1);
        logoGSS->setPixmap(QPixmap(QString::fromUtf8(":/main/nuts.png")));
        logoGSS->setScaledContents(true);
        acronym = new QLabel(AboutDialog);
        acronym->setObjectName(QString::fromUtf8("acronym"));
        acronym->setGeometry(QRect(180, 50, 200, 131));
        acronym->setFont(font);
        acronym->setTextFormat(Qt::RichText);

        retranslateUi(AboutDialog);

        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QApplication::translate("AboutDialog", "About", 0, QApplication::UnicodeUTF8));
        version->setText(QApplication::translate("AboutDialog", "MASSIVA version", 0, QApplication::UnicodeUTF8));
        logoUAH->setText(QString());
        logoSRG->setText(QString());
        logoGSS->setText(QString());
        acronym->setText(QApplication::translate("AboutDialog", "Monitoring and<br/>Analysis<br/>System for<br/>Software<br/>Inspection,<br/>Verification/Validation and<br/>Assessment", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
