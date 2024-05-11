# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = Release
TARGET = massiva
VERSION = 4.0.0.7
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += release 
PKGCONFIG +=
QT = core gui widgets opengl
SOURCES += CheckTools/CheckFilterTools.c CheckTools/CheckFormatTools.c CheckTools/ExportSettingTools.c CheckTools/ImportFields.c CheckTools/ProcessPacket.cpp CommonClasses/InitialConfig.cpp CommonClasses/Logs.cpp CommonClasses/MathTools.cpp CommonClasses/gssStructs.cpp CommonTools/GetSetFieldTools.c CommonTools/crc16.c Forms/AboutDialog.cpp Forms/PeriodicTCsDialog.cpp Forms/SpWTimeCodesDialog.cpp Forms/mainForm.cpp Forms/plotForm.cpp Forms/resetGlobalVariables.cpp Forms/selectProtocolPacket.cpp Forms/selectRawFileForm.cpp GuiClasses/CheckSpecialPeriods.cpp GuiClasses/DoubleClickButton.cpp GuiClasses/HexSpinBox.cpp GuiClasses/MonitorPlots.cpp GuiClasses/ProgressBarsWrapper.cpp GuiClasses/RxTxInfo.cpp GuiClasses/SpecialPackets.cpp GuiClasses/TestButtons.cpp PortTools/configScenario.c PortTools/configSerial.c PortTools/configSpW.c PortTools/configSpWpci.c PortTools/configSpWusb.c PortTools/configSpWusbmk2.c PortTools/configTCPSocket.c PortTools/configUDPSocket.c PortTools/raw.c PortTools/rawProtocol.c PortTools/rawSerial.c PortTools/rawSpWmk2.c PortTools/rawSpWpci.c PortTools/rawSpWusb.c PortTools/rawTCPSocket.c PortTools/rawUDPSocket.c TesterClasses/AutoTester.cpp TesterClasses/EnqueueWorker.cpp TesterClasses/PeriodicMonitor.cpp TesterClasses/PeriodicTC.cpp TesterClasses/PrepareInput.cpp TesterClasses/RxPacket.cpp TesterClasses/TPWorker.cpp TesterClasses/TestManager.cpp TesterClasses/TxStep.cpp TesterClasses/sendRaw.cpp XMLParsingTools/XMLEgseConfig.c XMLParsingTools/XMLEgseConfigFileParser.c XMLParsingTools/XMLEgseConfigOptions.c XMLParsingTools/XMLEgseGlobalVars.c XMLParsingTools/XMLEgseMonitors.c XMLParsingTools/XMLEgsePeriodicTCs.c XMLParsingTools/XMLExImTools.c XMLParsingTools/XMLExportSettingTools.c XMLParsingTools/XMLExportTools.c XMLParsingTools/XMLFilterTools.c XMLParsingTools/XMLFormatTools.c XMLParsingTools/XMLImportTools.c XMLParsingTools/XMLInterfaceTools.c XMLParsingTools/XMLLevelTools.c XMLParsingTools/XMLPortTools.c XMLParsingTools/XMLProcedureListTools.c XMLParsingTools/XMLProtocolPacketTools.c XMLParsingTools/XMLSpecialPacketTools.c XMLParsingTools/XMLTPInputTools.c XMLParsingTools/XMLTPOutputTools.c XMLParsingTools/XMLTPStepTools.c XMLParsingTools/XMLTPTools.c XMLParsingTools/XMLTools.c main.cpp
HEADERS += CheckTools/CheckFilterTools.h CheckTools/CheckFormatTools.h CheckTools/ExportSettingTools.h CheckTools/ImportFields.h CheckTools/ProcessPacket.h CommonClasses/InitialConfig.h CommonClasses/Logs.h CommonClasses/MathTools.h CommonClasses/gssStructs.h CommonTools/GetSetFieldTools.h CommonTools/crc16.h Forms/AboutDialog.h Forms/PeriodicTCsDialog.h Forms/SpWTimeCodesDialog.h Forms/mainForm.h Forms/plotForm.h Forms/resetGlobalVariables.h Forms/selectProtocolPacket.h Forms/selectRawFileForm.h GuiClasses/CheckSpecialPeriods.h GuiClasses/DoubleClickButton.h GuiClasses/HexSpinBox.h GuiClasses/MonitorPlots.h GuiClasses/ProgressBarsWrapper.h GuiClasses/RxTxInfo.h GuiClasses/SpecialPackets.h GuiClasses/TestButtons.h PortTools/configScenario.h PortTools/configSerial.h PortTools/configSpW.h PortTools/configSpWpci.h PortTools/configSpWusb.h PortTools/configSpWusbmk2.h PortTools/configTCPSocket.h PortTools/configUDPSocket.h PortTools/raw.h PortTools/rawProtocol.h PortTools/rawSerial.h PortTools/rawSpWmk2.h PortTools/rawSpWpci.h PortTools/rawSpWusb.h PortTools/rawTCPSocket.h PortTools/rawUDPSocket.h TesterClasses/AutoTester.h TesterClasses/EnqueueWorker.h TesterClasses/PeriodicMonitor.h TesterClasses/PeriodicTC.h TesterClasses/PrepareInput.h TesterClasses/RxPacket.h TesterClasses/TPWorker.h TesterClasses/TestManager.h TesterClasses/TxStep.h TesterClasses/sendRaw.h XMLParsingTools/XMLEgseConfig.h XMLParsingTools/XMLEgseConfigFileParser.h XMLParsingTools/XMLEgseConfigOptions.h XMLParsingTools/XMLEgseGlobalVars.h XMLParsingTools/XMLEgseMonitors.h XMLParsingTools/XMLEgsePeriodicTCs.h XMLParsingTools/XMLExImTools.h XMLParsingTools/XMLExportSettingTools.h XMLParsingTools/XMLExportTools.h XMLParsingTools/XMLFilterTools.h XMLParsingTools/XMLFormatTools.h XMLParsingTools/XMLImportTools.h XMLParsingTools/XMLInterfaceTools.h XMLParsingTools/XMLLevelTools.h XMLParsingTools/XMLPortTools.h XMLParsingTools/XMLProcedureListTools.h XMLParsingTools/XMLProtocolPacketTools.h XMLParsingTools/XMLSpecialPacketTools.h XMLParsingTools/XMLTPInputTools.h XMLParsingTools/XMLTPOutputTools.h XMLParsingTools/XMLTPStepTools.h XMLParsingTools/XMLTPTools.h XMLParsingTools/XMLTools.h definitions.h
FORMS += Forms/AboutDialog.ui Forms/PeriodicTCsDialog.ui Forms/SpWTimeCodesDialog.ui Forms/mainForm.ui Forms/plotForm.ui Forms/resetGlobalVariables.ui Forms/selectProtocolPacket.ui Forms/selectRawFileForm.ui
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Release/MinGW-Windows
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += LIBXML_STATIC NOT_PCI_DEV PLOTS 
INCLUDEPATH += "/C/Program Files/STAR-Dundee/SpaceWire USB Driver/code/inc" "/C/Program Files/STAR-Dundee/STAR-System/inc/star" 
LIBS += -lxml2 -liconv -lws2_32 -lpthread "C:/Program Files/STAR-Dundee/STAR-System/lib/x86-32/star-api.lib" "C:/Program Files/STAR-Dundee/STAR-System/lib/x86-32/star_conf_api_brick_mk2.lib" "C:/Program Files/STAR-Dundee/STAR-System/lib/x86-32/star_conf_api_mk2.lib" "C:/Program Files/STAR-Dundee/STAR-System/lib/x86-32/star_conf_api_router.lib" "C:/Program Files/STAR-Dundee/SpaceWire USB Driver/code/lib/SpaceWireUSBAPI.lib" "C:/Program Files/STAR-Dundee/SpaceWire USB Driver/code/lib/ConfigLibraryUSB.lib" C:/Qwt-6.1.3/lib/libqwt.a  
equals(QT_MAJOR_VERSION, 4) {
QMAKE_CXXFLAGS += -std=c++11
}
equals(QT_MAJOR_VERSION, 5) {
CONFIG += c++11
}
RESOURCES += icons.qrc logos.qrc
UI_DIR += Forms
CONFIG += qwt
RC_FILE = ../icon.rc
