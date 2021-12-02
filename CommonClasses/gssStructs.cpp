#include <cstdlib>
#include <cstdio>
#include <chrono>
#include "../CommonTools/GetSetFieldTools.h"
#include "gssStructs.h"

gssStructs::gssStructs()
{
    numberOfSteps = 0;
    steps = NULL;
    
    for(unsigned int port=0; port<MAX_INTERFACES; ++port)
    {
        notExpectedPackets[port] = 0;
    }
    
    autoTest = false;
    currentTest = -1;
    std::unique_lock<std::mutex> currentTestLock(currentTestMutex);
}

portConfig * gssStructs::getPointerToPorts()
{
    return ports;
}

unsigned int gssStructs::getNumberOfPorts()
{
    return config.numberOfPorts;
}

portConfig * gssStructs::getPointerToPort(unsigned int portNumber)
{
    return &ports[portNumber];
}

portTypes gssStructs::getPortType(unsigned int portNumber)
{
    return ports[portNumber].portType;
}

const char * gssStructs::getPortName(unsigned int portNumber)
{
    return ports[portNumber].name;
}

const char * gssStructs::getUartName(unsigned int portNumber)
{
    if(ports[portNumber].portType == UART_PORT)
        return (const char*)ports[portNumber].config.uart.portName;
    else
        return NULL;
}

ioTypes gssStructs::getPortIoType(unsigned int portNumber)
{
    return ports[portNumber].ioType;
}

const char * gssStructs::getPortDeviceInfo(unsigned int portNumber)
{
    return ports[portNumber].deviceInfo;
}

unsigned int gssStructs::getPortPhyHeaderOffsetTM(unsigned int portNumber)
{
    return ports[portNumber].portPhyHeaderOffsetTM;
}

unsigned int gssStructs::getPortPhyHeaderOffsetTC(unsigned int portNumber)
{
    return ports[portNumber].portPhyHeaderOffsetTC;
}

gss_options * gssStructs::getPointerToOptions()
{
    return &options;
}

enabled_status * gssStructs::getPrintPhyHeaderOptions()
{
    return options.phy_header_print;
}

enabled_status gssStructs::getPhyHeaderShowGss()
{
    return options.phy_header_gss_tabs;
}

enabled_status * gssStructs::getDiscardErrorFlags()
{
    return options.discardErrorFlags;
}

enabled_status gssStructs::getDiscardErrorFlagsIndex(unsigned int idx)
{
    return options.discardErrorFlags[idx];
}

portProtocol * gssStructs::getPointerToProtocols()
{
    return protocols;
}

portProtocol * gssStructs::getPointerToProtocol(unsigned int portNumber)
{
    return &protocols[ports[portNumber].protocolID];
}

bool gssStructs::checkPacketProtocolTypeSubtype(const unsigned char * packet,
        unsigned int length, unsigned int portNumber, 
        unsigned int expectedType, unsigned int expectedSubtype)
{
    bool ret = false;
    if((ports[portNumber].protocolID != -1) &&
            (protocols[ports[portNumber].protocolID].typeOffset < length))
    {
        if((expectedType == packet[protocols[ports[portNumber].protocolID].typeOffset]) &&
                ((protocols[ports[portNumber].protocolID].subtypeOffset == -1) ||
                (expectedSubtype = packet[protocols[ports[portNumber].protocolID].subtypeOffset])))
        {
            ret = true;
        }
    }
    return ret;
}

void gssStructs::printPacketProtocolTypeSubtype(const unsigned char * packet,
        unsigned int length, unsigned int portNumber, char * msg,
        unsigned int maxMsgSize, unsigned int * pMsgLen)
{
    if((ports[portNumber].protocolID != -1) &&
            (protocols[ports[portNumber].protocolID].typeOffset < length))
    {
        *pMsgLen += snprintf(&msg[*pMsgLen], maxMsgSize-*pMsgLen, " (%d",
                packet[protocols[ports[portNumber].protocolID].typeOffset]);
        if((protocols[ports[portNumber].protocolID].subtypeOffset != -1) &&
            (protocols[ports[portNumber].protocolID].subtypeOffset < (int)length))
        {
            *pMsgLen += snprintf(&msg[*pMsgLen], maxMsgSize-*pMsgLen, ".%d",
                    packet[protocols[ports[portNumber].protocolID].subtypeOffset]);
        }
        *pMsgLen += snprintf(&msg[*pMsgLen], maxMsgSize-*pMsgLen, ")");
    }
}

gss_config * gssStructs::getPointerToConfig()
{
    return &config;
}

level * gssStructs::getPointerToLevels(unsigned int iface)
{
    return config.levels[iface];
}

unsigned int gssStructs::getNumberOfLevels(unsigned int iface)
{
    return config.numberOfLevels[iface];
}

level * gssStructs::getPointerToLevel(unsigned int iface, unsigned int level = 0)
{
    return &config.levels[iface][level];
}

test_proc * gssStructs::getPointerToProceduresList()
{
    return config.procedures;
}

unsigned int gssStructs::getNumberOfProcedures()
{
    return config.numberOfProcedures;
}

test_proc * gssStructs::getPointerToProcedure(unsigned int idx)
{
    return &config.procedures[idx];
}

const char * gssStructs::getProcedureName(unsigned int idx)
{
    return config.procedures[idx].name;
}

const char * gssStructs::getProcedureFilename(unsigned int idx)
{
    return config.procedures[idx].filename;
}

bool gssStructs::getProcedureEnabled(unsigned int idx)
{
    return (bool)config.procedures[idx].enabled;
}

globalVar * gssStructs::getPointerToGlobalVars()
{
    return config.globalVars;
}

unsigned int gssStructs::getNumberOfGlobalVars()
{
    return config.numberOfGlobalVars;
}

const char * gssStructs::getGlobalVarName(unsigned int idx)
{
    return config.globalVars[idx].name;
}

double gssStructs::getGlobalVarValue(unsigned int idx)
{
    return config.globalVars[idx].value;
}

void gssStructs::incrementGlobalVarValue(unsigned int idx, double incrValue)
{
    config.globalVars[idx].value += incrValue;
}

void gssStructs::resetGlobalVarValue(unsigned int idx)
{
    config.globalVars[idx].value = 0.0;
}

void gssStructs::resetGlobalVarsValues()
{
    for(unsigned int i=0; i<config.numberOfGlobalVars; ++i)
    {
        config.globalVars[i].value = 0.0;
    }
}

input * gssStructs::getPointerToPeriodicTCs()
{
    return config.periodicTCs;
}

input * gssStructs::getPointerToPeriodicTC(unsigned int idx)
{
    return &config.periodicTCs[idx];
}

unsigned int gssStructs::getNumberOfPeriodicTCs()
{
    return config.numberOfPeriodicTCs;
}

const char * gssStructs::getPeriodicTCName(unsigned int idx)
{
    return config.periodicTCnames[idx];
}

unsigned int gssStructs::getPeriodicTCPeriodMs(unsigned int idx)
{
    return config.periodicTCs[idx].delayInMs;
}

void gssStructs::setPeriodicTCPeriodMs(unsigned int idx, unsigned int newValue)
{
    config.periodicTCs[idx].delayInMs = newValue;
}

unsigned int gssStructs::getPeriodicTCPort(unsigned int idx)
{
    return config.periodicTCs[idx].ifRef;
}

protocolPacket * gssStructs::getPointerToProtocolPackets()
{
    return config.protocolPackets;
}

unsigned int gssStructs::getNumberOfProtocolPackets()
{
    return config.numberOfProtocolPackets;
}

protocolPacket * gssStructs::getPointerToProtocolPacket(unsigned int idx)
{
    return &config.protocolPackets[idx];
}

const char * gssStructs::getProtocolPacketName(unsigned int idx)
{
    return config.protocolPackets[idx].name;
}

special_packet_info * gssStructs::getPointerToSpecialInfos()
{
    return config.specialInfo;
}

special_packet_info * gssStructs::getPointerToSpecialInfo(unsigned int special)
{
    return &config.specialInfo[special];
}

output * gssStructs::getPointerToSpecialPackets()
{
    return config.specialPackets;
}

levelOut * gssStructs::getPointerToLevelsSpecialPacket(unsigned int special)
{
    return config.specialPackets[special].level;
}

unsigned int gssStructs::getNumberOfSpecialPackets()
{
    return config.numberOfSpecialPackets;
}

unsigned char gssStructs::getSpecialInfoEnabled(unsigned int idx)
{
    return config.specialInfo[idx].enabled;
}

void gssStructs::setSpecialInfoEnabled(unsigned int idx, unsigned char state)
{
    config.specialInfo[idx].enabled = state;
}

unsigned char gssStructs::getSpecialInfoPrint(unsigned int idx)
{
    return config.specialInfo[idx].print_enabled;
}

void gssStructs::setSpecialInfoPrint(unsigned int idx, unsigned char state)
{
    config.specialInfo[idx].print_enabled = state;
}

unsigned char gssStructs::getSpecialInfoPeriod(unsigned int idx)
{
    return config.specialInfo[idx].period_enabled;
}

void gssStructs::setSpecialInfoPeriod(unsigned int idx, unsigned char state)
{
    config.specialInfo[idx].period_enabled = state;
}

chart * gssStructs::getPointerToCharts()
{
    return config.charts;
}

unsigned int gssStructs::getNumberOfCharts()
{
    return config.numberOfCharts;
}

monitor * gssStructs::getPointerToMonitors()
{
    return config.monitors;
}

unsigned int gssStructs::getNumberOfMonitors()
{
    return config.numberOfMonitors;
}

monitor * gssStructs::getPointerToMonitor(unsigned int idx)
{
    return &config.monitors[idx];
}

typeOfGlobalVarRef gssStructs::getGlobalVarType(unsigned int idx)
{
    return config.monitors[idx].typeOfRef;
}

int gssStructs::getMainPort()
{
    return config.mainPort;
}

int gssStructs::getMainPortAux()
{
    return config.mainPortAux;
}

stepStruct * gssStructs::getPointerToSteps()
{
    return steps;
}

stepStruct ** gssStructs::getPointerToPointerToSteps()
{
    return &steps;
}

stepStruct * gssStructs::getPointerToStep(unsigned int stepNumber)
{
    return &steps[stepNumber];
}

char * gssStructs::getPointerToStepName(unsigned int stepNumber)
{
    return steps[stepNumber].name;
}
    
void gssStructs::setNextStepWithoutOutputs(unsigned int stepNumber,
        unsigned int port, int value)
{
    steps[stepNumber].nextStepWithOutputsAtPort[port] = value;
}

double gssStructs::getStepValidTime(unsigned int stepNumber)
{
    return steps[stepNumber].intervalInMs;
}

unsigned int gssStructs::getNumberOfSteps()
{
    return numberOfSteps;
}

void gssStructs::setNumberOfSteps(unsigned int value)
{
    numberOfSteps = value;
    return;
}

unsigned int * gssStructs::getPointerToNumberOfSteps()
{
    return &numberOfSteps;
}

void gssStructs::freeSteps()
{
    unsigned int stp, io, lvl, fld;
    for(stp=0; stp<numberOfSteps; ++stp)
    {
        if(steps[stp].inputs != NULL)
        {
            for(io=0; io<steps[stp].numberOfInputs; ++io)
            {
                if(steps[stp].inputs[io].level != NULL)
                {
                    for(lvl=0; lvl<steps[stp].inputs[io].numberOfLevels; ++lvl)
                    {
                        if(steps[stp].inputs[io].level[lvl].TCFields != NULL)
                            free(steps[stp].inputs[io].level[lvl].TCFields);
                        if(steps[stp].inputs[io].level[lvl].crcTCFieldRefs != NULL)
                        {
                            for(fld=0; fld<steps[stp].inputs[io].level[lvl].numberOfFDICTCFields; ++fld)
                            {
                                if(steps[stp].inputs[io].level[lvl].crcTCFieldRefs[fld] != NULL)
                                    free(steps[stp].inputs[io].level[lvl].crcTCFieldRefs[fld]);
                            }
                            free(steps[stp].inputs[io].level[lvl].crcTCFieldRefs);
                        }
                        if(steps[stp].inputs[io].level[lvl].exportFields != NULL)
                            free(steps[stp].inputs[io].level[lvl].exportFields);
                        if(steps[stp].inputs[io].level[lvl].ActiveDICs != NULL)
                            free(steps[stp].inputs[io].level[lvl].ActiveDICs);
                    }
                    free(steps[stp].inputs[io].level);
                }
            }
            free(steps[stp].inputs);
            steps[stp].numberOfInputs = 0;
        }
        if(steps[stp].outputs != NULL)
        {
            for(io=0; io<steps[stp].numberOfOutputs; ++io)
            {
                if(steps[stp].outputs[io].level != NULL)
                {
                    for(lvl=0; lvl<steps[stp].outputs[io].numberOfLevels; ++lvl)
                    {
                        if(steps[stp].outputs[io].level[lvl].TMFields != NULL)
                            free(steps[stp].outputs[io].level[lvl].TMFields);
                        if(steps[stp].outputs[io].level[lvl].crcTMFieldRefs != NULL)
                        {
                            for(fld=0; fld<steps[stp].outputs[io].level[lvl].numberOfFDICTMFields; ++fld)
                            {
                                if(steps[stp].outputs[io].level[lvl].crcTMFieldRefs[fld] != NULL)
                                    free(steps[stp].outputs[io].level[lvl].crcTMFieldRefs[fld]);
                            }
                            free(steps[stp].outputs[io].level[lvl].crcTMFieldRefs);
                        }
                        if(steps[stp].outputs[io].level[lvl].boolVars != NULL)
                            free(steps[stp].outputs[io].level[lvl].boolVars);
                        if(steps[stp].outputs[io].level[lvl].filters != NULL)
                        {
                            for(fld=0; fld<steps[stp].outputs[io].level[lvl].numberOfFilters; ++fld)
                            {
                                if(steps[stp].outputs[io].level[lvl].filters[fld].boolVarRef != NULL)
                                    free(steps[stp].outputs[io].level[lvl].filters[fld].boolVarRef);
                            }
                            free(steps[stp].outputs[io].level[lvl].filters);
                        }
                        if(steps[stp].outputs[io].level[lvl].virtualFields != NULL)
                            free(steps[stp].outputs[io].level[lvl].virtualFields);
                    }
                    free(steps[stp].outputs[io].level);
                }
            }
            free(steps[stp].outputs);
            steps[stp].numberOfOutputs = 0;
        }
        if(steps[stp].specials != NULL)
        {
            free(steps[stp].specials);
            steps[stp].numberOfSpecials = 0;
        }
        if(steps[stp].concurrent.list != NULL)
        {
            free(steps[stp].concurrent.list);
            steps[stp].concurrent.number = 0;
        }
        if(steps[stp].action != NULL)
        {
            free(steps[stp].action);
        }
    }
    if(steps != NULL)
    {
        free(steps);
        steps = NULL;
    }
    numberOfSteps = 0;
    return;
}

unsigned int * gssStructs::getAllNotExpectedPackets()
{
    return notExpectedPackets;
}

void gssStructs::incNotExpectedPackets(unsigned int rxPort)
{
    notExpectedPackets[rxPort]++;
}

bool gssStructs::getAutoTest()
{
    return autoTest;
}

void gssStructs::setAutoTest(bool value)
{
    autoTest = value;
}

int gssStructs::getCurrentTest()
{
    return currentTest;
}

void gssStructs::setCurrentTest(int value)
{
    currentTest = value;
}

void gssStructs::currentTestMutexLock()
{
    currentTestMutex.lock();
}

void gssStructs::currentTestMutexUnlock()
{
    currentTestMutex.unlock();
}

int gssStructs::UpdateGlobalVarTC(unsigned char * buffer,
        formatField * targetFields, unsigned int numberOfTargetFields,
        unsigned int port, unsigned int level)
{
    int lastGvUpdated = 0;
    if(config.globalVars == NULL)
        return -1;
    for(unsigned int gv=0; gv<config.numberOfGlobalVars; ++gv)
    {
        unsigned int fld = config.globalVars[gv].fieldRef;
        if(((config.globalVars[gv].commandType == globalVar::GLOBAL_TC) ||
                (config.globalVars[gv].commandType == globalVar::GLOBAL_TC_PERIOD)) &&
                (port == config.globalVars[gv].Interface) &&
                (level == config.globalVars[gv].level))
        {
            if(fld >= numberOfTargetFields)
            {
                continue;
            }
            else if(!targetFields[fld].exported)
            {
                SettingNumber((unsigned long long)config.globalVars[gv].value,
                        buffer, targetFields[fld].totalSizeInBits,
                        targetFields[fld].offsetInBits);
                lastGvUpdated = (int) gv;
            }
        }
    }
    return lastGvUpdated;
}

int gssStructs::UpdateGlobalVarTM(unsigned char * buffer, unsigned int port,
        int specialRef)
{
    int lastGvUpdated = 0;
    levelOut * pLevelOut = NULL;
    if(config.globalVars == NULL)
        return -1;
    for(unsigned int gv=0; gv<config.numberOfGlobalVars; ++gv)
    {
        if((port == config.globalVars[gv].Interface) &&
                ((config.globalVars[gv].commandType == globalVar::GLOBAL_TM) ||
                ((config.globalVars[gv].commandType == globalVar::GLOBAL_TM_SPECIAL) &&
                (config.globalVars[gv].idRef == specialRef))))
        {
            unsigned int fld = config.globalVars[gv].fieldRef;
            unsigned int importInBytes = 0;
            
            for(unsigned int level=0; level<MAX_LEVELS; ++level)
            {
                if(config.globalVars[gv].commandType == globalVar::GLOBAL_TM)
                {
                    importInBytes += config.levels[config.globalVars[gv].Interface][level].out.importInBytes;
                }
                else if (config.globalVars[gv].commandType == globalVar::GLOBAL_TM_SPECIAL)
                {
                    if(config.specialPackets[config.globalVars[gv].idRef].level[level].numberOfTMFields == 0)
                        importInBytes += config.levels[config.globalVars[gv].Interface][level].out.importInBytes;
                    else
                        importInBytes += config.specialPackets[config.globalVars[gv].idRef].level[level].importInBytes;
                }
                if(level == config.globalVars[gv].level)
                {
                    if(config.globalVars[gv].commandType == globalVar::GLOBAL_TM)
                    {
                        pLevelOut = &config.levels[config.globalVars[gv].Interface][level].out;
                    }
                    else if (config.globalVars[gv].commandType == globalVar::GLOBAL_TM_SPECIAL)
                    {
                        pLevelOut = &config.specialPackets[config.globalVars[gv].idRef].level[level];
                    }
                    unsigned long long value = 0;
                    GetFieldFromBufferAsUllong(&buffer[importInBytes],
                            0, MAX_PACKET_SIZE,  pLevelOut->TMFields[fld].offsetInBits,
                            pLevelOut->TMFields[fld].totalSizeInBits, &value);
                    if(config.globalVars[gv].dataType == globalVar::GV_UINT)
                    {
                        config.globalVars[gv].value = (double)value;
                    }
                    else if(config.globalVars[gv].dataType == globalVar::GV_FORMULA)
                    {
                        config.globalVars[gv].value = (double)value
                                * pLevelOut->TMFields[fld].info.formula.slope
                                + pLevelOut->TMFields[fld].info.formula.intercept;
                    }
                }
            }
            lastGvUpdated = (int) gv;
        }
    }
    return lastGvUpdated;
}