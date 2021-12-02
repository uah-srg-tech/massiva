/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CheckSpecialPeriods.cpp
 * Author: Aaron
 * 
 * Created on 22 de septiembre de 2017, 18:46
 */

#include "CheckSpecialPeriods.h"
#include <time.h>				/* clock_gettime */

CheckSpecialPeriods::CheckSpecialPeriods(gssStructs * origGssStruct)
{
    pGssStruct = origGssStruct;
    checkSpecialPeriodsEnabled = true;
}

void CheckSpecialPeriods::SetEnabled(bool state)
{
    checkSpecialPeriodsEnabled = state;
    if(state == false)
        sleepWaitCondition.wakeAll();
}

void CheckSpecialPeriods::CheckSpecialPeriodsSlot()
{
    special_packet_info * specialInfo = pGssStruct->getPointerToSpecialInfos();
    double currentTimeMs = 0.0;
    QMutex mutex;
    struct timespec ref;
    
    do {
        clock_gettime(CLOCK_MONOTONIC, &ref);
        currentTimeMs = (ref.tv_sec * 1000.0) + (ref.tv_nsec / 1000000.0);

        for(unsigned int idx=0; idx<pGssStruct->getNumberOfSpecialPackets(); ++idx)
        {
            if(!checkSpecialPeriodsEnabled)
                break;
            if((specialInfo[idx].type != special_packet_info::SPECIAL_PERIODIC) || 
                    (!specialInfo[idx].period_enabled))
                continue;
            if(!checkSpecialPeriodsEnabled)
                break;
            if((currentTimeMs-specialInfo[idx].lastTimeMs != 0.0) &&
                    (currentTimeMs-specialInfo[idx].lastTimeMs > (2*specialInfo[idx].maxValueMs)))
            {
                emit DisplaySpecialPeriod(idx, (currentTimeMs-specialInfo[idx].lastTimeMs)/1000, Qt::red);
            }
        }
        /* controlled sleep 1 minute */
        mutex.lock();
        sleepWaitCondition.wait(&mutex, 60000);
        mutex.unlock();
    } while(checkSpecialPeriodsEnabled);
    emit finished();
}