/**
 * \file	RxPacket.h
 * \brief	functions for send and receive packets (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/01/2014 at 15:30:23
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef RXPACKET_H
#define RXPACKET_H

#include "../CommonClasses/Logs.h"
#include "../CheckTools/ProcessPacket.h"
#include "TestManager.h"

class RxPacket
{
public:
    RxPacket(gssStructs * origGssStruct, Logs * origLogs,
            TestManager * origTestMgr, const unsigned char * InQueue,
            unsigned int length, unsigned int portPhyHeaderOffset,
            unsigned int rxPort);
    int CheckProcessPacket(int &specialIndex, bool &notExpected);
    bool GetStepInfo(unsigned int &step, unsigned int &numberOfOutputs,
            unsigned int &outputsReceived, bool &finished);
    void GetNotExpectedInfoMsg(char * auxMsg, unsigned int auxMsg_max_len);

private:
    gssStructs * pGssStruct;
    Logs * pLogs;
    TestManager * pTestMgr;
    
    const unsigned char * packet;
    unsigned int packetLength;
    unsigned int phyHeaderOffset;
    unsigned int port;
    int currentRxStepNum;
    const unsigned char * dataPacket;
    
    unsigned int dataLength;
    stepStruct * currentStepHndl;
    double currentTimeMs;
    unsigned int notExpectedMsgLen;
    bool testFinished;
    
    char msg[MAX_MSG_SIZE];
    ProcessPacket process;
    
    void RxSpecialPacket(special_packet_info * spInfo);
    int CheckNotExpectedPacket();
    void InitFilterResults(filterResults * pResults,
            unsigned int numberOfLevels);
    void PrintFilterResults(filterResults * pResults, double validTimeMs,
            int currentOutput, const char * portName,
            const level * defaultLevels, output * expectedOutputHndl);
    void DeleteFilterResults(filterResults * pResults,
            const level * defaultLevels, output * expectedPacket);
};

#endif /* RXPACKET_H */