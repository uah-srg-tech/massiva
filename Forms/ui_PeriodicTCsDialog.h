/********************************************************************************
** Form generated from reading UI file 'PeriodicTCsDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PERIODICTCSDIALOG_H
#define UI_PERIODICTCSDIALOG_H

#include <QtCore/QVariant>
#if QT_VERSION < 0x050000
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#else
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QGridLayout>
#include <QHeaderView>
#endif

QT_BEGIN_NAMESPACE

class Ui_PeriodicTCsDialog
{
public:
    QGridLayout *gridLayout;

    void setupUi(QDialog *PeriodicTCsDialog)
    {
        if (PeriodicTCsDialog->objectName().isEmpty())
            PeriodicTCsDialog->setObjectName(QString::fromUtf8("PeriodicTCsDialog"));
        PeriodicTCsDialog->resize(300, 36);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/nuts.ico"), QSize(), QIcon::Normal, QIcon::Off);
        PeriodicTCsDialog->setWindowIcon(icon);
        gridLayout = new QGridLayout(PeriodicTCsDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        retranslateUi(PeriodicTCsDialog);

        QMetaObject::connectSlotsByName(PeriodicTCsDialog);
    } // setupUi

    void retranslateUi(QDialog *PeriodicTCsDialog)
    {
        PeriodicTCsDialog->setWindowTitle(QApplication::translate("PeriodicTCsDialog", "Periodic Telecommands", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PeriodicTCsDialog: public Ui_PeriodicTCsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PERIODICTCSDIALOG_H
