/**
 * \file	ProcessPacket.h
 * \brief	functions for process (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		18/09/2013 at 11:23:12
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __ProcessPacket_H__
#define __ProcessPacket_H__

#include <stdint.h>             /* uint8_t */
#include "../definitions.h"	/* gss_config, output, level, levelOut */

class ProcessPacket
{
public:
    ProcessPacket();
	
    int Expected(const uint8_t * dataPacket, unsigned int &level0Length, 
            const level * defaultLevels, output * expectedPacket,
            unsigned int &correctLevels, unsigned int rxPort,
            filterResults * pResults);
    int NotExpected(const uint8_t * dataPacket, unsigned int &level0Length,
            const level * defaultLevels, unsigned int numberOfdefaultLevels,
            unsigned int rxPort);
    int checkIfSpecial(const uint8_t * dataPacket, unsigned int &level0Length,
            unsigned int numberOfSpecialPackets, const level * defaultLevels,
            const output * specialPackets, special_packet_info * specialInfo,
            int &specialIndex, unsigned int rxPort);
    void printError(int status, char title[60], char * msg,
            unsigned int maxMsgSize, int specialPacket, int step, int output,
            const char * portName, unsigned int rxPort);

private:
    enum {
        NO_PROCESS_ERROR,
        FORMAT_ERROR,
        IMPORT_ERROR,
        DEFAULT_BV_ERROR,
        EXTRA_BV_ERROR,
        DEFAULT_FILTER_ERROR,
        EXTRA_FILTER_ERROR
    } processErrorType;	
    int errorLevel;
};

#endif /* ndef __ProcessPacket_H__ */