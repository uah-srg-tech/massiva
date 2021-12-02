#ifndef GSSCONFIG_H
#define GSSCONFIG_H

#include "../definitions.h"
#include <mutex>

class gssStructs
{
public:
    gssStructs();
    
    portConfig * getPointerToPorts();
    unsigned int getNumberOfPorts();
    portConfig * getPointerToPort(unsigned int portNumber);
    portTypes getPortType(unsigned int portNumber);
    const char * getPortName(unsigned int portNumber);
    const char * getUartName(unsigned int portNumber);
    ioTypes getPortIoType(unsigned int portNumber);
    const char * getPortDeviceInfo(unsigned int portNumber);
    unsigned int getPortPhyHeaderOffsetTM(unsigned int portNumber);
    unsigned int getPortPhyHeaderOffsetTC(unsigned int portNumber);

    gss_options * getPointerToOptions();
    enabled_status * getPrintPhyHeaderOptions();
    enabled_status getPhyHeaderShowGss();
    enabled_status * getDiscardErrorFlags();
    enabled_status getDiscardErrorFlagsIndex(unsigned int idx);
    
    portProtocol * getPointerToProtocols();
    portProtocol * getPointerToProtocol(unsigned int portNumber);
    bool checkPacketProtocolTypeSubtype(const unsigned char * packet,
            unsigned int length, unsigned int portNumber,
            unsigned int expectedType, unsigned int expectedSubtype);
    void printPacketProtocolTypeSubtype(const unsigned char * packet,
            unsigned int length, unsigned int portNumber, char * msg,
            unsigned int maxMsgSize, unsigned int * pMsgLen);
	
    gss_config * getPointerToConfig();
    
    level * getPointerToLevels(unsigned int iface);
    unsigned int getNumberOfLevels(unsigned int iface);
    level * getPointerToLevel(unsigned int iface, unsigned int level);

    test_proc * getPointerToProceduresList();
    unsigned int getNumberOfProcedures();
    test_proc * getPointerToProcedure(unsigned int idx);
    const char * getProcedureName(unsigned int idx);
    const char * getProcedureFilename(unsigned int idx);
    bool getProcedureEnabled(unsigned int idx);

    globalVar * getPointerToGlobalVars();
    unsigned int getNumberOfGlobalVars();
    const char * getGlobalVarName(unsigned int idx);
    double getGlobalVarValue(unsigned int idx);
    void incrementGlobalVarValue(unsigned int idx, double incrValue);
    void resetGlobalVarValue(unsigned int idx);
    void resetGlobalVarsValues();

    input * getPointerToPeriodicTCs();
    input * getPointerToPeriodicTC(unsigned int idx);
    unsigned int getNumberOfPeriodicTCs();
    const char * getPeriodicTCName(unsigned int idx);
    unsigned int getPeriodicTCPeriodMs(unsigned int idx);
    void setPeriodicTCPeriodMs(unsigned int idx, unsigned int newValue);
    unsigned int getPeriodicTCPort(unsigned int idx);
    
    protocolPacket * getPointerToProtocolPackets();
    protocolPacket * getPointerToProtocolPacket(unsigned int idx);
    unsigned int getNumberOfProtocolPackets();
    const char * getProtocolPacketName(unsigned int idx);
    
    special_packet_info * getPointerToSpecialInfos();
    special_packet_info * getPointerToSpecialInfo(unsigned int special);
    output * getPointerToSpecialPackets();
    levelOut * getPointerToLevelsSpecialPacket(unsigned int special);
    unsigned int getNumberOfSpecialPackets();
    unsigned char getSpecialInfoEnabled(unsigned int idx);
    void setSpecialInfoEnabled(unsigned int idx, unsigned char state);
    unsigned char getSpecialInfoPrint(unsigned int idx);
    void setSpecialInfoPrint(unsigned int idx, unsigned char state);
    unsigned char getSpecialInfoPeriod(unsigned int idx);
    void setSpecialInfoPeriod(unsigned int idx, unsigned char state);

    chart * getPointerToCharts();
    unsigned int getNumberOfCharts();
    monitor * getPointerToMonitors();
    unsigned int getNumberOfMonitors();
    monitor * getPointerToMonitor(unsigned int idx);
    typeOfGlobalVarRef getGlobalVarType(unsigned int idx);

    int getMainPort();
    int getMainPortAux();    
    
    stepStruct * getPointerToSteps();
    stepStruct ** getPointerToPointerToSteps();
    stepStruct * getPointerToStep(unsigned int stepNumber);
    char * getPointerToStepName(unsigned int stepNumber);
    void setNextStepWithoutOutputs(unsigned int stepNumber,
            unsigned int port, int value);
    double getStepValidTime(unsigned int stepNumber);
    unsigned int getNumberOfSteps();
    void setNumberOfSteps(unsigned int value);
    unsigned int * getPointerToNumberOfSteps();
    void freeSteps();

    unsigned int * getAllNotExpectedPackets();
    void incNotExpectedPackets(unsigned int rxPort);

    bool getAutoTest();
    void setAutoTest(bool value);
    
    int getCurrentTest();
    void setCurrentTest(int value);
    void currentTestMutexLock();
    void currentTestMutexUnlock();
    
    int UpdateGlobalVarTC(unsigned char * buffer,
            formatField * targetFields, unsigned int numberOfTargetFields,
            unsigned int port, unsigned int level);
    int UpdateGlobalVarTM(unsigned char * buffer, unsigned int port,
            int specialRef);
    
private:
    gss_config config;
    gss_options options;
    portProtocol protocols[MAX_PROTOCOLS];
    portConfig ports[MAX_INTERFACES];
    stepStruct * steps;
    unsigned int numberOfSteps;
        
    unsigned int notExpectedPackets[MAX_INTERFACES];
    
    bool autoTest;
    int currentTest;
    std::mutex currentTestMutex;
};

#endif /* GSSCONFIG_H */