/********************************************************************************
** Form generated from reading UI file 'plotForm.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOTFORM_H
#define UI_PLOTFORM_H

#include <QtCore/QVariant>
#if QT_VERSION < 0x050000
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#else
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#endif

QT_BEGIN_NAMESPACE

class Ui_plotForm
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *plotsTab;
    QWidget *plotTab0;
    QHBoxLayout *horizontalLayout;

    void setupUi(QDialog *plotForm)
    {
        if (plotForm->objectName().isEmpty())
            plotForm->setObjectName(QString::fromUtf8("plotForm"));
        plotForm->resize(600, 430);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(plotForm->sizePolicy().hasHeightForWidth());
        plotForm->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/nuts.ico"), QSize(), QIcon::Normal, QIcon::Off);
        plotForm->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(plotForm);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        plotsTab = new QTabWidget(plotForm);
        plotsTab->setObjectName(QString::fromUtf8("plotsTab"));
        plotTab0 = new QWidget();
        plotTab0->setObjectName(QString::fromUtf8("plotTab0"));
        horizontalLayout = new QHBoxLayout(plotTab0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        plotsTab->addTab(plotTab0, QString());

        verticalLayout->addWidget(plotsTab);


        retranslateUi(plotForm);

        plotsTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(plotForm);
    } // setupUi

    void retranslateUi(QDialog *plotForm)
    {
        plotForm->setWindowTitle(QApplication::translate("plotForm", "plotForm", 0));
        plotsTab->setTabText(plotsTab->indexOf(plotTab0), QApplication::translate("plotForm", "0", 0));
    } // retranslateUi

};

namespace Ui {
    class plotForm: public Ui_plotForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOTFORM_H
