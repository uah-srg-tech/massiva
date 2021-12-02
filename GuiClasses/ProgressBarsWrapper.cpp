/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../definitions.h"
#include "../Forms/mainForm.h"
#include "ProgressBarsWrapper.h"

static mainForm * getMainGui(void)
{
    mainForm * pMainGui = NULL;
    foreach(QWidget *widget, QApplication::topLevelWidgets())
    {
        if(widget->objectName() == "mainForm")
        {
            pMainGui = (mainForm *)widget;
            break;
        }
    }
    return pMainGui;
}

void SetProgressBarMaximum(unsigned int bar, int maxValue)
{
    mainForm * pMainGui = getMainGui();
    switch(bar)
    {
        case BAR_TX_STEP:
            pMainGui->setMaximumTxStep(maxValue);
            break;
        case BAR_INPUT:
            pMainGui->setMaximumInput(maxValue);
            break;
        case BAR_RX_STEP:
            pMainGui->setMaximumRxStep(maxValue);
            break;
        case BAR_OUTPUT:
            pMainGui->setMaximumOutput(maxValue);
            break;
    }
    return;
}

void SetProgressBarValue(unsigned int bar, int value)
{
    mainForm * pMainGui = getMainGui();
    switch(bar)
    {
        case BAR_TX_STEP:
            pMainGui->setValueTxStep(value);
            break;
        case BAR_INPUT:
            pMainGui->setValueInput(value);
            break;
        case BAR_RX_STEP:
            pMainGui->setValueRxStep(value);
            break;
        case BAR_OUTPUT:
            pMainGui->setValueOutput(value);
            break;
    }
    return;
}

void ShowHideBar(unsigned int bar, unsigned char visible)
{
    mainForm * pMainGui = getMainGui();
    switch(bar)
    {
        case BAR_TX_STEP:
            pMainGui->setVisibleTxStep((bool)visible);
            break;
        case BAR_INPUT:
            pMainGui->setVisibleInput((bool)visible);
            break;
        case BAR_RX_STEP:
            pMainGui->setVisibleRxStep((bool)visible);
            break;
        case BAR_OUTPUT:
            pMainGui->setVisibleOutput((bool)visible);
            break;
    }
    return;
}