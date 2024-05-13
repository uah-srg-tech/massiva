/********************************************************************************
** Form generated from reading UI file 'resetGlobalVariables.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESETGLOBALVARIABLES_H
#define UI_RESETGLOBALVARIABLES_H

#include <QtCore/QVariant>
#if QT_VERSION < 0x050000
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#else
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDialog>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#endif

QT_BEGIN_NAMESPACE

class Ui_resetGlobalVariables
{
public:
    QPushButton *okButton;
    QCheckBox *resetAll;
    QSpinBox *globalVarIdx;
    QLineEdit *globalVar;

    void setupUi(QDialog *resetGlobalVariables)
    {
        if (resetGlobalVariables->objectName().isEmpty())
            resetGlobalVariables->setObjectName(QString::fromUtf8("resetGlobalVariables"));
        resetGlobalVariables->resize(339, 161);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(resetGlobalVariables->sizePolicy().hasHeightForWidth());
        resetGlobalVariables->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/nuts.ico"), QSize(), QIcon::Normal, QIcon::Off);
        resetGlobalVariables->setWindowIcon(icon);
        okButton = new QPushButton(resetGlobalVariables);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setGeometry(QRect(130, 130, 75, 23));
        resetAll = new QCheckBox(resetGlobalVariables);
        resetAll->setObjectName(QString::fromUtf8("resetAll"));
        resetAll->setGeometry(QRect(130, 30, 70, 17));
        globalVarIdx = new QSpinBox(resetGlobalVariables);
        globalVarIdx->setObjectName(QString::fromUtf8("globalVarIdx"));
        globalVarIdx->setEnabled(false);
        globalVarIdx->setGeometry(QRect(60, 70, 42, 22));
        globalVar = new QLineEdit(resetGlobalVariables);
        globalVar->setObjectName(QString::fromUtf8("globalVar"));
        globalVar->setEnabled(false);
        globalVar->setGeometry(QRect(110, 70, 221, 20));
        sizePolicy.setHeightForWidth(globalVar->sizePolicy().hasHeightForWidth());
        globalVar->setSizePolicy(sizePolicy);
        globalVar->setReadOnly(true);

        retranslateUi(resetGlobalVariables);

        QMetaObject::connectSlotsByName(resetGlobalVariables);
    } // setupUi

    void retranslateUi(QDialog *resetGlobalVariables)
    {
        resetGlobalVariables->setWindowTitle(QApplication::translate("resetGlobalVariables", "Reset Global Variable", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("resetGlobalVariables", "Ok", 0, QApplication::UnicodeUTF8));
        resetAll->setText(QApplication::translate("resetGlobalVariables", "Reset All", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class resetGlobalVariables: public Ui_resetGlobalVariables {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESETGLOBALVARIABLES_H
