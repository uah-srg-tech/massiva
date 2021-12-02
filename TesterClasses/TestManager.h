/**
 * \file	TestManager.h
 * \brief	functions for managing tests (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		08/01/2014 at 15:30:23
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef TESTMANAGER_H
#define TESTMANAGER_H

#include "../CommonClasses/gssStructs.h"
#include "../CommonClasses/Logs.h"
#include <mutex>
#include <condition_variable>

typedef enum {
    TEST_ENDED,
    TEST_ENDED_WITH_ERRORS,
    TEST_ENDED_WITH_NOT_DISCARDED_ERRORS,
    TEST_CANCELED,
    TEST_ENDED_WITH_TX_ERROR,
    TEST_ENDED_ACTION_CHECK_KO,
} testEndStatus;

class TestManager
{
public:
    TestManager(gssStructs * origGssStruct, Logs * origLogs,
            InitialConfig * origInitialConfig);
    int TestInit(const char * stepFilename);
    testEndStatus TestFinish(char * msg, unsigned int msgSize,
            testEndStatus status, unsigned int step, unsigned int input);
    
    bool getAnyTestLoaded();
    void setAnyTestLoaded(bool newTestLoadedStatus);
    bool getAnythingInProgress(char * msg, unsigned int max_msg_size);
    bool cancelTest(char * msg, unsigned int msgSize);
    
    unsigned int getRxStep(unsigned int rxPort);
    
    void incTxErrorsCurrentTest();
    void incNotExpectedPacketsCurrentTest();
    void incFiltersKoCurrentTest();
    
    double checkOutputValidTime(unsigned int rxPort, unsigned int stepNumber,
            double currentTimeMs);
    
    void setAllTxStepFinished();
    bool getAllRxStepFinished();
    
    bool updateRxStepAtPort(unsigned int rxPort);
    
    unsigned int getTestErrorsMsg(unsigned int errorCntr[TEST_ERRORS],
            char * msg, unsigned int maxMsgSize, const char * title, 
            bool &errors, bool &notDiscardedErrors);
    void joinErrors(unsigned int errorCntr[TEST_ERRORS]);

    processStatus getParsingStatus(void);
    void setParsingStatus(processStatus status);
    processStatus * getPointerToParsingStatus(void);

    processStatus getSessionStatus(void);
    void setSessionStatus(processStatus status);

    processStatus getPeriodicalSpWTCStatus(void);
    void setPeriodicalSpWTCStatus(processStatus status);

    processStatus getRawStatus(unsigned int port);
    void setRawStatus(unsigned int port, processStatus status);
    void setRawSleep(unsigned int port, unsigned int msec);

private:
    gssStructs * pGssStruct;
    Logs * pLogs;
    InitialConfig * pInitialConfig;
    
    bool anyTestLoaded;
    
    int rxStep[MAX_INTERFACES];
    double rxStepTimerMs[MAX_INTERFACES];
    testEndStatus endStatus;
    unsigned int currentTestErrors[TEST_ERRORS];
    bool allTxStepFinished;
    bool allRxStepFinished;
    
    processStatus parsing_status;
    processStatus session_status;
    processStatus periodical_spw_tc_status;
    
    processStatus raw_status[MAX_INTERFACES];
    std::mutex raw_sleepMutex[MAX_INTERFACES];
    std::condition_variable raw_sleepWaitCondition[MAX_INTERFACES];
    
    void resetRxPortSteps();
};

#endif /* TESTMANAGER_H */