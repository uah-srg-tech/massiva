/********************************************************************************
** Form generated from reading UI file 'mainForm.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINFORM_H
#define UI_MAINFORM_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#if QT_VERSION < 0x050000
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#else
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#endif

QT_BEGIN_NAMESPACE

class Ui_mainForm
{
public:
    QAction *actionEdit_current_GSS_config_file;
    QAction *actionSend_one_step;
    QAction *actionSend_several_steps;
    QAction *actionReset_GSS;
    QAction *actionSwitch_GSS_config_file;
    QAction *actionUserManual;
    QAction *actionAbout;
    QAction *actionPeriodic_TCs;
    QAction *actionPlots;
    QAction *actionSend_single_SpW_TC;
    QAction *actionEnable_periodical_SpW_TC;
    QAction *actionOpen_Logs_folder;
    QAction *actionSelect_Logs_folder;
    QWidget *content;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QProgressBar *barTxStep;
    QProgressBar *barInput;
    QProgressBar *barRxStep;
    QProgressBar *barOutput;
    QLineEdit *tpLoaded;
    QVBoxLayout *logButtons;
    QPushButton *resetGlobalVars;
    QPushButton *logButton;
    QPushButton *rawLogButton;
    QGridLayout *specialLayout;
    QCheckBox *specialPrint_4;
    QLCDNumber *specialCount_12;
    QLCDNumber *specialCount_5;
    QLabel *specialName_7;
    QCheckBox *specialEnabled_3;
    QCheckBox *specialEnabled_11;
    QLCDNumber *specialPeriodCount_5;
    QLCDNumber *specialCount_4;
    QLCDNumber *specialPeriodCount_3;
    QLCDNumber *specialPeriodCount_0;
    QLabel *specialName_0;
    QCheckBox *specialEnabled_10;
    QCheckBox *specialPrint_1;
    QCheckBox *specialEnabled_1;
    QLCDNumber *specialCount_2;
    QLabel *specialName_1;
    QCheckBox *specialEnabled_0;
    QCheckBox *specialEnabled_4;
    QLCDNumber *specialPeriodCount_7;
    QCheckBox *specialEnabled_2;
    QLCDNumber *specialPeriodCount_4;
    QLCDNumber *specialCount_1;
    QCheckBox *specialEnabled_8;
    QLabel *specialName_8;
    QLCDNumber *specialCount_8;
    QCheckBox *specialPrint_8;
    QCheckBox *specialPrint_7;
    QLabel *specialName_12;
    QCheckBox *specialEnabled_12;
    QLCDNumber *specialPeriodCount_11;
    QLabel *specialName_11;
    QLCDNumber *specialCount_13;
    QLabel *specialName_13;
    QCheckBox *specialEnabled_13;
    QLCDNumber *specialCount_11;
    QCheckBox *specialPrint_12;
    QLCDNumber *specialPeriodCount_12;
    QCheckBox *specialPrint_13;
    QLabel *specialName_4;
    QCheckBox *specialPrint_11;
    QCheckBox *specialPrint_3;
    QLCDNumber *specialCount_3;
    QLCDNumber *specialCount_0;
    QLabel *specialName_3;
    QCheckBox *specialPrint_9;
    QLCDNumber *specialCount_10;
    QLabel *specialName_2;
    QCheckBox *specialEnabled_6;
    QCheckBox *specialPrint_5;
    QLCDNumber *specialPeriodCount_2;
    QCheckBox *specialPrint_2;
    QCheckBox *specialEnabled_5;
    QLCDNumber *specialCount_6;
    QLabel *specialName_5;
    QLabel *specialName_6;
    QLCDNumber *specialCount_7;
    QCheckBox *specialPrint_14;
    QLCDNumber *specialPeriodCount_6;
    QCheckBox *specialPrint_6;
    QCheckBox *specialEnabled_7;
    QCheckBox *specialEnabled_9;
    QLabel *specialName_14;
    QLCDNumber *specialPeriodCount_8;
    QLCDNumber *specialCount_14;
    QLabel *specialName_9;
    QLCDNumber *specialPeriodCount_13;
    QLCDNumber *specialPeriodCount_9;
    QLabel *specialName_10;
    QLCDNumber *specialPeriodCount_10;
    QCheckBox *specialPrint_10;
    QLCDNumber *specialCount_9;
    QCheckBox *specialPrint_0;
    QLCDNumber *specialPeriodCount_1;
    QLabel *specialName_15;
    QLCDNumber *specialPeriodCount_15;
    QCheckBox *specialEnabled_14;
    QLCDNumber *specialPeriodCount_14;
    QLCDNumber *specialCount_15;
    QLabel *specialName_16;
    QCheckBox *specialEnabled_15;
    QLCDNumber *specialCount_16;
    QCheckBox *specialPrint_15;
    QLCDNumber *specialPeriodCount_16;
    QCheckBox *specialPrint_16;
    QCheckBox *specialEnabled_16;
    QWidget *widget_2;
    QGridLayout *alarmValLayout;
    QLCDNumber *alarmVal0;
    QLCDNumber *alarmVal2;
    QLabel *alarmValName_2;
    QLabel *alarmValName_1;
    QLCDNumber *alarmVal1;
    QLabel *alarmValName_0;
    QWidget *alarmWidget;
    QTabWidget *portsTab;
    QWidget *portsTab1;
    QGridLayout *gridLayout_2;
    QTabWidget *specialFieldsTab;
    QWidget *specialFieldsTab1;
    QGridLayout *gridLayout_3;
    QTableWidget *proceduresList;
    QVBoxLayout *buttonsLayout;
    QPushButton *tpLoadButton;
    QPushButton *tpLaunchButton;
    QPushButton *tpLoadLaunchButton;
    QHBoxLayout *LoadLaunchAllLayout;
    QPushButton *tpLoadLaunchAllButton;
    QCheckBox *tpLoadLaunchAllStop;
    QPushButton *cancelButton;
    QStatusBar *statusbar;
    QMenuBar *menuBar;
    QMenu *menuGSS;
    QMenu *menuLogs_folder;
    QMenu *menuSend_Raw_Commands;
    QMenu *menuSend_Protocol_Packets;
    QMenu *menuSend_Steps;
    QMenu *menuHelp;
    QMenu *menuView;

    void setupUi(QMainWindow *mainForm)
    {
        if (mainForm->objectName().isEmpty())
            mainForm->setObjectName(QString::fromUtf8("mainForm"));
        mainForm->resize(1024, 576);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mainForm->sizePolicy().hasHeightForWidth());
        mainForm->setSizePolicy(sizePolicy);
        mainForm->setMinimumSize(QSize(1024, 576));
        mainForm->setMaximumSize(QSize(16777215, 16777215));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/nuts.ico"), QSize(), QIcon::Normal, QIcon::Off);
        mainForm->setWindowIcon(icon);
        mainForm->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        actionEdit_current_GSS_config_file = new QAction(mainForm);
        actionEdit_current_GSS_config_file->setObjectName(QString::fromUtf8("actionEdit_current_GSS_config_file"));
        actionSend_one_step = new QAction(mainForm);
        actionSend_one_step->setObjectName(QString::fromUtf8("actionSend_one_step"));
        actionSend_several_steps = new QAction(mainForm);
        actionSend_several_steps->setObjectName(QString::fromUtf8("actionSend_several_steps"));
        actionReset_GSS = new QAction(mainForm);
        actionReset_GSS->setObjectName(QString::fromUtf8("actionReset_GSS"));
        actionSwitch_GSS_config_file = new QAction(mainForm);
        actionSwitch_GSS_config_file->setObjectName(QString::fromUtf8("actionSwitch_GSS_config_file"));
        actionUserManual = new QAction(mainForm);
        actionUserManual->setObjectName(QString::fromUtf8("actionUserManual"));
        actionAbout = new QAction(mainForm);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionPeriodic_TCs = new QAction(mainForm);
        actionPeriodic_TCs->setObjectName(QString::fromUtf8("actionPeriodic_TCs"));
        actionPlots = new QAction(mainForm);
        actionPlots->setObjectName(QString::fromUtf8("actionPlots"));
        actionSend_single_SpW_TC = new QAction(mainForm);
        actionSend_single_SpW_TC->setObjectName(QString::fromUtf8("actionSend_single_SpW_TC"));
        actionEnable_periodical_SpW_TC = new QAction(mainForm);
        actionEnable_periodical_SpW_TC->setObjectName(QString::fromUtf8("actionEnable_periodical_SpW_TC"));
        actionOpen_Logs_folder = new QAction(mainForm);
        actionOpen_Logs_folder->setObjectName(QString::fromUtf8("actionOpen_Logs_folder"));
        actionSelect_Logs_folder = new QAction(mainForm);
        actionSelect_Logs_folder->setObjectName(QString::fromUtf8("actionSelect_Logs_folder"));
        content = new QWidget(mainForm);
        content->setObjectName(QString::fromUtf8("content"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(content->sizePolicy().hasHeightForWidth());
        content->setSizePolicy(sizePolicy1);
        content->setMaximumSize(QSize(16777215, 16777215));
        gridLayout = new QGridLayout(content);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        barTxStep = new QProgressBar(content);
        barTxStep->setObjectName(QString::fromUtf8("barTxStep"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(barTxStep->sizePolicy().hasHeightForWidth());
        barTxStep->setSizePolicy(sizePolicy2);
        barTxStep->setValue(0);

        verticalLayout->addWidget(barTxStep);

        barInput = new QProgressBar(content);
        barInput->setObjectName(QString::fromUtf8("barInput"));
        sizePolicy2.setHeightForWidth(barInput->sizePolicy().hasHeightForWidth());
        barInput->setSizePolicy(sizePolicy2);
        barInput->setValue(0);

        verticalLayout->addWidget(barInput);

        barRxStep = new QProgressBar(content);
        barRxStep->setObjectName(QString::fromUtf8("barRxStep"));
        sizePolicy2.setHeightForWidth(barRxStep->sizePolicy().hasHeightForWidth());
        barRxStep->setSizePolicy(sizePolicy2);
        barRxStep->setValue(0);

        verticalLayout->addWidget(barRxStep);

        barOutput = new QProgressBar(content);
        barOutput->setObjectName(QString::fromUtf8("barOutput"));
        sizePolicy2.setHeightForWidth(barOutput->sizePolicy().hasHeightForWidth());
        barOutput->setSizePolicy(sizePolicy2);
        barOutput->setValue(0);

        verticalLayout->addWidget(barOutput);


        gridLayout->addLayout(verticalLayout, 3, 0, 1, 1);

        tpLoaded = new QLineEdit(content);
        tpLoaded->setObjectName(QString::fromUtf8("tpLoaded"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(tpLoaded->sizePolicy().hasHeightForWidth());
        tpLoaded->setSizePolicy(sizePolicy3);
        tpLoaded->setDragEnabled(false);
        tpLoaded->setReadOnly(true);

        gridLayout->addWidget(tpLoaded, 2, 0, 1, 3);

        logButtons = new QVBoxLayout();
        logButtons->setObjectName(QString::fromUtf8("logButtons"));
        resetGlobalVars = new QPushButton(content);
        resetGlobalVars->setObjectName(QString::fromUtf8("resetGlobalVars"));
        sizePolicy1.setHeightForWidth(resetGlobalVars->sizePolicy().hasHeightForWidth());
        resetGlobalVars->setSizePolicy(sizePolicy1);
        QFont font;
        font.setPointSize(7);
        resetGlobalVars->setFont(font);

        logButtons->addWidget(resetGlobalVars);

        logButton = new QPushButton(content);
        logButton->setObjectName(QString::fromUtf8("logButton"));
        sizePolicy1.setHeightForWidth(logButton->sizePolicy().hasHeightForWidth());
        logButton->setSizePolicy(sizePolicy1);

        logButtons->addWidget(logButton);

        rawLogButton = new QPushButton(content);
        rawLogButton->setObjectName(QString::fromUtf8("rawLogButton"));
        sizePolicy1.setHeightForWidth(rawLogButton->sizePolicy().hasHeightForWidth());
        rawLogButton->setSizePolicy(sizePolicy1);

        logButtons->addWidget(rawLogButton);


        gridLayout->addLayout(logButtons, 3, 2, 1, 1);

        specialLayout = new QGridLayout();
        specialLayout->setObjectName(QString::fromUtf8("specialLayout"));
        specialLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        specialPrint_4 = new QCheckBox(content);
        specialPrint_4->setObjectName(QString::fromUtf8("specialPrint_4"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(specialPrint_4->sizePolicy().hasHeightForWidth());
        specialPrint_4->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_4, 4, 3, 1, 1);

        specialCount_12 = new QLCDNumber(content);
        specialCount_12->setObjectName(QString::fromUtf8("specialCount_12"));
        sizePolicy3.setHeightForWidth(specialCount_12->sizePolicy().hasHeightForWidth());
        specialCount_12->setSizePolicy(sizePolicy3);
        specialCount_12->setMaximumSize(QSize(50, 16));
        specialCount_12->setFrameShadow(QFrame::Plain);
        specialCount_12->setSmallDecimalPoint(true);
        specialCount_12->setNumDigits(6);
        specialCount_12->setMode(QLCDNumber::Dec);
        specialCount_12->setSegmentStyle(QLCDNumber::Flat);
        specialCount_12->setProperty("value", QVariant(0));
        specialCount_12->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_12, 12, 1, 1, 1);

        specialCount_5 = new QLCDNumber(content);
        specialCount_5->setObjectName(QString::fromUtf8("specialCount_5"));
        sizePolicy3.setHeightForWidth(specialCount_5->sizePolicy().hasHeightForWidth());
        specialCount_5->setSizePolicy(sizePolicy3);
        specialCount_5->setMaximumSize(QSize(50, 16));
        specialCount_5->setFrameShadow(QFrame::Plain);
        specialCount_5->setSmallDecimalPoint(true);
        specialCount_5->setNumDigits(6);
        specialCount_5->setMode(QLCDNumber::Dec);
        specialCount_5->setSegmentStyle(QLCDNumber::Flat);
        specialCount_5->setProperty("value", QVariant(0));
        specialCount_5->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_5, 5, 1, 1, 1);

        specialName_7 = new QLabel(content);
        specialName_7->setObjectName(QString::fromUtf8("specialName_7"));
        sizePolicy3.setHeightForWidth(specialName_7->sizePolicy().hasHeightForWidth());
        specialName_7->setSizePolicy(sizePolicy3);
        specialName_7->setMinimumSize(QSize(100, 0));
        specialName_7->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_7, 7, 0, 1, 1);

        specialEnabled_3 = new QCheckBox(content);
        specialEnabled_3->setObjectName(QString::fromUtf8("specialEnabled_3"));
        sizePolicy4.setHeightForWidth(specialEnabled_3->sizePolicy().hasHeightForWidth());
        specialEnabled_3->setSizePolicy(sizePolicy4);
        specialEnabled_3->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_3, 3, 2, 1, 1);

        specialEnabled_11 = new QCheckBox(content);
        specialEnabled_11->setObjectName(QString::fromUtf8("specialEnabled_11"));
        sizePolicy4.setHeightForWidth(specialEnabled_11->sizePolicy().hasHeightForWidth());
        specialEnabled_11->setSizePolicy(sizePolicy4);
        specialEnabled_11->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_11, 11, 2, 1, 1);

        specialPeriodCount_5 = new QLCDNumber(content);
        specialPeriodCount_5->setObjectName(QString::fromUtf8("specialPeriodCount_5"));
        specialPeriodCount_5->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_5->sizePolicy().hasHeightForWidth());
        specialPeriodCount_5->setSizePolicy(sizePolicy3);
        specialPeriodCount_5->setMaximumSize(QSize(40, 16));
        specialPeriodCount_5->setFrameShadow(QFrame::Plain);
        specialPeriodCount_5->setSmallDecimalPoint(true);
        specialPeriodCount_5->setMode(QLCDNumber::Dec);
        specialPeriodCount_5->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_5->setProperty("value", QVariant(0));
        specialPeriodCount_5->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_5, 5, 4, 1, 1);

        specialCount_4 = new QLCDNumber(content);
        specialCount_4->setObjectName(QString::fromUtf8("specialCount_4"));
        sizePolicy3.setHeightForWidth(specialCount_4->sizePolicy().hasHeightForWidth());
        specialCount_4->setSizePolicy(sizePolicy3);
        specialCount_4->setMaximumSize(QSize(50, 16));
        specialCount_4->setFrameShadow(QFrame::Plain);
        specialCount_4->setSmallDecimalPoint(true);
        specialCount_4->setNumDigits(6);
        specialCount_4->setMode(QLCDNumber::Dec);
        specialCount_4->setSegmentStyle(QLCDNumber::Flat);
        specialCount_4->setProperty("value", QVariant(0));
        specialCount_4->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_4, 4, 1, 1, 1);

        specialPeriodCount_3 = new QLCDNumber(content);
        specialPeriodCount_3->setObjectName(QString::fromUtf8("specialPeriodCount_3"));
        specialPeriodCount_3->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_3->sizePolicy().hasHeightForWidth());
        specialPeriodCount_3->setSizePolicy(sizePolicy3);
        specialPeriodCount_3->setMaximumSize(QSize(40, 16));
        specialPeriodCount_3->setFrameShadow(QFrame::Plain);
        specialPeriodCount_3->setSmallDecimalPoint(true);
        specialPeriodCount_3->setMode(QLCDNumber::Dec);
        specialPeriodCount_3->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_3->setProperty("value", QVariant(0));
        specialPeriodCount_3->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_3, 3, 4, 1, 1);

        specialPeriodCount_0 = new QLCDNumber(content);
        specialPeriodCount_0->setObjectName(QString::fromUtf8("specialPeriodCount_0"));
        specialPeriodCount_0->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_0->sizePolicy().hasHeightForWidth());
        specialPeriodCount_0->setSizePolicy(sizePolicy3);
        specialPeriodCount_0->setMaximumSize(QSize(40, 16));
        specialPeriodCount_0->setFrameShadow(QFrame::Plain);
        specialPeriodCount_0->setSmallDecimalPoint(true);
        specialPeriodCount_0->setMode(QLCDNumber::Dec);
        specialPeriodCount_0->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_0->setProperty("value", QVariant(0));
        specialPeriodCount_0->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_0, 0, 4, 1, 1);

        specialName_0 = new QLabel(content);
        specialName_0->setObjectName(QString::fromUtf8("specialName_0"));
        sizePolicy3.setHeightForWidth(specialName_0->sizePolicy().hasHeightForWidth());
        specialName_0->setSizePolicy(sizePolicy3);
        specialName_0->setMinimumSize(QSize(100, 0));
        specialName_0->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_0, 0, 0, 1, 1);

        specialEnabled_10 = new QCheckBox(content);
        specialEnabled_10->setObjectName(QString::fromUtf8("specialEnabled_10"));
        sizePolicy4.setHeightForWidth(specialEnabled_10->sizePolicy().hasHeightForWidth());
        specialEnabled_10->setSizePolicy(sizePolicy4);
        specialEnabled_10->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_10, 10, 2, 1, 1);

        specialPrint_1 = new QCheckBox(content);
        specialPrint_1->setObjectName(QString::fromUtf8("specialPrint_1"));
        sizePolicy4.setHeightForWidth(specialPrint_1->sizePolicy().hasHeightForWidth());
        specialPrint_1->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_1, 1, 3, 1, 1);

        specialEnabled_1 = new QCheckBox(content);
        specialEnabled_1->setObjectName(QString::fromUtf8("specialEnabled_1"));
        sizePolicy4.setHeightForWidth(specialEnabled_1->sizePolicy().hasHeightForWidth());
        specialEnabled_1->setSizePolicy(sizePolicy4);
        specialEnabled_1->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_1, 1, 2, 1, 1);

        specialCount_2 = new QLCDNumber(content);
        specialCount_2->setObjectName(QString::fromUtf8("specialCount_2"));
        sizePolicy3.setHeightForWidth(specialCount_2->sizePolicy().hasHeightForWidth());
        specialCount_2->setSizePolicy(sizePolicy3);
        specialCount_2->setMaximumSize(QSize(50, 16));
        specialCount_2->setFrameShadow(QFrame::Plain);
        specialCount_2->setSmallDecimalPoint(true);
        specialCount_2->setNumDigits(6);
        specialCount_2->setMode(QLCDNumber::Dec);
        specialCount_2->setSegmentStyle(QLCDNumber::Flat);
        specialCount_2->setProperty("value", QVariant(0));
        specialCount_2->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_2, 2, 1, 1, 1);

        specialName_1 = new QLabel(content);
        specialName_1->setObjectName(QString::fromUtf8("specialName_1"));
        sizePolicy3.setHeightForWidth(specialName_1->sizePolicy().hasHeightForWidth());
        specialName_1->setSizePolicy(sizePolicy3);
        specialName_1->setMinimumSize(QSize(100, 0));
        specialName_1->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_1, 1, 0, 1, 1);

        specialEnabled_0 = new QCheckBox(content);
        specialEnabled_0->setObjectName(QString::fromUtf8("specialEnabled_0"));
        sizePolicy4.setHeightForWidth(specialEnabled_0->sizePolicy().hasHeightForWidth());
        specialEnabled_0->setSizePolicy(sizePolicy4);
        specialEnabled_0->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_0, 0, 2, 1, 1);

        specialEnabled_4 = new QCheckBox(content);
        specialEnabled_4->setObjectName(QString::fromUtf8("specialEnabled_4"));
        sizePolicy4.setHeightForWidth(specialEnabled_4->sizePolicy().hasHeightForWidth());
        specialEnabled_4->setSizePolicy(sizePolicy4);
        specialEnabled_4->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_4, 4, 2, 1, 1);

        specialPeriodCount_7 = new QLCDNumber(content);
        specialPeriodCount_7->setObjectName(QString::fromUtf8("specialPeriodCount_7"));
        specialPeriodCount_7->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_7->sizePolicy().hasHeightForWidth());
        specialPeriodCount_7->setSizePolicy(sizePolicy3);
        specialPeriodCount_7->setMaximumSize(QSize(40, 16));
        specialPeriodCount_7->setFrameShadow(QFrame::Plain);
        specialPeriodCount_7->setSmallDecimalPoint(true);
        specialPeriodCount_7->setMode(QLCDNumber::Dec);
        specialPeriodCount_7->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_7->setProperty("value", QVariant(0));
        specialPeriodCount_7->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_7, 7, 4, 1, 1);

        specialEnabled_2 = new QCheckBox(content);
        specialEnabled_2->setObjectName(QString::fromUtf8("specialEnabled_2"));
        sizePolicy4.setHeightForWidth(specialEnabled_2->sizePolicy().hasHeightForWidth());
        specialEnabled_2->setSizePolicy(sizePolicy4);
        specialEnabled_2->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_2, 2, 2, 1, 1);

        specialPeriodCount_4 = new QLCDNumber(content);
        specialPeriodCount_4->setObjectName(QString::fromUtf8("specialPeriodCount_4"));
        specialPeriodCount_4->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_4->sizePolicy().hasHeightForWidth());
        specialPeriodCount_4->setSizePolicy(sizePolicy3);
        specialPeriodCount_4->setMaximumSize(QSize(40, 16));
        specialPeriodCount_4->setFrameShadow(QFrame::Plain);
        specialPeriodCount_4->setSmallDecimalPoint(true);
        specialPeriodCount_4->setMode(QLCDNumber::Dec);
        specialPeriodCount_4->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_4->setProperty("value", QVariant(0));
        specialPeriodCount_4->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_4, 4, 4, 1, 1);

        specialCount_1 = new QLCDNumber(content);
        specialCount_1->setObjectName(QString::fromUtf8("specialCount_1"));
        sizePolicy3.setHeightForWidth(specialCount_1->sizePolicy().hasHeightForWidth());
        specialCount_1->setSizePolicy(sizePolicy3);
        specialCount_1->setMaximumSize(QSize(50, 16));
        specialCount_1->setFrameShadow(QFrame::Plain);
        specialCount_1->setSmallDecimalPoint(true);
        specialCount_1->setNumDigits(6);
        specialCount_1->setMode(QLCDNumber::Dec);
        specialCount_1->setSegmentStyle(QLCDNumber::Flat);
        specialCount_1->setProperty("value", QVariant(0));
        specialCount_1->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_1, 1, 1, 1, 1);

        specialEnabled_8 = new QCheckBox(content);
        specialEnabled_8->setObjectName(QString::fromUtf8("specialEnabled_8"));
        sizePolicy4.setHeightForWidth(specialEnabled_8->sizePolicy().hasHeightForWidth());
        specialEnabled_8->setSizePolicy(sizePolicy4);
        specialEnabled_8->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_8, 8, 2, 1, 1);

        specialName_8 = new QLabel(content);
        specialName_8->setObjectName(QString::fromUtf8("specialName_8"));
        sizePolicy3.setHeightForWidth(specialName_8->sizePolicy().hasHeightForWidth());
        specialName_8->setSizePolicy(sizePolicy3);
        specialName_8->setMinimumSize(QSize(100, 0));
        specialName_8->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_8, 8, 0, 1, 1);

        specialCount_8 = new QLCDNumber(content);
        specialCount_8->setObjectName(QString::fromUtf8("specialCount_8"));
        sizePolicy3.setHeightForWidth(specialCount_8->sizePolicy().hasHeightForWidth());
        specialCount_8->setSizePolicy(sizePolicy3);
        specialCount_8->setMaximumSize(QSize(50, 16));
        specialCount_8->setFrameShadow(QFrame::Plain);
        specialCount_8->setSmallDecimalPoint(true);
        specialCount_8->setNumDigits(6);
        specialCount_8->setMode(QLCDNumber::Dec);
        specialCount_8->setSegmentStyle(QLCDNumber::Flat);
        specialCount_8->setProperty("value", QVariant(0));
        specialCount_8->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_8, 8, 1, 1, 1);

        specialPrint_8 = new QCheckBox(content);
        specialPrint_8->setObjectName(QString::fromUtf8("specialPrint_8"));
        sizePolicy4.setHeightForWidth(specialPrint_8->sizePolicy().hasHeightForWidth());
        specialPrint_8->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_8, 8, 3, 1, 1);

        specialPrint_7 = new QCheckBox(content);
        specialPrint_7->setObjectName(QString::fromUtf8("specialPrint_7"));
        sizePolicy4.setHeightForWidth(specialPrint_7->sizePolicy().hasHeightForWidth());
        specialPrint_7->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_7, 7, 3, 1, 1);

        specialName_12 = new QLabel(content);
        specialName_12->setObjectName(QString::fromUtf8("specialName_12"));
        sizePolicy3.setHeightForWidth(specialName_12->sizePolicy().hasHeightForWidth());
        specialName_12->setSizePolicy(sizePolicy3);
        specialName_12->setMinimumSize(QSize(100, 0));
        specialName_12->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_12, 12, 0, 1, 1);

        specialEnabled_12 = new QCheckBox(content);
        specialEnabled_12->setObjectName(QString::fromUtf8("specialEnabled_12"));
        sizePolicy4.setHeightForWidth(specialEnabled_12->sizePolicy().hasHeightForWidth());
        specialEnabled_12->setSizePolicy(sizePolicy4);
        specialEnabled_12->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_12, 12, 2, 1, 1);

        specialPeriodCount_11 = new QLCDNumber(content);
        specialPeriodCount_11->setObjectName(QString::fromUtf8("specialPeriodCount_11"));
        specialPeriodCount_11->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_11->sizePolicy().hasHeightForWidth());
        specialPeriodCount_11->setSizePolicy(sizePolicy3);
        specialPeriodCount_11->setMaximumSize(QSize(40, 16));
        specialPeriodCount_11->setFrameShadow(QFrame::Plain);
        specialPeriodCount_11->setSmallDecimalPoint(true);
        specialPeriodCount_11->setMode(QLCDNumber::Dec);
        specialPeriodCount_11->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_11->setProperty("value", QVariant(0));
        specialPeriodCount_11->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_11, 11, 4, 1, 1);

        specialName_11 = new QLabel(content);
        specialName_11->setObjectName(QString::fromUtf8("specialName_11"));
        sizePolicy3.setHeightForWidth(specialName_11->sizePolicy().hasHeightForWidth());
        specialName_11->setSizePolicy(sizePolicy3);
        specialName_11->setMinimumSize(QSize(100, 0));
        specialName_11->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_11, 11, 0, 1, 1);

        specialCount_13 = new QLCDNumber(content);
        specialCount_13->setObjectName(QString::fromUtf8("specialCount_13"));
        sizePolicy3.setHeightForWidth(specialCount_13->sizePolicy().hasHeightForWidth());
        specialCount_13->setSizePolicy(sizePolicy3);
        specialCount_13->setMaximumSize(QSize(50, 16));
        specialCount_13->setFrameShadow(QFrame::Plain);
        specialCount_13->setSmallDecimalPoint(true);
        specialCount_13->setNumDigits(6);
        specialCount_13->setMode(QLCDNumber::Dec);
        specialCount_13->setSegmentStyle(QLCDNumber::Flat);
        specialCount_13->setProperty("value", QVariant(0));
        specialCount_13->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_13, 13, 1, 1, 1);

        specialName_13 = new QLabel(content);
        specialName_13->setObjectName(QString::fromUtf8("specialName_13"));
        sizePolicy3.setHeightForWidth(specialName_13->sizePolicy().hasHeightForWidth());
        specialName_13->setSizePolicy(sizePolicy3);
        specialName_13->setMinimumSize(QSize(100, 0));
        specialName_13->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_13, 13, 0, 1, 1);

        specialEnabled_13 = new QCheckBox(content);
        specialEnabled_13->setObjectName(QString::fromUtf8("specialEnabled_13"));
        sizePolicy4.setHeightForWidth(specialEnabled_13->sizePolicy().hasHeightForWidth());
        specialEnabled_13->setSizePolicy(sizePolicy4);
        specialEnabled_13->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_13, 13, 2, 1, 1);

        specialCount_11 = new QLCDNumber(content);
        specialCount_11->setObjectName(QString::fromUtf8("specialCount_11"));
        sizePolicy3.setHeightForWidth(specialCount_11->sizePolicy().hasHeightForWidth());
        specialCount_11->setSizePolicy(sizePolicy3);
        specialCount_11->setMaximumSize(QSize(50, 16));
        specialCount_11->setFrameShadow(QFrame::Plain);
        specialCount_11->setSmallDecimalPoint(true);
        specialCount_11->setNumDigits(6);
        specialCount_11->setMode(QLCDNumber::Dec);
        specialCount_11->setSegmentStyle(QLCDNumber::Flat);
        specialCount_11->setProperty("value", QVariant(0));
        specialCount_11->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_11, 11, 1, 1, 1);

        specialPrint_12 = new QCheckBox(content);
        specialPrint_12->setObjectName(QString::fromUtf8("specialPrint_12"));
        sizePolicy4.setHeightForWidth(specialPrint_12->sizePolicy().hasHeightForWidth());
        specialPrint_12->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_12, 12, 3, 1, 1);

        specialPeriodCount_12 = new QLCDNumber(content);
        specialPeriodCount_12->setObjectName(QString::fromUtf8("specialPeriodCount_12"));
        specialPeriodCount_12->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_12->sizePolicy().hasHeightForWidth());
        specialPeriodCount_12->setSizePolicy(sizePolicy3);
        specialPeriodCount_12->setMaximumSize(QSize(40, 16));
        specialPeriodCount_12->setFrameShadow(QFrame::Plain);
        specialPeriodCount_12->setSmallDecimalPoint(true);
        specialPeriodCount_12->setMode(QLCDNumber::Dec);
        specialPeriodCount_12->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_12->setProperty("value", QVariant(0));
        specialPeriodCount_12->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_12, 12, 4, 1, 1);

        specialPrint_13 = new QCheckBox(content);
        specialPrint_13->setObjectName(QString::fromUtf8("specialPrint_13"));
        sizePolicy4.setHeightForWidth(specialPrint_13->sizePolicy().hasHeightForWidth());
        specialPrint_13->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_13, 13, 3, 1, 1);

        specialName_4 = new QLabel(content);
        specialName_4->setObjectName(QString::fromUtf8("specialName_4"));
        sizePolicy3.setHeightForWidth(specialName_4->sizePolicy().hasHeightForWidth());
        specialName_4->setSizePolicy(sizePolicy3);
        specialName_4->setMinimumSize(QSize(100, 0));
        specialName_4->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_4, 4, 0, 1, 1);

        specialPrint_11 = new QCheckBox(content);
        specialPrint_11->setObjectName(QString::fromUtf8("specialPrint_11"));
        sizePolicy4.setHeightForWidth(specialPrint_11->sizePolicy().hasHeightForWidth());
        specialPrint_11->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_11, 11, 3, 1, 1);

        specialPrint_3 = new QCheckBox(content);
        specialPrint_3->setObjectName(QString::fromUtf8("specialPrint_3"));
        sizePolicy4.setHeightForWidth(specialPrint_3->sizePolicy().hasHeightForWidth());
        specialPrint_3->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_3, 3, 3, 1, 1);

        specialCount_3 = new QLCDNumber(content);
        specialCount_3->setObjectName(QString::fromUtf8("specialCount_3"));
        sizePolicy3.setHeightForWidth(specialCount_3->sizePolicy().hasHeightForWidth());
        specialCount_3->setSizePolicy(sizePolicy3);
        specialCount_3->setMaximumSize(QSize(50, 16));
        specialCount_3->setFrameShadow(QFrame::Plain);
        specialCount_3->setSmallDecimalPoint(true);
        specialCount_3->setNumDigits(6);
        specialCount_3->setMode(QLCDNumber::Dec);
        specialCount_3->setSegmentStyle(QLCDNumber::Flat);
        specialCount_3->setProperty("value", QVariant(0));
        specialCount_3->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_3, 3, 1, 1, 1);

        specialCount_0 = new QLCDNumber(content);
        specialCount_0->setObjectName(QString::fromUtf8("specialCount_0"));
        sizePolicy3.setHeightForWidth(specialCount_0->sizePolicy().hasHeightForWidth());
        specialCount_0->setSizePolicy(sizePolicy3);
        specialCount_0->setMaximumSize(QSize(50, 16));
        specialCount_0->setFrameShadow(QFrame::Plain);
        specialCount_0->setSmallDecimalPoint(true);
        specialCount_0->setNumDigits(6);
        specialCount_0->setMode(QLCDNumber::Dec);
        specialCount_0->setSegmentStyle(QLCDNumber::Flat);
        specialCount_0->setProperty("value", QVariant(0));
        specialCount_0->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_0, 0, 1, 1, 1);

        specialName_3 = new QLabel(content);
        specialName_3->setObjectName(QString::fromUtf8("specialName_3"));
        sizePolicy3.setHeightForWidth(specialName_3->sizePolicy().hasHeightForWidth());
        specialName_3->setSizePolicy(sizePolicy3);
        specialName_3->setMinimumSize(QSize(100, 0));
        specialName_3->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_3, 3, 0, 1, 1);

        specialPrint_9 = new QCheckBox(content);
        specialPrint_9->setObjectName(QString::fromUtf8("specialPrint_9"));
        sizePolicy4.setHeightForWidth(specialPrint_9->sizePolicy().hasHeightForWidth());
        specialPrint_9->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_9, 9, 3, 1, 1);

        specialCount_10 = new QLCDNumber(content);
        specialCount_10->setObjectName(QString::fromUtf8("specialCount_10"));
        sizePolicy3.setHeightForWidth(specialCount_10->sizePolicy().hasHeightForWidth());
        specialCount_10->setSizePolicy(sizePolicy3);
        specialCount_10->setMaximumSize(QSize(50, 16));
        specialCount_10->setFrameShadow(QFrame::Plain);
        specialCount_10->setSmallDecimalPoint(true);
        specialCount_10->setNumDigits(6);
        specialCount_10->setMode(QLCDNumber::Dec);
        specialCount_10->setSegmentStyle(QLCDNumber::Flat);
        specialCount_10->setProperty("value", QVariant(0));
        specialCount_10->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_10, 10, 1, 1, 1);

        specialName_2 = new QLabel(content);
        specialName_2->setObjectName(QString::fromUtf8("specialName_2"));
        sizePolicy3.setHeightForWidth(specialName_2->sizePolicy().hasHeightForWidth());
        specialName_2->setSizePolicy(sizePolicy3);
        specialName_2->setMinimumSize(QSize(100, 0));
        specialName_2->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_2, 2, 0, 1, 1);

        specialEnabled_6 = new QCheckBox(content);
        specialEnabled_6->setObjectName(QString::fromUtf8("specialEnabled_6"));
        sizePolicy4.setHeightForWidth(specialEnabled_6->sizePolicy().hasHeightForWidth());
        specialEnabled_6->setSizePolicy(sizePolicy4);
        specialEnabled_6->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_6, 6, 2, 1, 1);

        specialPrint_5 = new QCheckBox(content);
        specialPrint_5->setObjectName(QString::fromUtf8("specialPrint_5"));
        sizePolicy4.setHeightForWidth(specialPrint_5->sizePolicy().hasHeightForWidth());
        specialPrint_5->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_5, 5, 3, 1, 1);

        specialPeriodCount_2 = new QLCDNumber(content);
        specialPeriodCount_2->setObjectName(QString::fromUtf8("specialPeriodCount_2"));
        specialPeriodCount_2->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_2->sizePolicy().hasHeightForWidth());
        specialPeriodCount_2->setSizePolicy(sizePolicy3);
        specialPeriodCount_2->setMaximumSize(QSize(40, 16));
        specialPeriodCount_2->setFrameShadow(QFrame::Plain);
        specialPeriodCount_2->setSmallDecimalPoint(true);
        specialPeriodCount_2->setMode(QLCDNumber::Dec);
        specialPeriodCount_2->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_2->setProperty("value", QVariant(0));
        specialPeriodCount_2->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_2, 2, 4, 1, 1);

        specialPrint_2 = new QCheckBox(content);
        specialPrint_2->setObjectName(QString::fromUtf8("specialPrint_2"));
        sizePolicy4.setHeightForWidth(specialPrint_2->sizePolicy().hasHeightForWidth());
        specialPrint_2->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_2, 2, 3, 1, 1);

        specialEnabled_5 = new QCheckBox(content);
        specialEnabled_5->setObjectName(QString::fromUtf8("specialEnabled_5"));
        sizePolicy4.setHeightForWidth(specialEnabled_5->sizePolicy().hasHeightForWidth());
        specialEnabled_5->setSizePolicy(sizePolicy4);
        specialEnabled_5->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_5, 5, 2, 1, 1);

        specialCount_6 = new QLCDNumber(content);
        specialCount_6->setObjectName(QString::fromUtf8("specialCount_6"));
        sizePolicy3.setHeightForWidth(specialCount_6->sizePolicy().hasHeightForWidth());
        specialCount_6->setSizePolicy(sizePolicy3);
        specialCount_6->setMaximumSize(QSize(50, 16));
        specialCount_6->setFrameShadow(QFrame::Plain);
        specialCount_6->setSmallDecimalPoint(true);
        specialCount_6->setNumDigits(6);
        specialCount_6->setMode(QLCDNumber::Dec);
        specialCount_6->setSegmentStyle(QLCDNumber::Flat);
        specialCount_6->setProperty("value", QVariant(0));
        specialCount_6->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_6, 6, 1, 1, 1);

        specialName_5 = new QLabel(content);
        specialName_5->setObjectName(QString::fromUtf8("specialName_5"));
        sizePolicy3.setHeightForWidth(specialName_5->sizePolicy().hasHeightForWidth());
        specialName_5->setSizePolicy(sizePolicy3);
        specialName_5->setMinimumSize(QSize(100, 0));
        specialName_5->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_5, 5, 0, 1, 1);

        specialName_6 = new QLabel(content);
        specialName_6->setObjectName(QString::fromUtf8("specialName_6"));
        sizePolicy3.setHeightForWidth(specialName_6->sizePolicy().hasHeightForWidth());
        specialName_6->setSizePolicy(sizePolicy3);
        specialName_6->setMinimumSize(QSize(100, 0));
        specialName_6->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_6, 6, 0, 1, 1);

        specialCount_7 = new QLCDNumber(content);
        specialCount_7->setObjectName(QString::fromUtf8("specialCount_7"));
        sizePolicy3.setHeightForWidth(specialCount_7->sizePolicy().hasHeightForWidth());
        specialCount_7->setSizePolicy(sizePolicy3);
        specialCount_7->setMaximumSize(QSize(50, 16));
        specialCount_7->setFrameShadow(QFrame::Plain);
        specialCount_7->setSmallDecimalPoint(true);
        specialCount_7->setNumDigits(6);
        specialCount_7->setMode(QLCDNumber::Dec);
        specialCount_7->setSegmentStyle(QLCDNumber::Flat);
        specialCount_7->setProperty("value", QVariant(0));
        specialCount_7->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_7, 7, 1, 1, 1);

        specialPrint_14 = new QCheckBox(content);
        specialPrint_14->setObjectName(QString::fromUtf8("specialPrint_14"));
        sizePolicy4.setHeightForWidth(specialPrint_14->sizePolicy().hasHeightForWidth());
        specialPrint_14->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_14, 14, 3, 1, 1);

        specialPeriodCount_6 = new QLCDNumber(content);
        specialPeriodCount_6->setObjectName(QString::fromUtf8("specialPeriodCount_6"));
        specialPeriodCount_6->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_6->sizePolicy().hasHeightForWidth());
        specialPeriodCount_6->setSizePolicy(sizePolicy3);
        specialPeriodCount_6->setMaximumSize(QSize(40, 16));
        specialPeriodCount_6->setFrameShadow(QFrame::Plain);
        specialPeriodCount_6->setSmallDecimalPoint(true);
        specialPeriodCount_6->setMode(QLCDNumber::Dec);
        specialPeriodCount_6->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_6->setProperty("value", QVariant(0));
        specialPeriodCount_6->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_6, 6, 4, 1, 1);

        specialPrint_6 = new QCheckBox(content);
        specialPrint_6->setObjectName(QString::fromUtf8("specialPrint_6"));
        sizePolicy4.setHeightForWidth(specialPrint_6->sizePolicy().hasHeightForWidth());
        specialPrint_6->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_6, 6, 3, 1, 1);

        specialEnabled_7 = new QCheckBox(content);
        specialEnabled_7->setObjectName(QString::fromUtf8("specialEnabled_7"));
        sizePolicy4.setHeightForWidth(specialEnabled_7->sizePolicy().hasHeightForWidth());
        specialEnabled_7->setSizePolicy(sizePolicy4);
        specialEnabled_7->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_7, 7, 2, 1, 1);

        specialEnabled_9 = new QCheckBox(content);
        specialEnabled_9->setObjectName(QString::fromUtf8("specialEnabled_9"));
        sizePolicy4.setHeightForWidth(specialEnabled_9->sizePolicy().hasHeightForWidth());
        specialEnabled_9->setSizePolicy(sizePolicy4);
        specialEnabled_9->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_9, 9, 2, 1, 1);

        specialName_14 = new QLabel(content);
        specialName_14->setObjectName(QString::fromUtf8("specialName_14"));
        sizePolicy3.setHeightForWidth(specialName_14->sizePolicy().hasHeightForWidth());
        specialName_14->setSizePolicy(sizePolicy3);
        specialName_14->setMinimumSize(QSize(100, 0));
        specialName_14->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_14, 14, 0, 1, 1);

        specialPeriodCount_8 = new QLCDNumber(content);
        specialPeriodCount_8->setObjectName(QString::fromUtf8("specialPeriodCount_8"));
        specialPeriodCount_8->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_8->sizePolicy().hasHeightForWidth());
        specialPeriodCount_8->setSizePolicy(sizePolicy3);
        specialPeriodCount_8->setMaximumSize(QSize(40, 16));
        specialPeriodCount_8->setFrameShadow(QFrame::Plain);
        specialPeriodCount_8->setSmallDecimalPoint(true);
        specialPeriodCount_8->setMode(QLCDNumber::Dec);
        specialPeriodCount_8->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_8->setProperty("value", QVariant(0));
        specialPeriodCount_8->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_8, 8, 4, 1, 1);

        specialCount_14 = new QLCDNumber(content);
        specialCount_14->setObjectName(QString::fromUtf8("specialCount_14"));
        sizePolicy3.setHeightForWidth(specialCount_14->sizePolicy().hasHeightForWidth());
        specialCount_14->setSizePolicy(sizePolicy3);
        specialCount_14->setMaximumSize(QSize(50, 16));
        specialCount_14->setFrameShadow(QFrame::Plain);
        specialCount_14->setSmallDecimalPoint(true);
        specialCount_14->setNumDigits(6);
        specialCount_14->setMode(QLCDNumber::Dec);
        specialCount_14->setSegmentStyle(QLCDNumber::Flat);
        specialCount_14->setProperty("value", QVariant(0));
        specialCount_14->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_14, 14, 1, 1, 1);

        specialName_9 = new QLabel(content);
        specialName_9->setObjectName(QString::fromUtf8("specialName_9"));
        sizePolicy3.setHeightForWidth(specialName_9->sizePolicy().hasHeightForWidth());
        specialName_9->setSizePolicy(sizePolicy3);
        specialName_9->setMinimumSize(QSize(100, 0));
        specialName_9->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_9, 9, 0, 1, 1);

        specialPeriodCount_13 = new QLCDNumber(content);
        specialPeriodCount_13->setObjectName(QString::fromUtf8("specialPeriodCount_13"));
        specialPeriodCount_13->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_13->sizePolicy().hasHeightForWidth());
        specialPeriodCount_13->setSizePolicy(sizePolicy3);
        specialPeriodCount_13->setMaximumSize(QSize(40, 16));
        specialPeriodCount_13->setFrameShadow(QFrame::Plain);
        specialPeriodCount_13->setSmallDecimalPoint(true);
        specialPeriodCount_13->setMode(QLCDNumber::Dec);
        specialPeriodCount_13->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_13->setProperty("value", QVariant(0));
        specialPeriodCount_13->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_13, 13, 4, 1, 1);

        specialPeriodCount_9 = new QLCDNumber(content);
        specialPeriodCount_9->setObjectName(QString::fromUtf8("specialPeriodCount_9"));
        specialPeriodCount_9->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_9->sizePolicy().hasHeightForWidth());
        specialPeriodCount_9->setSizePolicy(sizePolicy3);
        specialPeriodCount_9->setMaximumSize(QSize(40, 16));
        specialPeriodCount_9->setFrameShadow(QFrame::Plain);
        specialPeriodCount_9->setSmallDecimalPoint(true);
        specialPeriodCount_9->setMode(QLCDNumber::Dec);
        specialPeriodCount_9->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_9->setProperty("value", QVariant(0));
        specialPeriodCount_9->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_9, 9, 4, 1, 1);

        specialName_10 = new QLabel(content);
        specialName_10->setObjectName(QString::fromUtf8("specialName_10"));
        sizePolicy3.setHeightForWidth(specialName_10->sizePolicy().hasHeightForWidth());
        specialName_10->setSizePolicy(sizePolicy3);
        specialName_10->setMinimumSize(QSize(100, 0));
        specialName_10->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_10, 10, 0, 1, 1);

        specialPeriodCount_10 = new QLCDNumber(content);
        specialPeriodCount_10->setObjectName(QString::fromUtf8("specialPeriodCount_10"));
        specialPeriodCount_10->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_10->sizePolicy().hasHeightForWidth());
        specialPeriodCount_10->setSizePolicy(sizePolicy3);
        specialPeriodCount_10->setMaximumSize(QSize(40, 16));
        specialPeriodCount_10->setFrameShadow(QFrame::Plain);
        specialPeriodCount_10->setSmallDecimalPoint(true);
        specialPeriodCount_10->setMode(QLCDNumber::Dec);
        specialPeriodCount_10->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_10->setProperty("value", QVariant(0));
        specialPeriodCount_10->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_10, 10, 4, 1, 1);

        specialPrint_10 = new QCheckBox(content);
        specialPrint_10->setObjectName(QString::fromUtf8("specialPrint_10"));
        sizePolicy4.setHeightForWidth(specialPrint_10->sizePolicy().hasHeightForWidth());
        specialPrint_10->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_10, 10, 3, 1, 1);

        specialCount_9 = new QLCDNumber(content);
        specialCount_9->setObjectName(QString::fromUtf8("specialCount_9"));
        sizePolicy3.setHeightForWidth(specialCount_9->sizePolicy().hasHeightForWidth());
        specialCount_9->setSizePolicy(sizePolicy3);
        specialCount_9->setMaximumSize(QSize(50, 16));
        specialCount_9->setFrameShadow(QFrame::Plain);
        specialCount_9->setSmallDecimalPoint(true);
        specialCount_9->setNumDigits(6);
        specialCount_9->setMode(QLCDNumber::Dec);
        specialCount_9->setSegmentStyle(QLCDNumber::Flat);
        specialCount_9->setProperty("value", QVariant(0));
        specialCount_9->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_9, 9, 1, 1, 1);

        specialPrint_0 = new QCheckBox(content);
        specialPrint_0->setObjectName(QString::fromUtf8("specialPrint_0"));
        sizePolicy4.setHeightForWidth(specialPrint_0->sizePolicy().hasHeightForWidth());
        specialPrint_0->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_0, 0, 3, 1, 1);

        specialPeriodCount_1 = new QLCDNumber(content);
        specialPeriodCount_1->setObjectName(QString::fromUtf8("specialPeriodCount_1"));
        specialPeriodCount_1->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_1->sizePolicy().hasHeightForWidth());
        specialPeriodCount_1->setSizePolicy(sizePolicy3);
        specialPeriodCount_1->setMaximumSize(QSize(40, 16));
        specialPeriodCount_1->setFrameShadow(QFrame::Plain);
        specialPeriodCount_1->setSmallDecimalPoint(true);
        specialPeriodCount_1->setMode(QLCDNumber::Dec);
        specialPeriodCount_1->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_1->setProperty("value", QVariant(0));
        specialPeriodCount_1->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_1, 1, 4, 1, 1);

        specialName_15 = new QLabel(content);
        specialName_15->setObjectName(QString::fromUtf8("specialName_15"));
        sizePolicy3.setHeightForWidth(specialName_15->sizePolicy().hasHeightForWidth());
        specialName_15->setSizePolicy(sizePolicy3);
        specialName_15->setMinimumSize(QSize(100, 0));
        specialName_15->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_15, 15, 0, 1, 1);

        specialPeriodCount_15 = new QLCDNumber(content);
        specialPeriodCount_15->setObjectName(QString::fromUtf8("specialPeriodCount_15"));
        specialPeriodCount_15->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_15->sizePolicy().hasHeightForWidth());
        specialPeriodCount_15->setSizePolicy(sizePolicy3);
        specialPeriodCount_15->setMaximumSize(QSize(40, 16));
        specialPeriodCount_15->setFrameShadow(QFrame::Plain);
        specialPeriodCount_15->setSmallDecimalPoint(true);
        specialPeriodCount_15->setMode(QLCDNumber::Dec);
        specialPeriodCount_15->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_15->setProperty("value", QVariant(0));
        specialPeriodCount_15->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_15, 15, 4, 1, 1);

        specialEnabled_14 = new QCheckBox(content);
        specialEnabled_14->setObjectName(QString::fromUtf8("specialEnabled_14"));
        sizePolicy4.setHeightForWidth(specialEnabled_14->sizePolicy().hasHeightForWidth());
        specialEnabled_14->setSizePolicy(sizePolicy4);
        specialEnabled_14->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_14, 14, 2, 1, 1);

        specialPeriodCount_14 = new QLCDNumber(content);
        specialPeriodCount_14->setObjectName(QString::fromUtf8("specialPeriodCount_14"));
        specialPeriodCount_14->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_14->sizePolicy().hasHeightForWidth());
        specialPeriodCount_14->setSizePolicy(sizePolicy3);
        specialPeriodCount_14->setMaximumSize(QSize(40, 16));
        specialPeriodCount_14->setFrameShadow(QFrame::Plain);
        specialPeriodCount_14->setSmallDecimalPoint(true);
        specialPeriodCount_14->setMode(QLCDNumber::Dec);
        specialPeriodCount_14->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_14->setProperty("value", QVariant(0));
        specialPeriodCount_14->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_14, 14, 4, 1, 1);

        specialCount_15 = new QLCDNumber(content);
        specialCount_15->setObjectName(QString::fromUtf8("specialCount_15"));
        sizePolicy3.setHeightForWidth(specialCount_15->sizePolicy().hasHeightForWidth());
        specialCount_15->setSizePolicy(sizePolicy3);
        specialCount_15->setMaximumSize(QSize(50, 16));
        specialCount_15->setFrameShadow(QFrame::Plain);
        specialCount_15->setSmallDecimalPoint(true);
        specialCount_15->setNumDigits(6);
        specialCount_15->setMode(QLCDNumber::Dec);
        specialCount_15->setSegmentStyle(QLCDNumber::Flat);
        specialCount_15->setProperty("value", QVariant(0));
        specialCount_15->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_15, 15, 1, 1, 1);

        specialName_16 = new QLabel(content);
        specialName_16->setObjectName(QString::fromUtf8("specialName_16"));
        sizePolicy3.setHeightForWidth(specialName_16->sizePolicy().hasHeightForWidth());
        specialName_16->setSizePolicy(sizePolicy3);
        specialName_16->setMinimumSize(QSize(100, 0));
        specialName_16->setMaximumSize(QSize(300, 16777215));

        specialLayout->addWidget(specialName_16, 16, 0, 1, 1);

        specialEnabled_15 = new QCheckBox(content);
        specialEnabled_15->setObjectName(QString::fromUtf8("specialEnabled_15"));
        sizePolicy4.setHeightForWidth(specialEnabled_15->sizePolicy().hasHeightForWidth());
        specialEnabled_15->setSizePolicy(sizePolicy4);
        specialEnabled_15->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_15, 15, 2, 1, 1);

        specialCount_16 = new QLCDNumber(content);
        specialCount_16->setObjectName(QString::fromUtf8("specialCount_16"));
        sizePolicy3.setHeightForWidth(specialCount_16->sizePolicy().hasHeightForWidth());
        specialCount_16->setSizePolicy(sizePolicy3);
        specialCount_16->setMaximumSize(QSize(50, 16));
        specialCount_16->setFrameShadow(QFrame::Plain);
        specialCount_16->setSmallDecimalPoint(true);
        specialCount_16->setNumDigits(6);
        specialCount_16->setMode(QLCDNumber::Dec);
        specialCount_16->setSegmentStyle(QLCDNumber::Flat);
        specialCount_16->setProperty("value", QVariant(0));
        specialCount_16->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialCount_16, 16, 1, 1, 1);

        specialPrint_15 = new QCheckBox(content);
        specialPrint_15->setObjectName(QString::fromUtf8("specialPrint_15"));
        sizePolicy4.setHeightForWidth(specialPrint_15->sizePolicy().hasHeightForWidth());
        specialPrint_15->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_15, 15, 3, 1, 1);

        specialPeriodCount_16 = new QLCDNumber(content);
        specialPeriodCount_16->setObjectName(QString::fromUtf8("specialPeriodCount_16"));
        specialPeriodCount_16->setEnabled(false);
        sizePolicy3.setHeightForWidth(specialPeriodCount_16->sizePolicy().hasHeightForWidth());
        specialPeriodCount_16->setSizePolicy(sizePolicy3);
        specialPeriodCount_16->setMaximumSize(QSize(40, 16));
        specialPeriodCount_16->setFrameShadow(QFrame::Plain);
        specialPeriodCount_16->setSmallDecimalPoint(true);
        specialPeriodCount_16->setMode(QLCDNumber::Dec);
        specialPeriodCount_16->setSegmentStyle(QLCDNumber::Flat);
        specialPeriodCount_16->setProperty("value", QVariant(0));
        specialPeriodCount_16->setProperty("intValue", QVariant(0));

        specialLayout->addWidget(specialPeriodCount_16, 16, 4, 1, 1);

        specialPrint_16 = new QCheckBox(content);
        specialPrint_16->setObjectName(QString::fromUtf8("specialPrint_16"));
        sizePolicy4.setHeightForWidth(specialPrint_16->sizePolicy().hasHeightForWidth());
        specialPrint_16->setSizePolicy(sizePolicy4);

        specialLayout->addWidget(specialPrint_16, 16, 3, 1, 1);

        specialEnabled_16 = new QCheckBox(content);
        specialEnabled_16->setObjectName(QString::fromUtf8("specialEnabled_16"));
        sizePolicy4.setHeightForWidth(specialEnabled_16->sizePolicy().hasHeightForWidth());
        specialEnabled_16->setSizePolicy(sizePolicy4);
        specialEnabled_16->setMaximumSize(QSize(16, 16));

        specialLayout->addWidget(specialEnabled_16, 16, 2, 1, 1);

        widget_2 = new QWidget(content);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));

        specialLayout->addWidget(widget_2, 17, 0, 1, 5);


        gridLayout->addLayout(specialLayout, 1, 3, 3, 1);

        alarmValLayout = new QGridLayout();
        alarmValLayout->setObjectName(QString::fromUtf8("alarmValLayout"));
        alarmVal0 = new QLCDNumber(content);
        alarmVal0->setObjectName(QString::fromUtf8("alarmVal0"));
        sizePolicy3.setHeightForWidth(alarmVal0->sizePolicy().hasHeightForWidth());
        alarmVal0->setSizePolicy(sizePolicy3);
        alarmVal0->setMaximumSize(QSize(100, 20));
        alarmVal0->setFrameShadow(QFrame::Plain);
        alarmVal0->setSmallDecimalPoint(true);
        alarmVal0->setNumDigits(8);
        alarmVal0->setMode(QLCDNumber::Hex);
        alarmVal0->setSegmentStyle(QLCDNumber::Flat);
        alarmVal0->setProperty("value", QVariant(0));
        alarmVal0->setProperty("intValue", QVariant(0));

        alarmValLayout->addWidget(alarmVal0, 0, 1, 1, 1);

        alarmVal2 = new QLCDNumber(content);
        alarmVal2->setObjectName(QString::fromUtf8("alarmVal2"));
        sizePolicy3.setHeightForWidth(alarmVal2->sizePolicy().hasHeightForWidth());
        alarmVal2->setSizePolicy(sizePolicy3);
        alarmVal2->setMaximumSize(QSize(100, 20));
        alarmVal2->setFrameShadow(QFrame::Plain);
        alarmVal2->setSmallDecimalPoint(true);
        alarmVal2->setNumDigits(8);
        alarmVal2->setMode(QLCDNumber::Hex);
        alarmVal2->setSegmentStyle(QLCDNumber::Flat);
        alarmVal2->setProperty("value", QVariant(0));
        alarmVal2->setProperty("intValue", QVariant(0));

        alarmValLayout->addWidget(alarmVal2, 2, 1, 1, 1);

        alarmValName_2 = new QLabel(content);
        alarmValName_2->setObjectName(QString::fromUtf8("alarmValName_2"));
        sizePolicy3.setHeightForWidth(alarmValName_2->sizePolicy().hasHeightForWidth());
        alarmValName_2->setSizePolicy(sizePolicy3);
        alarmValName_2->setMinimumSize(QSize(130, 0));
        alarmValName_2->setMaximumSize(QSize(250, 16777215));
        alarmValName_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        alarmValLayout->addWidget(alarmValName_2, 2, 0, 1, 1);

        alarmValName_1 = new QLabel(content);
        alarmValName_1->setObjectName(QString::fromUtf8("alarmValName_1"));
        sizePolicy3.setHeightForWidth(alarmValName_1->sizePolicy().hasHeightForWidth());
        alarmValName_1->setSizePolicy(sizePolicy3);
        alarmValName_1->setMinimumSize(QSize(130, 0));
        alarmValName_1->setMaximumSize(QSize(250, 16777215));
        alarmValName_1->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        alarmValLayout->addWidget(alarmValName_1, 1, 0, 1, 1);

        alarmVal1 = new QLCDNumber(content);
        alarmVal1->setObjectName(QString::fromUtf8("alarmVal1"));
        sizePolicy3.setHeightForWidth(alarmVal1->sizePolicy().hasHeightForWidth());
        alarmVal1->setSizePolicy(sizePolicy3);
        alarmVal1->setMaximumSize(QSize(100, 20));
        alarmVal1->setFrameShadow(QFrame::Plain);
        alarmVal1->setSmallDecimalPoint(true);
        alarmVal1->setNumDigits(8);
        alarmVal1->setMode(QLCDNumber::Hex);
        alarmVal1->setSegmentStyle(QLCDNumber::Flat);
        alarmVal1->setProperty("value", QVariant(0));
        alarmVal1->setProperty("intValue", QVariant(0));

        alarmValLayout->addWidget(alarmVal1, 1, 1, 1, 1);

        alarmValName_0 = new QLabel(content);
        alarmValName_0->setObjectName(QString::fromUtf8("alarmValName_0"));
        sizePolicy3.setHeightForWidth(alarmValName_0->sizePolicy().hasHeightForWidth());
        alarmValName_0->setSizePolicy(sizePolicy3);
        alarmValName_0->setMinimumSize(QSize(130, 0));
        alarmValName_0->setMaximumSize(QSize(250, 16777215));
        alarmValName_0->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        alarmValLayout->addWidget(alarmValName_0, 0, 0, 1, 1);

        alarmWidget = new QWidget(content);
        alarmWidget->setObjectName(QString::fromUtf8("alarmWidget"));

        alarmValLayout->addWidget(alarmWidget, 3, 0, 1, 2);


        gridLayout->addLayout(alarmValLayout, 4, 3, 1, 1);

        portsTab = new QTabWidget(content);
        portsTab->setObjectName(QString::fromUtf8("portsTab"));
        QSizePolicy sizePolicy5(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(portsTab->sizePolicy().hasHeightForWidth());
        portsTab->setSizePolicy(sizePolicy5);
        portsTab->setMinimumSize(QSize(520, 150));
        portsTab->setMaximumSize(QSize(16777215, 150));
        portsTab1 = new QWidget();
        portsTab1->setObjectName(QString::fromUtf8("portsTab1"));
        gridLayout_2 = new QGridLayout(portsTab1);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        portsTab->addTab(portsTab1, QString());

        gridLayout->addWidget(portsTab, 4, 0, 1, 3);

        specialFieldsTab = new QTabWidget(content);
        specialFieldsTab->setObjectName(QString::fromUtf8("specialFieldsTab"));
        specialFieldsTab->setMaximumSize(QSize(16777215, 16777215));
        specialFieldsTab1 = new QWidget();
        specialFieldsTab1->setObjectName(QString::fromUtf8("specialFieldsTab1"));
        gridLayout_3 = new QGridLayout(specialFieldsTab1);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        specialFieldsTab->addTab(specialFieldsTab1, QString());

        gridLayout->addWidget(specialFieldsTab, 1, 4, 4, 1);

        proceduresList = new QTableWidget(content);
        proceduresList->setObjectName(QString::fromUtf8("proceduresList"));
        sizePolicy.setHeightForWidth(proceduresList->sizePolicy().hasHeightForWidth());
        proceduresList->setSizePolicy(sizePolicy);
        proceduresList->setMinimumSize(QSize(400, 200));
        proceduresList->setEditTriggers(QAbstractItemView::NoEditTriggers);

        gridLayout->addWidget(proceduresList, 1, 0, 1, 1);

        buttonsLayout = new QVBoxLayout();
        buttonsLayout->setObjectName(QString::fromUtf8("buttonsLayout"));
        tpLoadButton = new QPushButton(content);
        tpLoadButton->setObjectName(QString::fromUtf8("tpLoadButton"));
        tpLoadButton->setEnabled(false);
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(tpLoadButton->sizePolicy().hasHeightForWidth());
        tpLoadButton->setSizePolicy(sizePolicy6);
        tpLoadButton->setMinimumSize(QSize(110, 40));
        tpLoadButton->setMaximumSize(QSize(16777215, 16777215));

        buttonsLayout->addWidget(tpLoadButton);

        tpLaunchButton = new QPushButton(content);
        tpLaunchButton->setObjectName(QString::fromUtf8("tpLaunchButton"));
        tpLaunchButton->setEnabled(false);
        sizePolicy6.setHeightForWidth(tpLaunchButton->sizePolicy().hasHeightForWidth());
        tpLaunchButton->setSizePolicy(sizePolicy6);
        tpLaunchButton->setMinimumSize(QSize(110, 40));
        tpLaunchButton->setMaximumSize(QSize(16777215, 16777215));

        buttonsLayout->addWidget(tpLaunchButton);

        tpLoadLaunchButton = new QPushButton(content);
        tpLoadLaunchButton->setObjectName(QString::fromUtf8("tpLoadLaunchButton"));
        tpLoadLaunchButton->setEnabled(false);
        sizePolicy6.setHeightForWidth(tpLoadLaunchButton->sizePolicy().hasHeightForWidth());
        tpLoadLaunchButton->setSizePolicy(sizePolicy6);
        tpLoadLaunchButton->setMinimumSize(QSize(110, 40));
        tpLoadLaunchButton->setMaximumSize(QSize(16777215, 16777215));

        buttonsLayout->addWidget(tpLoadLaunchButton);

        LoadLaunchAllLayout = new QHBoxLayout();
        LoadLaunchAllLayout->setSpacing(0);
        LoadLaunchAllLayout->setObjectName(QString::fromUtf8("LoadLaunchAllLayout"));
        tpLoadLaunchAllButton = new QPushButton(content);
        tpLoadLaunchAllButton->setObjectName(QString::fromUtf8("tpLoadLaunchAllButton"));
        tpLoadLaunchAllButton->setEnabled(false);
        sizePolicy6.setHeightForWidth(tpLoadLaunchAllButton->sizePolicy().hasHeightForWidth());
        tpLoadLaunchAllButton->setSizePolicy(sizePolicy6);
        tpLoadLaunchAllButton->setMinimumSize(QSize(90, 40));
        tpLoadLaunchAllButton->setMaximumSize(QSize(16777215, 16777215));

        LoadLaunchAllLayout->addWidget(tpLoadLaunchAllButton);

        tpLoadLaunchAllStop = new QCheckBox(content);
        tpLoadLaunchAllStop->setObjectName(QString::fromUtf8("tpLoadLaunchAllStop"));
        tpLoadLaunchAllStop->setEnabled(false);
        sizePolicy4.setHeightForWidth(tpLoadLaunchAllStop->sizePolicy().hasHeightForWidth());
        tpLoadLaunchAllStop->setSizePolicy(sizePolicy4);
        tpLoadLaunchAllStop->setMaximumSize(QSize(16, 16));

        LoadLaunchAllLayout->addWidget(tpLoadLaunchAllStop);


        buttonsLayout->addLayout(LoadLaunchAllLayout);

        cancelButton = new QPushButton(content);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setEnabled(false);
        sizePolicy6.setHeightForWidth(cancelButton->sizePolicy().hasHeightForWidth());
        cancelButton->setSizePolicy(sizePolicy6);
        cancelButton->setMinimumSize(QSize(110, 40));
        cancelButton->setMaximumSize(QSize(16777215, 16777215));

        buttonsLayout->addWidget(cancelButton);


        gridLayout->addLayout(buttonsLayout, 1, 2, 1, 1);

        mainForm->setCentralWidget(content);
        statusbar = new QStatusBar(mainForm);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        mainForm->setStatusBar(statusbar);
        menuBar = new QMenuBar(mainForm);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 21));
        menuGSS = new QMenu(menuBar);
        menuGSS->setObjectName(QString::fromUtf8("menuGSS"));
        menuLogs_folder = new QMenu(menuGSS);
        menuLogs_folder->setObjectName(QString::fromUtf8("menuLogs_folder"));
        menuSend_Raw_Commands = new QMenu(menuBar);
        menuSend_Raw_Commands->setObjectName(QString::fromUtf8("menuSend_Raw_Commands"));
        menuSend_Protocol_Packets = new QMenu(menuBar);
        menuSend_Protocol_Packets->setObjectName(QString::fromUtf8("menuSend_Protocol_Packets"));
        menuSend_Steps = new QMenu(menuBar);
        menuSend_Steps->setObjectName(QString::fromUtf8("menuSend_Steps"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        mainForm->setMenuBar(menuBar);

        menuBar->addAction(menuGSS->menuAction());
        menuBar->addAction(menuSend_Raw_Commands->menuAction());
        menuBar->addAction(menuSend_Protocol_Packets->menuAction());
        menuBar->addAction(menuSend_Steps->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuGSS->addAction(actionSwitch_GSS_config_file);
        menuGSS->addAction(actionEdit_current_GSS_config_file);
        menuGSS->addAction(menuLogs_folder->menuAction());
        menuGSS->addAction(actionReset_GSS);
        menuLogs_folder->addAction(actionOpen_Logs_folder);
        menuLogs_folder->addAction(actionSelect_Logs_folder);
        menuSend_Steps->addAction(actionSend_one_step);
        menuSend_Steps->addAction(actionSend_several_steps);
        menuHelp->addAction(actionUserManual);
        menuHelp->addAction(actionAbout);
        menuView->addAction(actionPeriodic_TCs);
        menuView->addAction(actionPlots);
        menuView->addAction(actionSend_single_SpW_TC);
        menuView->addAction(actionEnable_periodical_SpW_TC);

        retranslateUi(mainForm);

        QMetaObject::connectSlotsByName(mainForm);
    } // setupUi

    void retranslateUi(QMainWindow *mainForm)
    {
        actionEdit_current_GSS_config_file->setText(QApplication::translate("mainForm", "Edit current MASSIVA config file", 0, QApplication::UnicodeUTF8));
        actionSend_one_step->setText(QApplication::translate("mainForm", "Send one step", 0, QApplication::UnicodeUTF8));
        actionSend_several_steps->setText(QApplication::translate("mainForm", "Send several steps", 0, QApplication::UnicodeUTF8));
        actionReset_GSS->setText(QApplication::translate("mainForm", "Reset MASSIVA", 0, QApplication::UnicodeUTF8));
        actionSwitch_GSS_config_file->setText(QApplication::translate("mainForm", "Switch MASSIVA config file", 0, QApplication::UnicodeUTF8));
        actionSwitch_GSS_config_file->setIconText(QApplication::translate("mainForm", "Switch MASSIVA config file", 0, QApplication::UnicodeUTF8));
        actionUserManual->setText(QApplication::translate("mainForm", "User Manual", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("mainForm", "About", 0, QApplication::UnicodeUTF8));
        actionPeriodic_TCs->setText(QApplication::translate("mainForm", "Periodic Telecommands", 0, QApplication::UnicodeUTF8));
        actionPlots->setText(QApplication::translate("mainForm", "Plots", 0, QApplication::UnicodeUTF8));
        actionSend_single_SpW_TC->setText(QApplication::translate("mainForm", "Send single SpW timecode", 0, QApplication::UnicodeUTF8));
        actionEnable_periodical_SpW_TC->setText(QApplication::translate("mainForm", "Enable periodical SpW timecodes", 0, QApplication::UnicodeUTF8));
        actionOpen_Logs_folder->setText(QApplication::translate("mainForm", "Open", 0, QApplication::UnicodeUTF8));
        actionSelect_Logs_folder->setText(QApplication::translate("mainForm", "Select", 0, QApplication::UnicodeUTF8));
        barTxStep->setFormat(QApplication::translate("mainForm", "%v / %m", 0, QApplication::UnicodeUTF8));
        barInput->setFormat(QApplication::translate("mainForm", "%v / %m", 0, QApplication::UnicodeUTF8));
        barRxStep->setFormat(QApplication::translate("mainForm", "%v / %m", 0, QApplication::UnicodeUTF8));
        barOutput->setFormat(QApplication::translate("mainForm", "%v / %m", 0, QApplication::UnicodeUTF8));
        resetGlobalVars->setText(QApplication::translate("mainForm", "Reset global variable", 0, QApplication::UnicodeUTF8));
        logButton->setText(QApplication::translate("mainForm", "Log", 0, QApplication::UnicodeUTF8));
        rawLogButton->setText(QApplication::translate("mainForm", "Raw Log", 0, QApplication::UnicodeUTF8));
        specialPrint_4->setText(QString());
        specialName_7->setText(QString());
        specialEnabled_3->setText(QString());
        specialEnabled_11->setText(QString());
        specialName_0->setText(QString());
        specialEnabled_10->setText(QString());
        specialPrint_1->setText(QString());
        specialEnabled_1->setText(QString());
        specialName_1->setText(QString());
        specialEnabled_0->setText(QString());
        specialEnabled_4->setText(QString());
        specialEnabled_2->setText(QString());
        specialEnabled_8->setText(QString());
        specialName_8->setText(QString());
        specialPrint_8->setText(QString());
        specialPrint_7->setText(QString());
        specialName_12->setText(QString());
        specialEnabled_12->setText(QString());
        specialName_11->setText(QString());
        specialName_13->setText(QString());
        specialEnabled_13->setText(QString());
        specialPrint_12->setText(QString());
        specialPrint_13->setText(QString());
        specialName_4->setText(QString());
        specialPrint_11->setText(QString());
        specialPrint_3->setText(QString());
        specialName_3->setText(QString());
        specialPrint_9->setText(QString());
        specialName_2->setText(QString());
        specialEnabled_6->setText(QString());
        specialPrint_5->setText(QString());
        specialPrint_2->setText(QString());
        specialEnabled_5->setText(QString());
        specialName_5->setText(QString());
        specialName_6->setText(QString());
        specialPrint_14->setText(QString());
        specialPrint_6->setText(QString());
        specialEnabled_7->setText(QString());
        specialEnabled_9->setText(QString());
        specialName_14->setText(QString());
        specialName_9->setText(QString());
        specialName_10->setText(QString());
        specialPrint_10->setText(QString());
        specialPrint_0->setText(QString());
        specialName_15->setText(QString());
        specialEnabled_14->setText(QString());
        specialName_16->setText(QString());
        specialEnabled_15->setText(QString());
        specialPrint_15->setText(QString());
        specialPrint_16->setText(QString());
        specialEnabled_16->setText(QString());
        alarmValName_2->setText(QString());
        alarmValName_1->setText(QString());
        alarmValName_0->setText(QString());
        portsTab->setTabText(portsTab->indexOf(portsTab1), QString());
        specialFieldsTab->setTabText(specialFieldsTab->indexOf(specialFieldsTab1), QString());
        tpLoadButton->setText(QApplication::translate("mainForm", "Load TP", 0, QApplication::UnicodeUTF8));
        tpLaunchButton->setText(QApplication::translate("mainForm", "Launch TP", 0, QApplication::UnicodeUTF8));
        tpLoadLaunchButton->setText(QApplication::translate("mainForm", "Load + Launch", 0, QApplication::UnicodeUTF8));
        tpLoadLaunchAllButton->setText(QApplication::translate("mainForm", "Load +\n"
"Launch ALL", 0, QApplication::UnicodeUTF8));
        tpLoadLaunchAllStop->setText(QString());
        cancelButton->setText(QApplication::translate("mainForm", "Cancel", 0, QApplication::UnicodeUTF8));
        menuGSS->setTitle(QApplication::translate("mainForm", "MASSIVA", 0, QApplication::UnicodeUTF8));
        menuLogs_folder->setTitle(QApplication::translate("mainForm", "MASSIVA logs folder", 0, QApplication::UnicodeUTF8));
        menuSend_Raw_Commands->setTitle(QApplication::translate("mainForm", "Send Raw Commands", 0, QApplication::UnicodeUTF8));
        menuSend_Protocol_Packets->setTitle(QApplication::translate("mainForm", "Send Protocol Packets", 0, QApplication::UnicodeUTF8));
        menuSend_Steps->setTitle(QApplication::translate("mainForm", "Send Steps", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("mainForm", "Help", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("mainForm", "View", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(mainForm);
    } // retranslateUi

};

namespace Ui {
    class mainForm: public Ui_mainForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFORM_H
