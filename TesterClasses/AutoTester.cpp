/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   autoTester.cpp
 * Author: Aaron
 * 
 * Created on 5 de agosto de 2020, 13:18
 */

#include "AutoTester.h"
#include <QMutex>
#include <QWaitCondition>

AutoTester::AutoTester(gssStructs * origGssStruct,
        TestButtons * origTestButtons)
{
    pGssStruct = origGssStruct;
    pTestButtons = origTestButtons;
}

void AutoTester::startAutoTest()
{
    /* sleep for 2 seconds */
    QMutex mutex;
    QWaitCondition waitCondition;
    mutex.lock();
    waitCondition.wait(&mutex, 2000);
    mutex.unlock();
    
    pGssStruct->setAutoTest(true);
    if(pGssStruct->getCurrentTest() == -1)
    {
        pTestButtons->LoadAndLaunchAllTPConfirmed();
    }
    else
    {
        pTestButtons->LoadAndLaunchTPButton();
    }
}