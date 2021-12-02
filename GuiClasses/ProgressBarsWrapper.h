/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ProgressBarsWrapper.h
 * Author: Aaron
 *
 * Created on 7 de junio de 2017, 17:33
 */

#ifndef PROGRESSBARSWRAPPER_H
#define PROGRESSBARSWRAPPER_H
    
enum {
    SHOW = 1,
    HIDE = 0,
};

enum {
    BAR_TX_STEP,
    BAR_INPUT,
    BAR_RX_STEP,
    BAR_OUTPUT
};

#ifdef __cplusplus
extern "C" {
#endif
          
void SetProgressBarMaximum(unsigned int bar, int maxValue);
void SetProgressBarValue(unsigned int bar, int value);
void ShowHideBar(unsigned int bar, unsigned char visible);

#ifdef __cplusplus
}
#endif
#endif /* PROGRESSBARSWRAPPER_H */