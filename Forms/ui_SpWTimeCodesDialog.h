/********************************************************************************
** Form generated from reading UI file 'SpWTimeCodesDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPWTIMECODESDIALOG_H
#define UI_SPWTIMECODESDIALOG_H

#include <QtCore/QVariant>
#if QT_VERSION < 0x050000
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#else
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QHeaderView>
#endif

QT_BEGIN_NAMESPACE

class Ui_SpWTimeCodesDialog
{
public:

    void setupUi(QDialog *SpWTimeCodesDialog)
    {
        if (SpWTimeCodesDialog->objectName().isEmpty())
            SpWTimeCodesDialog->setObjectName(QString::fromUtf8("SpWTimeCodesDialog"));
        SpWTimeCodesDialog->resize(299, 146);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SpWTimeCodesDialog->sizePolicy().hasHeightForWidth());
        SpWTimeCodesDialog->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/nuts.ico"), QSize(), QIcon::Normal, QIcon::Off);
        SpWTimeCodesDialog->setWindowIcon(icon);

        retranslateUi(SpWTimeCodesDialog);

        QMetaObject::connectSlotsByName(SpWTimeCodesDialog);
    } // setupUi

    void retranslateUi(QDialog *SpWTimeCodesDialog)
    {
        SpWTimeCodesDialog->setWindowTitle(QApplication::translate("SpWTimeCodesDialog", "SpW Timecodes", 0));
    } // retranslateUi

};

namespace Ui {
    class SpWTimeCodesDialog: public Ui_SpWTimeCodesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPWTIMECODESDIALOG_H
