/**
 * \file	RxPacket.c
 * \brief	functions for send and receive packets (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		27/11/2012 at 15:37:19
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c)2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <cstdio>				/* sprintf */
#include <cstring>
#include <time.h>				/* clock_gettime */
#include "RxPacket.h"

RxPacket::RxPacket(gssStructs * origGssStruct, Logs * origLogs,
        TestManager * origTestMgr, const unsigned char * InQueue,
        unsigned int length, unsigned int portPhyHeaderOffset,
        unsigned int rxPort)
{
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pTestMgr = origTestMgr;
    
    packet = InQueue;
    packetLength = length;
    phyHeaderOffset = portPhyHeaderOffset;
    port = rxPort;
    currentRxStepNum = pTestMgr->getRxStep(port);
    dataPacket = &packet[phyHeaderOffset];
    
    dataLength = 0;
    currentStepHndl = NULL;
    notExpectedMsgLen = 0;
    testFinished = false;
    
    struct timespec ref;
    clock_gettime(CLOCK_MONOTONIC, &ref);
    currentTimeMs = (ref.tv_sec * 1000.0) + (ref.tv_nsec / 1000000.0);
}

int RxPacket::CheckProcessPacket(int &specialIndex, bool &notExpected)
{    
    int status = 0;
    
    status = process.checkIfSpecial(dataPacket, dataLength,
            pGssStruct->getNumberOfSpecialPackets(),
            pGssStruct->getPointerToLevels(port),
            pGssStruct->getPointerToSpecialPackets(),
            pGssStruct->getPointerToSpecialInfos(), specialIndex, port);
    if(status < 0)
    {
        char title[60] = "";
        process.printError(status, title, msg, MAX_MSG_SIZE, specialIndex, 
                currentRxStepNum, -1, pGssStruct->getPortName(port), port);
        pLogs->saveMsgToLog(port, title, msg);
        pLogs->savePacketMsgToLogs(port, "\tData: ", packet, packetLength,
                phyHeaderOffset);
        specialIndex = -1;
        return -1;
    }
    if(specialIndex != -1)
    {
        special_packet_info * spInfo =
                pGssStruct->getPointerToSpecialInfo(specialIndex);
        RxSpecialPacket(spInfo);
    }
    else //if not Special, it is due to TP or unexpected 
    {
        unsigned int currentOutput = 0, tries = 0;
        if(currentRxStepNum == -1)
        {
            /* if no steps defined or TP finished, */
            /* something has been received and it wasn't expected */
            notExpected = true;
            status = CheckNotExpectedPacket();
        }
        else
        {
            /* check if the valid time of the output */
            double validTimeMs = pTestMgr->checkOutputValidTime(port,
                    currentRxStepNum, currentTimeMs);
            
            currentStepHndl = pGssStruct->getPointerToStep(currentRxStepNum);
        
            if(currentStepHndl->outputsCheckmode == stepStruct::ALL)
            {
                unsigned int out = 0;
                for(out=0; out<currentStepHndl->numberOfOutputs; ++out)
                {
                    if((currentStepHndl->outputs[out].ifRef == port) &&
                            (!currentStepHndl->outputs[out].received))
                    {
                        currentOutput = out;
                        break;
                    }
                }
                if(out == currentStepHndl->numberOfOutputs)
                {
                    notExpected = true;
                    status = CheckNotExpectedPacket();
                }
                else
                {
                    filterResults results;
                    unsigned int correctLevels = 0;
                    InitFilterResults(&results,
                            currentStepHndl->outputs[currentOutput].numberOfLevels);
                    status = process.Expected(dataPacket, dataLength,
                            pGssStruct->getPointerToLevels(port),
                            &currentStepHndl->outputs[currentOutput], correctLevels,
                            port, &results);
                    /* 
                     * if all levels correct until type level is a first similar packet (equal type+subtype)
                     * and it is also printed
                     */
                    if((status >= 0) &&
                            ((correctLevels == (currentStepHndl->outputs[currentOutput].numberOfLevels-1)) ||
                            (pGssStruct->checkPacketProtocolTypeSubtype(dataPacket,
                            dataLength, port, currentStepHndl->outputs[currentOutput].type,
                            currentStepHndl->outputs[currentOutput].subtype) == true)))
                    {
                        /* if not found, increase errors in filters counter */
                        if(correctLevels != (currentStepHndl->outputs[currentOutput].numberOfLevels-1))
                            pTestMgr->incFiltersKoCurrentTest();
                        /* print filter results */                        
                        PrintFilterResults(&results, validTimeMs, currentOutput,
                                pGssStruct->getPortName(port),
                                pGssStruct->getPointerToLevels(port), 
                                &currentStepHndl->outputs[currentOutput]);
                        /* increase outputs received counter */
                        currentStepHndl->outputsReceived++;
                        currentStepHndl->outputsReceivedAtPort[port]++;
                    }
                    else
                    {
                        notExpected = true;
                        status = CheckNotExpectedPacket();
                    }
                    DeleteFilterResults(&results,
                            pGssStruct->getPointerToLevels(port),
                            &currentStepHndl->outputs[currentOutput]);
                }
            }
            else if((currentStepHndl->outputsCheckmode == stepStruct::ANY) ||
                     (currentStepHndl->outputsCheckmode == stepStruct::ALLUNSORTED))
            {
                int firstSimilarOutput = -1;
                filterResults results[currentStepHndl->numberOfOutputs];
                for(tries=0; tries<currentStepHndl->numberOfOutputs; ++tries)
                {
                    results[tries].defaultResult = NULL;
                }
                for(tries=0; tries<currentStepHndl->numberOfOutputs; ++tries)
                {
                    if(((currentStepHndl->outputsCheckmode == stepStruct::ALLUNSORTED) &&
                            (currentStepHndl->outputs[tries].received)) ||
                            (currentStepHndl->outputs[tries].ifRef != port))
                    {
                        continue;
                    }
                    InitFilterResults(&results[tries],
                            currentStepHndl->outputs[tries].numberOfLevels);
                    unsigned int correctLevels = 0;
                    status = process.Expected(dataPacket, dataLength,
                            pGssStruct->getPointerToLevels(port),
                            &currentStepHndl->outputs[tries], correctLevels, port,
                            &results[tries]);
                    /* 
                     * if error (status < 0) or all filters correct (status == 1)
                     * exit from tries for loop
                     * else if first similar packet (equal type+subtype) mark it
                     */
                    if((status < 0) || (status == 1))
                    {
                        break;
                    }
                    else if((firstSimilarOutput == -1) &&
                            (pGssStruct->checkPacketProtocolTypeSubtype(dataPacket,
                            dataLength, port, currentStepHndl->outputs[tries].type,
                            currentStepHndl->outputs[tries].subtype) == true))
                    {
                        firstSimilarOutput = (int)tries;
                    }
                }
                if((status == 1) || (firstSimilarOutput != -1))
                {
                    /* if found (status == 1) or firstSimilarOutput, print it */
                    if(status == 1)
                    {
                        currentOutput = tries;
                    }
                    else
                    {
                        currentOutput = firstSimilarOutput;
                        pTestMgr->incFiltersKoCurrentTest();
                    }
                    /* print filter results */
                    PrintFilterResults(&results[currentOutput], validTimeMs,
                            currentOutput, pGssStruct->getPortName(port),
                            pGssStruct->getPointerToLevels(port),
                            &currentStepHndl->outputs[currentOutput]);
                    /* increase outputs received counter */
                    currentStepHndl->outputsReceived++;
                    if(!currentStepHndl->outputs[currentOutput].optional)
                    {
                        /* optionals don't count for ANY and ALLUNSORTED */
                        currentStepHndl->outputsReceivedAtPort[port]++;
                    }
                }
                else /* if not found packet nor firstSimilarOutput, treat it as not expected */
                {
                    notExpected = true;
                    status = CheckNotExpectedPacket();
                }
                /* delete filter results */
                for(unsigned int out=0; out<currentStepHndl->numberOfOutputs; ++out)
                {
                    if(results[out].defaultResult != NULL)
                    {
                        DeleteFilterResults(&results[out],
                                pGssStruct->getPointerToLevels(port),
                                &currentStepHndl->outputs[out]);
                    }
                }
            }
            else
            {
                snprintf(msg, MAX_MSG_SIZE,
                        "\tNo outputs expected at step %d, port %d\n",
                        currentRxStepNum, port);
                pLogs->saveMsgToLog(port, NULL, msg);
                return -1;
            }
        }
        /* save error from process.Expected or process.NotExpected */
        if(status < 0)
        {
            char title[60] = "";
            if(currentRxStepNum == -1)
                process.printError(status, title, msg, MAX_MSG_SIZE, -1,
                        -1, -1, pGssStruct->getPortName(port), port);
            else if((currentStepHndl != NULL) &&
                    (currentStepHndl->outputsCheckmode == stepStruct::ALL))
                process.printError(status, title, msg, MAX_MSG_SIZE, -1,
                        currentRxStepNum, currentOutput, pGssStruct->getPortName(port),
                        port);
            else
                process.printError(status, title, msg, MAX_MSG_SIZE, -1,
                        currentRxStepNum, tries, pGssStruct->getPortName(port),
                        port);
            pLogs->saveMsgToLog(port, title, msg);
            pLogs->savePacketMsgToLogs(port, "\tData: ", packet, packetLength,
                    phyHeaderOffset);
            return -1;
        }

        /* length received is different than expected (checked) */
        if(((packetLength-phyHeaderOffset) != dataLength) &&
                (pGssStruct->getPortType(port) != SPW_PCI_PORT))
        {
            snprintf(msg, MAX_MSG_SIZE, "\tWarning! Packet length received (%d)"
                    " != expected (%d) at port %s\n",
                    packetLength-(int)phyHeaderOffset, dataLength,
                    pGssStruct->getPortName(port));
            pLogs->saveMsgToLog(port, NULL, msg);
        }
        /* update rx step at this port */
        testFinished = pTestMgr->updateRxStepAtPort(port);
    }
    return status;
}

bool RxPacket::GetStepInfo(unsigned int &step, unsigned int &numberOfOutputs,
        unsigned int &outputsReceived, bool &finished)
{
    bool ret = false;
    finished = testFinished;
    if(currentStepHndl != NULL)
    {
        ret = true;
        step = currentRxStepNum;
        numberOfOutputs = currentStepHndl->numberOfOutputs;
        outputsReceived = currentStepHndl->outputsReceived;
    }
    return ret;
}

void RxPacket::GetNotExpectedInfoMsg(char * auxMsg, unsigned int auxMsg_max_len)
{
    if(notExpectedMsgLen < auxMsg_max_len)
    {
        strncpy(auxMsg, msg, notExpectedMsgLen);
        auxMsg[notExpectedMsgLen] = '\0';
    }
    else
    {
        strncpy(auxMsg, msg, auxMsg_max_len);
        auxMsg[auxMsg_max_len-1] = '\0';
    }
        
}

int RxPacket::CheckNotExpectedPacket()
{
    int status = 0;
    unsigned int msgLen = 0;
	
    status = process.NotExpected(dataPacket, dataLength,
            pGssStruct->getPointerToLevels(port),
            pGssStruct->getNumberOfLevels(port), port);
    
    msgLen = snprintf(msg, MAX_MSG_SIZE, "Packet received but not expected");
    pGssStruct->printPacketProtocolTypeSubtype(dataPacket, dataLength, port,
            msg, MAX_MSG_SIZE, &msgLen);
    msgLen += snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen, " at %s port",
            pGssStruct->getPortName(port));
    notExpectedMsgLen = msgLen;
    msgLen += snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen,
            "\n\tSize: %d\n\tData: ", dataLength);
    pLogs->savePacketMsgToLogs(port, msg, packet, packetLength,
            phyHeaderOffset);
    pGssStruct->incNotExpectedPackets(port);
    pTestMgr->incNotExpectedPacketsCurrentTest();
    return status;
}

void RxPacket::InitFilterResults(filterResults * pResults,
        unsigned int numberOfLevels)
{
    pResults->numbersDef = new uint64_t* [numberOfLevels];
    pResults->stringsDef = new uint8_t ** [numberOfLevels];
    pResults->defaultResult = new short [numberOfLevels];
    pResults->numbersExt = new uint64_t* [numberOfLevels];
    pResults->stringsExt = new uint8_t ** [numberOfLevels];
    pResults->extraResult = new short [numberOfLevels];
    for(unsigned int lvl=0; lvl<numberOfLevels; ++lvl)
    {
        pResults->numbersDef[lvl] = NULL;
        pResults->stringsDef[lvl] = NULL;
        pResults->numbersExt[lvl] = NULL;
        pResults->stringsExt[lvl] = NULL;
    }
}

void RxPacket::PrintFilterResults(filterResults * pResults, double validTimeMs,
        int currentOutput, const char * portName, const level * defaultLevels,
        output * expectedOutputHndl)
{
    pLogs->printFilterMsgHeader(port, currentRxStepNum, currentOutput, portName,
            validTimeMs);
    for(unsigned int lvl=0; lvl<expectedOutputHndl->numberOfLevels; ++lvl)
    {
        for(unsigned int flt=0; flt<defaultLevels[lvl].out.numberOfFilters; ++flt)
        {
            if((defaultLevels[lvl].out.filters[flt].result == 0) &&
                    (pResults->defaultResult[lvl] == 1))
            {
                /* avoid filters KO when result is OK (for minterms) */
                continue;
            }

            if((expectedOutputHndl->level[lvl].defaultFilter) &&
                    (pResults->numbersDef[lvl] != NULL) &&
                    (pResults->stringsDef[lvl] != NULL))
            {
                /* log default filters */
                pLogs->printFilterBody(port, lvl, flt, "default", 
                        &defaultLevels[lvl].out.filters[flt], 
                        defaultLevels[lvl].out.boolVars,
                        pResults->numbersDef[lvl], pResults->stringsDef[lvl]);
            }
        }
        for(unsigned int flt=0; flt<expectedOutputHndl->level[lvl].numberOfFilters; ++flt)
        {
            if((expectedOutputHndl->level[lvl].filters[flt].result == 0) &&
                    (pResults->extraResult[lvl] == 1))
            {
                /* avoid filters KO when result is OK (for minterms) */
                continue;
            }
            if((pResults->numbersExt[lvl] != NULL) && (pResults->stringsExt[lvl] != NULL))
            {
                /* log extra filters */
                pLogs->printFilterBody(port, lvl, flt, "extra",
                        &expectedOutputHndl->level[lvl].filters[flt],
                        expectedOutputHndl->level[lvl].boolVars,
                        pResults->numbersExt[lvl], pResults->stringsExt[lvl]);
            }
        }
    }
    pLogs->printFilterMsgEnd(port, packet, packetLength, phyHeaderOffset);
    expectedOutputHndl->received = 1;
    return;
}

void RxPacket::DeleteFilterResults(filterResults * pResults,
        const level * defaultLevels, output * expectedPacket)
{
    for(unsigned int lvl=0; lvl<expectedPacket->numberOfLevels; ++lvl)
    {
        if(pResults->numbersDef[lvl] != NULL)
            delete[] pResults->numbersDef[lvl];
        if(pResults->stringsDef[lvl] != NULL)
        {
            for(unsigned int bv=0; bv<defaultLevels[lvl].out.numberOfBoolVars; ++bv)
            {
                delete[] pResults->stringsDef[lvl][bv];
            }
            delete[] pResults->stringsDef[lvl];
        }
        if(pResults->numbersExt[lvl] != NULL)
            delete[] pResults->numbersExt[lvl];
        if(pResults->stringsExt[lvl] != NULL)
        {
            for(unsigned int bv=0; bv<expectedPacket->level[lvl].numberOfBoolVars; ++bv)
            {
                delete[] pResults->stringsExt[lvl][bv];
            }
            delete[] pResults->stringsExt[lvl];
        }
    }
    delete[] pResults->numbersDef;
    delete[] pResults->stringsDef;
    delete[] pResults->numbersExt;
    delete[] pResults->stringsExt;
    delete[] pResults->defaultResult;
    delete[] pResults->extraResult;
}

void RxPacket::RxSpecialPacket(special_packet_info * spInfo)
{
    int msgLen = 0;
    spInfo->counter++;

    if(spInfo->print_enabled)
    {
        msgLen = snprintf(msg, MAX_MSG_SIZE,
                "Special packet \"%s\" (%d bytes) received", spInfo->name,
                dataLength);
    }
    if(spInfo->type == special_packet_info::SPECIAL_PERIODIC)
    {
        if(!spInfo->period_enabled)
        {
            spInfo->period_enabled = 1;
            if(spInfo->print_enabled)
            {
                snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen,
                        ". It was the first received");
            }
        }
        else
        {
            spInfo->intervalTimeMs = currentTimeMs-spInfo->lastTimeMs;
            if((currentTimeMs-spInfo->lastTimeMs < spInfo->minValueMs) ||
                    (currentTimeMs-spInfo->lastTimeMs > spInfo->maxValueMs))
            {
                spInfo->outOfMinMax = 1;
                if(spInfo->print_enabled)
                {
                    snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen,
                            ". It was %.3f seconds from last one", 
                            (currentTimeMs-spInfo->lastTimeMs)/1000);
                }
                if(currentTimeMs-spInfo->lastTimeMs < spInfo->minValueMs)
                {
                    spInfo->timesUnderValue++;
                }
                if(currentTimeMs-spInfo->lastTimeMs > spInfo->maxValueMs)
                {
                    spInfo->timesOverValue++;
                }
            }
            else
            {
                spInfo->outOfMinMax = 0;
            }
        }
        spInfo->lastTimeMs = currentTimeMs;
    }
    else if(spInfo->type == special_packet_info::SPECIAL_INTERVAL)
    {
        if(currentTimeMs > (spInfo->lastTimeMs + spInfo->intervalTimeMs))
        {
            //new interval
            if(spInfo->currentIntervalValue < (unsigned int)spInfo->minValueMs)
            {
                spInfo->timesUnderValue++;
            }
            spInfo->currentIntervalValue = 1;
        }
        else
        {
            spInfo->currentIntervalValue++;

        }
        if(spInfo->currentIntervalValue > (unsigned int)spInfo->maxValueMs) 
        {
            spInfo->timesOverValue++;
        }
        spInfo->lastTimeMs = currentTimeMs;
    }
    if(spInfo->print_enabled)
    {
        pLogs->saveMsgToLog(port, NULL, msg);
        pLogs->savePacketMsgToLogs(port, "\tSource Data: ", packet,
                packetLength, phyHeaderOffset);
    }
    return;
}