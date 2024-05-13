/********************************************************************************
** Form generated from reading UI file 'selectProtocolPacket.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTPROTOCOLPACKET_H
#define UI_SELECTPROTOCOLPACKET_H

#include <QtCore/QVariant>
#if QT_VERSION < 0x050000
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#else
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#endif

QT_BEGIN_NAMESPACE

class Ui_selectProtocolPacket
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *selectProtocolPacket)
    {
        if (selectProtocolPacket->objectName().isEmpty())
            selectProtocolPacket->setObjectName(QString::fromUtf8("selectProtocolPacket"));
        selectProtocolPacket->resize(248, 90);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(selectProtocolPacket->sizePolicy().hasHeightForWidth());
        selectProtocolPacket->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/nuts.ico"), QSize(), QIcon::Normal, QIcon::Off);
        selectProtocolPacket->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(selectProtocolPacket);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);

        verticalLayout->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        okButton = new QPushButton(selectProtocolPacket);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(okButton->sizePolicy().hasHeightForWidth());
        okButton->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(okButton);

        cancelButton = new QPushButton(selectProtocolPacket);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        sizePolicy1.setHeightForWidth(cancelButton->sizePolicy().hasHeightForWidth());
        cancelButton->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(cancelButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(selectProtocolPacket);

        QMetaObject::connectSlotsByName(selectProtocolPacket);
    } // setupUi

    void retranslateUi(QDialog *selectProtocolPacket)
    {
        selectProtocolPacket->setWindowTitle(QApplication::translate("selectProtocolPacket", "Select Protocol Packet", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("selectProtocolPacket", "OK", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("selectProtocolPacket", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class selectProtocolPacket: public Ui_selectProtocolPacket {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTPROTOCOLPACKET_H
