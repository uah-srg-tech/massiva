/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   autoTester.h
 * Author: Aaron
 *
 * Created on 5 de agosto de 2020, 13:18
 */

#ifndef AUTOTESTER_H
#define AUTOTESTER_H

#include "../GuiClasses/TestButtons.h"

class AutoTester : public QObject
{
Q_OBJECT

public:
    AutoTester(gssStructs * origGssStruct, TestButtons * origTestButtons);
    
public slots:
    void startAutoTest();

private:
    gssStructs * pGssStruct;
    TestButtons * pTestButtons;
};

#endif /* AUTOTESTER_H */

