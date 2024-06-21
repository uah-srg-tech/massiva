/********************************************************************************
** Form generated from reading UI file 'selectRawFileForm.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTRAWFILEFORM_H
#define UI_SELECTRAWFILEFORM_H

#include <QtCore/QVariant>
#if QT_VERSION < 0x050000
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
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
#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#endif

QT_BEGIN_NAMESPACE

class Ui_selectRawFileForm
{
public:
    QPushButton *selectFileButton;
    QLineEdit *fileLineEdit;
    QLabel *linesCountLabel;
    QLabel *intervalLabel;
    QSpinBox *intervalBox;
    QCheckBox *loopBox;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QLCDNumber *commandsCount;
    QCheckBox *dicBox;
    QCheckBox *gvBox;

    void setupUi(QDialog *selectRawFileForm)
    {
        if (selectRawFileForm->objectName().isEmpty())
            selectRawFileForm->setObjectName(QString::fromUtf8("selectRawFileForm"));
        selectRawFileForm->resize(400, 170);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(selectRawFileForm->sizePolicy().hasHeightForWidth());
        selectRawFileForm->setSizePolicy(sizePolicy);
        selectRawFileForm->setMaximumSize(QSize(400, 170));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/nuts.ico"), QSize(), QIcon::Normal, QIcon::Off);
        selectRawFileForm->setWindowIcon(icon);
        selectFileButton = new QPushButton(selectRawFileForm);
        selectFileButton->setObjectName(QString::fromUtf8("selectFileButton"));
        selectFileButton->setGeometry(QRect(10, 30, 75, 23));
        fileLineEdit = new QLineEdit(selectRawFileForm);
        fileLineEdit->setObjectName(QString::fromUtf8("fileLineEdit"));
        fileLineEdit->setGeometry(QRect(10, 10, 381, 21));
        fileLineEdit->setReadOnly(true);
        linesCountLabel = new QLabel(selectRawFileForm);
        linesCountLabel->setObjectName(QString::fromUtf8("linesCountLabel"));
        linesCountLabel->setGeometry(QRect(310, 35, 31, 16));
        intervalLabel = new QLabel(selectRawFileForm);
        intervalLabel->setObjectName(QString::fromUtf8("intervalLabel"));
        intervalLabel->setGeometry(QRect(120, 60, 66, 16));
        intervalBox = new QSpinBox(selectRawFileForm);
        intervalBox->setObjectName(QString::fromUtf8("intervalBox"));
        intervalBox->setGeometry(QRect(185, 56, 91, 22));
        intervalBox->setMaximum(60000);
        intervalBox->setValue(50);
        loopBox = new QCheckBox(selectRawFileForm);
        loopBox->setObjectName(QString::fromUtf8("loopBox"));
        loopBox->setGeometry(QRect(280, 100, 101, 17));
        okButton = new QPushButton(selectRawFileForm);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setGeometry(QRect(90, 140, 75, 23));
        cancelButton = new QPushButton(selectRawFileForm);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setGeometry(QRect(220, 140, 75, 23));
        commandsCount = new QLCDNumber(selectRawFileForm);
        commandsCount->setObjectName(QString::fromUtf8("commandsCount"));
        commandsCount->setEnabled(true);
        commandsCount->setGeometry(QRect(340, 30, 51, 21));
        commandsCount->setFrameShape(QFrame::Box);
        commandsCount->setFrameShadow(QFrame::Plain);
        commandsCount->setSegmentStyle(QLCDNumber::Flat);
        dicBox = new QCheckBox(selectRawFileForm);
        dicBox->setObjectName(QString::fromUtf8("dicBox"));
        dicBox->setGeometry(QRect(40, 100, 101, 17));
        gvBox = new QCheckBox(selectRawFileForm);
        gvBox->setObjectName(QString::fromUtf8("gvBox"));
        gvBox->setGeometry(QRect(150, 100, 101, 17));

        retranslateUi(selectRawFileForm);

        QMetaObject::connectSlotsByName(selectRawFileForm);
    } // setupUi

    void retranslateUi(QDialog *selectRawFileForm)
    {
        selectRawFileForm->setWindowTitle(QApplication::translate("selectRawFileForm", "Send Raw Command", 0));
        selectFileButton->setText(QApplication::translate("selectRawFileForm", "Select File", 0));
        linesCountLabel->setText(QApplication::translate("selectRawFileForm", "Lines", 0));
        intervalLabel->setText(QApplication::translate("selectRawFileForm", "Interval (ms)", 0));
        loopBox->setText(QApplication::translate("selectRawFileForm", "Send in loop", 0));
        okButton->setText(QApplication::translate("selectRawFileForm", "OK", 0));
        cancelButton->setText(QApplication::translate("selectRawFileForm", "Cancel", 0));
        dicBox->setText(QApplication::translate("selectRawFileForm", "Perform DIC", 0));
        gvBox->setText(QApplication::translate("selectRawFileForm", "Check GlobalVars", 0));
    } // retranslateUi

};

namespace Ui {
    class selectRawFileForm: public Ui_selectRawFileForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTRAWFILEFORM_H
