/**
 * \file	TestManager.c
 * \brief	functions for managing tests (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		27/11/2012 at 15:37:19
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c)2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include "TestManager.h"
#include "../CommonClasses/Logs.h"

TestManager::TestManager(gssStructs * origGssStruct, 
        Logs * origLogs, InitialConfig * origInitialConfig)
{
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pInitialConfig = origInitialConfig;
    
    anyTestLoaded = false;
    
    resetRxPortSteps();
    for(unsigned int idx=0; idx<TEST_ERRORS; ++idx)
    {
        currentTestErrors[idx] = 0;
    }
    allTxStepFinished = false;
    allRxStepFinished = false;
    
    parsing_status = FINISHED;
    session_status = FINISHED;
    periodical_spw_tc_status = FINISHED;
    for(unsigned int port=0; port<MAX_INTERFACES; ++port)
    {
        raw_status[port] = FINISHED;
    }
}

int TestManager::TestInit(const char * stepFilename)
{
    unsigned int step, port;
    stepStruct * steps = pGssStruct->getPointerToSteps();
    int firstStepWithOutputs = -1;
    char testName[MAX_MSG_SIZE];
    
    if(stepFilename[0] == 0)
    {
        snprintf(testName, MAX_MSG_SIZE, "Test %s began",
                pGssStruct->getProcedureName(pGssStruct->getCurrentTest()));
    }
    else
    {
        snprintf(testName, MAX_MSG_SIZE, "Step file %s began", stepFilename);
    }
    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), testName,
            true, false, true);
    
    resetRxPortSteps();
    for(step=0; step<pGssStruct->getNumberOfSteps(); ++step)
    {
        /* get first step with outputs */
        if((firstStepWithOutputs == -1) && (steps[step].numberOfOutputs))
            firstStepWithOutputs = step;

        /* reset outputs received counters */
        steps[step].outputsReceived = 0;
        for(port=0; port<MAX_INTERFACES; ++port)
            steps[step].outputsReceivedAtPort[port] = 0;

        /* mark all outputs as not received */
        /* get accurate rxStep counter */
        for(port=0; port<steps[step].numberOfOutputs; ++port)
        {
            steps[step].outputs[port].received = 0;
            if(rxStep[steps[step].outputs[port].ifRef] == -1)
                rxStep[steps[step].outputs[port].ifRef] = step;
        }
    }
    for(unsigned int idx=0; idx<TEST_ERRORS; ++idx)
    {
        currentTestErrors[idx] = 0;
    }
    allTxStepFinished = false;
    allRxStepFinished = false;
    session_status = IN_PROGRESS;
    
    if(firstStepWithOutputs != -1)
    {
        struct timespec ref;
        clock_gettime(CLOCK_MONOTONIC, &ref);
        rxStepTimerMs[firstStepWithOutputs] =
                (ref.tv_sec * 1000.0) + (ref.tv_nsec / 1000000.0);
    }
    return firstStepWithOutputs;
}

testEndStatus TestManager::TestFinish(char * msg, unsigned int msgSize,
        testEndStatus status, unsigned int step, unsigned int input)
{
    bool errors = false, notDiscardedErrors = false; 
    endStatus = status;
    
    switch(endStatus)
    {
        case TEST_ENDED:
            getTestErrorsMsg(currentTestErrors, msg, msgSize, "Test ended",
                    errors, notDiscardedErrors);
            if(notDiscardedErrors)
            {
                endStatus = TEST_ENDED_WITH_NOT_DISCARDED_ERRORS;
                break;
            }
            else if(errors)
            {
                endStatus = TEST_ENDED_WITH_ERRORS;
            }
            break;
            
        case TEST_ENDED_WITH_TX_ERROR:
            snprintf(msg, msgSize,
                    "Test ended with TX error at step %d, input %d",
                    step, input);
            resetRxPortSteps();
            break;
            
        case TEST_ENDED_ACTION_CHECK_KO:
            snprintf(msg, msgSize,
                    "Test ended with action check KO at step %d, input %d",
                    step, input);
            resetRxPortSteps();
            break;

        case TEST_CANCELED:
            snprintf(msg, msgSize, "Test canceled at step %d, input %d",
                    step, input);
            resetRxPortSteps();
            break;

        default:
            break;
    }
    pLogs->SetTimeInAllLogs(pGssStruct->getNumberOfPorts(), msg, true, true, true);
    session_status = FINISHED;
    return endStatus;
}

bool TestManager::getAnyTestLoaded()
{
    return anyTestLoaded;
}

void TestManager::setAnyTestLoaded(bool newTestLoadedStatus)
{
    anyTestLoaded = newTestLoadedStatus;
}

bool TestManager::getAnythingInProgress(char * msg, unsigned int max_msg_size)
{
    bool isAnythingInProgress = false;
    if(parsing_status == IN_PROGRESS)
    {
        snprintf(msg, max_msg_size,
                "Please stop first any Load process");
        isAnythingInProgress = true;
    }
    else if(session_status == IN_PROGRESS)
    {
        snprintf(msg, max_msg_size,
                "Please stop first any Launch process");
        isAnythingInProgress = true;
    }
    else if(periodical_spw_tc_status == IN_PROGRESS)
    {
        snprintf(msg, max_msg_size,
                "Please stop first any periodical SpW TC process");
        isAnythingInProgress = true;
    }
    else
    {
        unsigned int port = 0;
        for(port=0; port<MAX_INTERFACES; ++port)
        {
            if(raw_status[port] == IN_PROGRESS)
                break;
        }
        if(port != MAX_INTERFACES)
        {
            snprintf(msg, MAX_MSG_SIZE, "Please stop first any Raw Send");
            isAnythingInProgress = true;
        }
    }
    return isAnythingInProgress;
}

bool TestManager::cancelTest(char * msg, unsigned int msgSize)
{
    bool testCanceled = false;
    if(parsing_status == IN_PROGRESS)
    {
        parsing_status = CANCELED;
        snprintf(msg, msgSize, "Test loading canceled");
    }
    else if(session_status == IN_PROGRESS)
    {
        session_status = CANCELED;
        resetRxPortSteps();
        snprintf(msg, msgSize, "Test canceled"); 
        if(allTxStepFinished)
        {
            testCanceled = true;
            TestFinish(msg, MAX_MSG_SIZE, TEST_CANCELED,
                    pGssStruct->getNumberOfSteps()-1,
                    pGssStruct->getPointerToStep(pGssStruct->getNumberOfSteps()-1)->numberOfInputs-1);
        }
    }
    else
    {
        setRawStatus(MAX_INTERFACES, CANCELED);
        snprintf(msg, msgSize, "Send Raw stopped"); 
        testCanceled = true;
    }
    return testCanceled;
}

unsigned int TestManager::getRxStep(unsigned int rxPort)
{
    return rxStep[rxPort];
}

void TestManager::incTxErrorsCurrentTest()
{
    currentTestErrors[TX_ERRORS]++;
}
    
void TestManager::incNotExpectedPacketsCurrentTest()
{
    currentTestErrors[NOT_EXPECTED_PACKETS]++;
}

void TestManager::incFiltersKoCurrentTest()
{
    currentTestErrors[FILTERS_KO]++;
}

double TestManager::checkOutputValidTime(unsigned int rxPort,
        unsigned int stepNumber, double currentTimeMs)
{
    double ret = 0.0;
    double elapsedMs = currentTimeMs - rxStepTimerMs[rxPort];
    if((pGssStruct->getDiscardErrorFlagsIndex(VALID_TIMES_KO) == DISABLED)
            && (elapsedMs > pGssStruct->getStepValidTime(stepNumber)))
    {
        currentTestErrors[VALID_TIMES_KO]++;
        ret = elapsedMs-pGssStruct->getStepValidTime(stepNumber);
    }
    return ret;
}

void TestManager::setAllTxStepFinished()
{
    allTxStepFinished = true;
}

bool TestManager::getAllRxStepFinished()
{
    return allRxStepFinished;
}

bool TestManager::updateRxStepAtPort(unsigned int rxPort)
{
    bool testFinished = false;
    /* check if this port has finished the step */
    stepStruct * rxPortStep = pGssStruct->getPointerToStep(rxStep[rxPort]);
    if((rxStep[rxPort] != -1) &&
            (rxPortStep->outputsReceivedAtPort[rxPort] ==
            rxPortStep->numberOfOutputsAtPort[rxPort]))
    {
        rxStep[rxPort] = rxPortStep->nextStepWithOutputsAtPort[rxPort];
        if(rxStep[rxPort] == -1)
        {
            /* if there are no more rx steps at this port, rxStep finished */
            
            /* check if ALL ports have finished all rx steps */
            unsigned int auxPort = 0;
            for(auxPort=0; auxPort<MAX_INTERFACES; ++auxPort)
            {
                if(rxStep[auxPort] != -1)
                    break;
            }
            if(auxPort == MAX_INTERFACES)
            {
                if(allTxStepFinished)
                {
                    testFinished = true;
                }
                else
                {
                    allRxStepFinished = true;
                }
            }
        }
        else
        {
            /* if there are still rx steps at this port, set new timer */
            /* FIXME: should this be done at tx? */
            struct timespec ref;
            clock_gettime(CLOCK_MONOTONIC, &ref);
            rxStepTimerMs[rxPort] =
                    (ref.tv_sec * 1000.0) + (ref.tv_nsec / 1000000.0);
        }
    }
    return testFinished;
}

void TestManager::resetRxPortSteps()
{
    for(unsigned int rxPort=0; rxPort<MAX_INTERFACES; ++rxPort)
    {
        rxStep[rxPort] = -1;
    }
}

unsigned int TestManager::getTestErrorsMsg(unsigned int errorCntr[TEST_ERRORS],
        char * msg, unsigned int maxMsgSize, const char * title, 
        bool &errors, bool &notDiscardedErrors)
{
    unsigned int msgLen = snprintf(msg, maxMsgSize, "%s", title);
    enabled_status * discardErrorFlags = pGssStruct->getDiscardErrorFlags();
    for(unsigned int idx=0; idx<TEST_ERRORS; ++idx)
    {
        if(errorCntr[idx])
        {
            errors = true;
            if(discardErrorFlags[idx] == DISABLED)
            {
                notDiscardedErrors = true;
                switch(idx)
                {
                    case TX_ERRORS:
                        msgLen += snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen,
                            " with %d sent packet errors", errorCntr[idx]);
                        break;
                    case NOT_EXPECTED_PACKETS:
                        msgLen += snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen,
                            " with %d not expected packets", errorCntr[idx]);
                        break;
                    case FILTERS_KO:
                        msgLen += snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen,
                            " with %d filter errors", errorCntr[idx]);
                        break;
                    case VALID_TIMES_KO:
                        msgLen += snprintf(&msg[msgLen], MAX_MSG_SIZE-msgLen,
                            " with %d time validity errors", errorCntr[idx]);
                        break;

                    default:
                        break;
                }
            }
        }
    }
    return msgLen;
}

void TestManager::joinErrors(unsigned int errorCntr[TEST_ERRORS])
{
    for(unsigned int idx=0; idx<TEST_ERRORS; ++idx)
    {
        errorCntr[idx] += currentTestErrors[idx];
    }
}
processStatus TestManager::getParsingStatus(void)
{
    return parsing_status;
}

void TestManager::setParsingStatus(processStatus status)
{
    parsing_status = status;
}

processStatus * TestManager::getPointerToParsingStatus(void)
{
    return &parsing_status;
}

processStatus TestManager::getSessionStatus(void)
{
    return session_status;
}

void TestManager::setSessionStatus(processStatus status)
{
    session_status = status;
}

processStatus TestManager::getPeriodicalSpWTCStatus(void)
{
    return periodical_spw_tc_status;
}

void TestManager::setPeriodicalSpWTCStatus(processStatus status)
{
    periodical_spw_tc_status = status;
}

processStatus TestManager::getRawStatus(unsigned int port)
{
    return raw_status[port];
}

void TestManager::setRawStatus(unsigned int port, processStatus status)
{
    if(port == MAX_INTERFACES)
    {
        for(unsigned int idxPort=0; idxPort<MAX_INTERFACES; ++idxPort)
        {
            raw_status[idxPort] = status;
            if(status == CANCELED)
            {
                std::lock_guard<std::mutex> lock(raw_sleepMutex[idxPort]);
                raw_sleepWaitCondition[idxPort].notify_all();
            }
        }
    }
    else
    {
        raw_status[port] = status;
        if(status == CANCELED)
        {
            std::lock_guard<std::mutex> lock(raw_sleepMutex[port]);
            raw_sleepWaitCondition[port].notify_all();
        }
    }
}

void TestManager::setRawSleep(unsigned int port, unsigned int msec)
{
    std::unique_lock<std::mutex> rawSleepLock(raw_sleepMutex[port]);
    raw_sleepWaitCondition[port].wait_for(rawSleepLock,
            std::chrono::milliseconds(msec));
}