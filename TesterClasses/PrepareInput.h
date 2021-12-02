/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PrepareInput.h
 * Author: Aaron
 *
 * Created on 27 de septiembre de 2017, 16:02
 */

#ifndef PREPAREINPUT_H
#define PREPAREINPUT_H

#include "../CommonClasses/gssStructs.h"

class PrepareInput
{
public:
    PrepareInput(gssStructs * origGssStruct);
    int prepare(unsigned char tcBuffer[MAX_LEVELS][MAX_PACKET_SIZE+MAX_PHY_HDR],
            unsigned int portPhyHeaderOffsetTC, input * currentInput);
    void printError(int status, char title[60], char * msg,
            unsigned int maxMsgSize, const char * portName, unsigned int port,
            int txStep, unsigned int input);
    
private:
    enum {
	PREPARE_NO_ERROR,
	PREPARE_EXPORT_ERROR,
	PREPARE_FORMAT_ERROR,
	PREPARE_DICS_ERROR
    } prepareErrorType;
    
    gssStructs * pGssStruct;
    unsigned int curLevel;
    
    void CreatePortPhyHeaderOffset(unsigned char * packet, int length,
            portConfig * pPort);
};
#endif /* PrepareInput */
