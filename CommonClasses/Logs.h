/**
 * \file	Logs.h
 * \brief	(declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		04/09/2013 at 19:12:34
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c)2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef LOGS_H
#define LOGS_H

#include <mutex>
#include <string>
#include "../definitions.h"		/* special_packet_info, portConfig */
#include "InitialConfig.h"

#define MAIN_LOG_IDX                    MAX_INTERFACES
#define RAW_LOG_IDX                     MAX_INTERFACES+1

class Logs
{
public:
    enum printOption{
        PRINT_COMPLEX,
        PRINT_SIMPLE,
    };

    enum logFileSource{
        RAW_TX,
        RAW_RX,
        RAW_TX_ERROR,
        RAW_RX_ERROR,
    };

    Logs(InitialConfig * origInitialConfig);
    
    void setPrintPhyHeaderOptions(enabled_status * options);
    void SetTitleInAllLogs(unsigned int numberOfPorts, const char * text,
            const gss_options * pOptions);
    
    void SetTimeInLog(unsigned int logIdx, const char * text, bool addNewLine);
    void SetTimeInAllLogs(unsigned int numberOfPorts, const char * text,
            bool addNewLine, bool addDottedLine, bool rawLog);

    void printRawPacket(logFileSource rawType, const unsigned char * packet,
            int statusLength, unsigned int portPhyHeaderOffset, int port);

    int openCreateLogs(unsigned int numberOfPorts, const char ** portNames,
            bool createRawLog = true, const char * logsBaseFolderName = "logs");
    void closeOpenLogs(bool reopen);

    void ShowLog(unsigned int logIdx);
    void OpenLogFolder();
    bool UpdateLogFolder();

    void saveMsgToLog(unsigned int logIdx, const char * title, const char * msg);
    void savePacketMsgToLogs(unsigned int logIdx, const char * msg,
            const unsigned char * packet, unsigned int packetLength,
            unsigned int portPhyHeaderOffset, bool date = true);

    void countPrintPacketSummary(portConfig * ports, unsigned int numberOfPorts,
            const special_packet_info * specialInfos,
            unsigned int specialNumber, const unsigned int * notExpected);

    void printFilterMsgHeader(unsigned int logIdx, int step, int output,
            const char * portName, double valid);
    void printFilterBody(unsigned int logIdx, unsigned int levelNum,
            unsigned int filterNum, const char * filterText,
            const filter * filters, const boolVar * boolVars,
            const uint64_t * numbers, uint8_t ** strings);
    void printFilterMsgEnd(unsigned int logIdx, const unsigned char * packet,
            unsigned int packetLength, unsigned int portPhyHeaderOffset);
    
private:
    static const unsigned int MAX_PRINT_LEN = 10000;
    static const unsigned int MAX_LOGFILE_NAME_LEN = 100;
    int logBaseFolderNameLen;
    char logFolderName[MAX_LOGFILE_NAME_LEN];
    char logNames[MAX_INTERFACES+2][MAX_LOGFILE_NAME_LEN];
    FILE * logFiles[MAX_INTERFACES+2];
    char rawRxMsg[MAX_INTERFACES][MAX_PRINT_LEN*2+1];
    char rawTxMsg[MAX_INTERFACES][MAX_PRINT_LEN*2+1];
    unsigned long logBytesPrinted[MAX_INTERFACES+2];
    unsigned int logFileNumber[MAX_INTERFACES+2];
    std::mutex logFileMutex, rawLogFileMutex;
    std::string rawTxRxMsg[2], textResult[2];
    std::string oper_string[6], inv_oper_string[6];
    InitialConfig * pInitialConfig;
    enabled_status phy_header_print[3];
    
    void SetTimeInsideMutex(unsigned int logIdx, time_t timeRef, 
        const char * text, bool addNewLine, bool addDottedLine,
        const char * symbol);
    void getFirstBytesOfData(const unsigned char * dataString,
            char * printString, unsigned int size);
    void printSingleBoolVar (const boolVar * boolVar,
            const uint64_t number, const unsigned char * string,
            printOption bVPrintMode, char * msg, unsigned int maxMsgSize);
    void PrintLogInfoInsideMutex(const gss_options * pOptions, unsigned int logIdx);
    void PrintPacketMsgInsideMutex(unsigned int logIdx, const char * msg,
            const unsigned char * packet, unsigned int packetLength,
            unsigned int portPhyHeaderOffset, bool date);
    void GetPacketForPrint(const unsigned char * packet,
            unsigned int packetLen, char * pBuffer, unsigned int pBufferLen);
    int OpenCreateLog(unsigned int logIdx, const char *portName);
    void CloseOpenLog(unsigned int logIdx, bool reopen);
};
#endif /* LOGS_H */