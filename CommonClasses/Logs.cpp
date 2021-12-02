/**
 * \file	Logs.c
 * \brief	(definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		04/09/2013 at 19:12:34
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c)2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef _WIN32
#pragma GCC diagnostic ignored "-Wformat-truncation="
#pragma GCC diagnostic ignored "-Wunused-result"
#endif

#ifndef NO_QT
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>
#endif
#include <dirent.h>
#include <unistd.h>                     /* chdir, mkdir */
#include <errno.h>
#include <cstdio>			/* FILE, fprintf */
#include <cstring>			/* memset */
#include <time.h>			/* asctime, localtime */
#include <sys/stat.h>                   /* mkdir */
#include <sys/time.h>                   /* mkdir */
#include "Logs.h"			/* RAW_TX */
#include "InitialConfig.h"		/* gss_directory */
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define LOG_NAME_DIGITS                         3
#define MAX_LOG_FILE_BYTES			150*1024*1024
#define FIRST_BYTES				8

Logs::Logs(InitialConfig * origInitialConfig)
{
    for(unsigned int idx=0; idx<=RAW_LOG_IDX; ++idx)
    {
        memset(logNames[idx], 0, MAX_LOGFILE_NAME_LEN);
        logFiles[idx] = NULL;
        logBytesPrinted[idx] = 0;
        logFileNumber[idx] = 0;
    }
    oper_string[0] = "==";
    oper_string[1] = "!=";
    oper_string[2] = ">";
    oper_string[3] = "<";
    oper_string[4] = ">=";
    oper_string[5] = "<=";
    
    inv_oper_string[0] = "!=";
    inv_oper_string[1] = "==";
    inv_oper_string[2] = "<=";
    inv_oper_string[3] = ">=";
    inv_oper_string[4] = "<";
    inv_oper_string[5] = ">";

    textResult[0] = "KO";
    textResult[1] = "OK";    
    
    rawTxRxMsg[0] = "Tx";
    rawTxRxMsg[1] = "Rx";
    
    pInitialConfig = origInitialConfig;
    
    for(unsigned int idx=0; idx<3; ++idx)
    {
        phy_header_print[idx] = DISABLED;
    }
    
    logBaseFolderNameLen = 0;
}
    
void Logs::setPrintPhyHeaderOptions(enabled_status * options)
{
    for(unsigned int idx=0; idx<3; ++idx)
    {
        phy_header_print[idx] = options[idx];
    }
}

void Logs::SetTitleInAllLogs(unsigned int numberOfPorts, const char * text,
        const gss_options * pOptions)
{
    time_t timeRef = time(NULL);

    logFileMutex.lock();
    for(unsigned int idx = 0; idx<numberOfPorts; ++idx)
    {
        if(logFiles[idx] != NULL)
        {
            SetTimeInsideMutex(idx, timeRef, text, false, false, ". ");
        }
        if(pOptions->gss_info_print[PORT_LOGS])
        {
            PrintLogInfoInsideMutex(pOptions, idx);
        }
    }
    if(logFiles[MAIN_LOG_IDX] != NULL)
    {
        SetTimeInsideMutex(MAIN_LOG_IDX, timeRef, text, false, false, ". ");
    }
    if(pOptions->gss_info_print[MAIN_LOG])
    {
        PrintLogInfoInsideMutex(pOptions, MAIN_LOG_IDX);
    }
    logFileMutex.unlock();

    rawLogFileMutex.lock();
    if(logFiles[RAW_LOG_IDX] != NULL)
    {
        SetTimeInsideMutex(RAW_LOG_IDX, timeRef, text, false, false, ". ");
    }
    if(pOptions->gss_info_print[RAW_LOG])
    {
        PrintLogInfoInsideMutex(pOptions, RAW_LOG_IDX);
    }
    rawLogFileMutex.unlock();
}

void Logs::SetTimeInLog(unsigned int logIdx, const char * text, bool addNewLine)
{
    time_t timeRef = time(NULL);
    
    if(logIdx == RAW_LOG_IDX)
    {
        rawLogFileMutex.lock();
        SetTimeInsideMutex(RAW_LOG_IDX, timeRef, text, false, false, ". ");
        rawLogFileMutex.unlock();
    }
    else if(logFiles[logIdx] != NULL)
    {
        logFileMutex.lock();
        SetTimeInsideMutex(logIdx, timeRef, text, addNewLine, false, ". ");
        logFileMutex.unlock();
    }
}

void Logs::SetTimeInAllLogs(unsigned int numberOfPorts, const char * text,
        bool addNewLine, bool addDottedLine, bool rawLog)
{
    time_t timeRef = time(NULL);

    logFileMutex.lock();
    for(unsigned int idx = 0; idx<numberOfPorts; ++idx)
    {
        if(logFiles[idx] != NULL)
        {
            SetTimeInsideMutex(idx, timeRef, text, addNewLine, addDottedLine, ". ");
        }
    }
    if(logFiles[MAIN_LOG_IDX] != NULL)
    {
        SetTimeInsideMutex(MAIN_LOG_IDX, timeRef, text, addNewLine, addDottedLine, ". ");
    }
    logFileMutex.unlock();
    if(rawLog)
    {
        rawLogFileMutex.lock();
        if(logFiles[RAW_LOG_IDX] != NULL)
        {
            SetTimeInsideMutex(RAW_LOG_IDX, timeRef, text, false, false, ". ");
        }
        rawLogFileMutex.unlock();
    }
}

void Logs::printRawPacket(logFileSource rawType, const unsigned char * packet,
        int statusLength, unsigned int portPhyHeaderOffset, int port)
{
    char *pBuffer = NULL;
    struct timeval time_now;
    time_t timeRef;
    struct tm * timeinfo;

    if(logFiles[RAW_LOG_IDX] != NULL)
    {
        gettimeofday(&time_now, NULL);
        timeinfo = gmtime(&time_now.tv_sec);
        
        time(&timeRef);
        timeinfo = localtime (&timeRef);
        
        if(statusLength >= 0)
        {
            if(rawType == RAW_TX)
                pBuffer = rawTxMsg[port];
            else if(rawType == RAW_RX)
                pBuffer = rawRxMsg[port];
            GetPacketForPrint(packet, statusLength, pBuffer, MAX_PRINT_LEN*2+1);
        }
        
        rawLogFileMutex.lock();
        logBytesPrinted[RAW_LOG_IDX] += fprintf(logFiles[RAW_LOG_IDX],
                "%d-%.2d-%.2dT%.2d:%.2d:%.2d.%03ld\t%s%d\t",
                1900 + timeinfo->tm_year, timeinfo->tm_mon+1, timeinfo->tm_mday,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
                time_now.tv_usec/1000, rawTxRxMsg[rawType].c_str(), port);
        //ISO 8601
        
        if(statusLength >= 0)
        {
            if((phy_header_print[RAW_LOG] == DISABLED) &&
                    (statusLength >= (int)portPhyHeaderOffset))
            {
                /* 
                 * move print buffer only if requested (disabled header print)
                 * and length is bigger than offset (for printing errors)
                 */
                pBuffer += 2*portPhyHeaderOffset;
            }
            logBytesPrinted[RAW_LOG_IDX] += fprintf(logFiles[RAW_LOG_IDX],
                    "%s\n", pBuffer);
        }
        else
        {
            logBytesPrinted[RAW_LOG_IDX] += fprintf(logFiles[RAW_LOG_IDX],
                    "error %d\n", statusLength);
        }
        
        if(logBytesPrinted[RAW_LOG_IDX] >= MAX_LOG_FILE_BYTES)
        {
            ++logFileNumber[RAW_LOG_IDX];
            logBytesPrinted[RAW_LOG_IDX] = 0;
            snprintf(&logNames[RAW_LOG_IDX][strlen(logNames[RAW_LOG_IDX])-4-LOG_NAME_DIGITS],
                    MAX_LOGFILE_NAME_LEN-strlen(logNames[RAW_LOG_IDX])+4+LOG_NAME_DIGITS,
                    "%.*d.txt", LOG_NAME_DIGITS, logFileNumber[RAW_LOG_IDX]);
            if(chdir(pInitialConfig->GetFile(GSS)) == 0)
            {
                fclose(logFiles[RAW_LOG_IDX]);
                logFiles[RAW_LOG_IDX] = fopen(logNames[RAW_LOG_IDX], "a+");
                chdir(pInitialConfig->GetFile(WORKSPACE));
            }
        }
        rawLogFileMutex.unlock();
        if(pBuffer != NULL)
            memset(pBuffer, 0, MAX_PRINT_LEN);
    }
}

int Logs::openCreateLogs(unsigned int numberOfPorts, const char ** portNames,
            bool createRawLog, const char * logsBaseFolderName)
{
    int status = 0;
    unsigned int idx = 0;
    memset(logFolderName, 0, MAX_LOGFILE_NAME_LEN);
    snprintf(logFolderName, MAX_LOGFILE_NAME_LEN, "%s/%s", logsBaseFolderName,
            pInitialConfig->GetFile(LOG_FOLDER_NAME));
    logBaseFolderNameLen = strlen(logsBaseFolderName);
    
    if(chdir(pInitialConfig->GetFile(GSS)) != 0)
        return -MAIN_LOG_IDX;
    
    /* create logs directory if doesn't exist */
#if defined (_WIN32) || defined (__CYGWIN__)
    status = mkdir(logsBaseFolderName);
#else
    status = mkdir(logsBaseFolderName, 0777);
#endif
    if((status == -1) && (errno != EEXIST))
    {
        status = chdir(pInitialConfig->GetFile(WORKSPACE));
        return -1;
    }

    /* create current date directory if doesn't exist */
    if(chdir(logsBaseFolderName) != 0)
    {
        status = chdir(pInitialConfig->GetFile(WORKSPACE));
        return -1;
    }
#if defined (_WIN32) || defined (__CYGWIN__)
    status = mkdir(pInitialConfig->GetFile(LOG_FOLDER_NAME));
#else
    status = mkdir(pInitialConfig->GetFile(LOG_FOLDER_NAME), 0777);
#endif
    if((status == -1) && (errno != EEXIST))
    {
        status = chdir(pInitialConfig->GetFile(WORKSPACE));
        return -MAIN_LOG_IDX;
    }
    if(chdir("../") != 0)
    {
        status = chdir(pInitialConfig->GetFile(WORKSPACE));
        return -MAIN_LOG_IDX;
    }

    if(OpenCreateLog(MAIN_LOG_IDX, "") != 0)
        return -MAIN_LOG_IDX;
    if((createRawLog) && (OpenCreateLog(RAW_LOG_IDX, "rawLogFile") != 0))
        return -RAW_LOG_IDX;
    for(unsigned int idx=0; idx<numberOfPorts; ++idx)
    {
        if((status = OpenCreateLog(idx, portNames[idx])) != 0)
            break;
    }
    if(status != 0)
        return -idx;
    if(chdir(pInitialConfig->GetFile(WORKSPACE)) != 0)
        return -MAIN_LOG_IDX;
    return 0;
}

void Logs::closeOpenLogs(bool reopen)
{
    for(unsigned int idx=0; idx<(MAX_INTERFACES+2); ++idx)
    {
        CloseOpenLog(idx, reopen);
    }
}

void Logs::ShowLog(unsigned int logIdx)
{
    CloseOpenLog(logIdx, true);
#ifndef NO_QT
    QString configFilenameStr(QString(pInitialConfig->GetFile(GSS)) + "/" + logNames[logIdx]);
    QDesktopServices::openUrl(QUrl::fromLocalFile(configFilenameStr));
#endif
}

void Logs::OpenLogFolder()
{
#ifndef NO_QT
    QProcess *process = new QProcess();
    bool fileExplorerFound = false;
    QString qFileExplorerPath;
    QString qLogFolderPath = pInitialConfig->GetFile(GSS);
    
#if(defined _WIN32)
    qFileExplorerPath = "C:\\Windows\\explorer.exe";
    std::string logFolderNameWin(logFolderName);
    logFolderNameWin[logBaseFolderNameLen] = '\\';
    qLogFolderPath += "\\" + QString::fromStdString(logFolderNameWin);
#else
    qFileExplorerPath = "/usr/bin/nautilus";
    qLogFolderPath += "/" + QString::fromStdString(logFolderName);
#endif
    FILE * fp = NULL;
    if ((fp = fopen(qFileExplorerPath.toLatin1().constData(), "r")) != NULL)
    {
        fclose(fp);
        fileExplorerFound = true;
    }
    if(fileExplorerFound)
        process->start(qFileExplorerPath, QStringList() << qLogFolderPath);
#endif
}

bool Logs::UpdateLogFolder()
{
#ifdef NO_QT
    return true;
#else
    unsigned int strMaxLen = pInitialConfig->strMaxLen;
    char auxCharArray[strMaxLen], folderChar[strMaxLen];
#ifdef _WIN32
    snprintf(folderChar, strMaxLen, "%s\\%s",
            pInitialConfig->GetFile(GSS),
            ((std::string)logFolderName).substr(0, logBaseFolderNameLen).c_str());
#elif(defined __linux__)
    snprintf(folderChar, strMaxLen, "%s/%s",
            pInitialConfig->GetFile(GSS),
            ((std::string)logFolderName).substr(0, logBaseFolderNameLen).c_str());
#endif
    snprintf(auxCharArray, strMaxLen,
            "Choose the MASSIVA logs folder (current: %s)",
            pInitialConfig->GetFile(LOG_FOLDER_NAME));
    QString folderString = QFileDialog::getExistingDirectory (0,
            QObject::tr(auxCharArray), QObject::tr(folderChar));
    if(folderString == NULL)
        return false;
    QByteArray folderArray = folderString.toLatin1();
    memset(auxCharArray, 0, strMaxLen);
    strncpy(auxCharArray, folderArray.constData(), strMaxLen);
    memset(folderChar, 0, strMaxLen);
    strncpy(folderChar, &auxCharArray[strlen(pInitialConfig->GetFile(GSS))+2+logBaseFolderNameLen],
            strlen(auxCharArray)-strlen(pInitialConfig->GetFile(GSS))-2-logBaseFolderNameLen);
        
    return pInitialConfig->SetFile(LOG_FOLDER_NAME, folderChar);
#endif
}

void Logs::saveMsgToLog(unsigned int logIdx, const char * title, const char * msg)
{
    if(logFiles[logIdx] != NULL)
    {
        logFileMutex.lock();
        if(logIdx == MAIN_LOG_IDX) //if requested main logIdx, print in all logs
        {
            for(unsigned int idx=0; idx<=MAIN_LOG_IDX; ++idx)
            {
                if(logFiles[idx] == NULL)
                {
                    continue;
                }
                if(title == NULL)
                {
                    logBytesPrinted[idx] += fprintf(logFiles[idx], "%s\n", msg);
                }
                else
                {
                    logBytesPrinted[idx] += fprintf(logFiles[idx], "%s\t%s\n", title, msg);
                }
            }
        }
        else//if requested any other logIdx, print in requested and in main logs
        {
            if(title == NULL)
            {
                logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], "%s\n", msg);
                logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], "%s\n", msg);
            }
            else
            {
                logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], "%s\t%s\n", title, msg);
                logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], "%s\t%s\n", title, msg);
            }
        }
        logFileMutex.unlock();
    }
}

void Logs::savePacketMsgToLogs(unsigned int logIdx, const char * msg,
        const unsigned char * packet, unsigned int packetLength,
        unsigned int portPhyHeaderOffset, bool date)
{
    logFileMutex.lock();
    PrintPacketMsgInsideMutex(logIdx, msg, packet, packetLength, 
            portPhyHeaderOffset, date);
    logFileMutex.unlock();
}

void Logs::countPrintPacketSummary(portConfig * ports, unsigned int numberOfPorts,
        const special_packet_info * specialInfos, unsigned int specialNumber,
        const unsigned int * notExpected)
{
    logFileMutex.lock();
    for(unsigned int idx=0; idx<specialNumber; ++idx)
    {
        if(specialInfos[idx].counter)
        {
            logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], "\tSpecial packets %s:\t%d\n",
                    specialInfos[idx].name, specialInfos[idx].counter);
        }
        if(specialInfos[idx].type == special_packet_info::SPECIAL_PERIODIC)
        {
            if(specialInfos[idx].timesUnderValue)
            {
                logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX],
                        "\t\tReceived %d packets before min period\n",
                        specialInfos[idx].timesUnderValue);
            }
            if(specialInfos[idx].timesOverValue)
            {
                logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX],
                        "\t\tReceived %d packets after max period\n",
                        specialInfos[idx].timesOverValue);
            }
        }
        else if(specialInfos[idx].type == special_packet_info::SPECIAL_INTERVAL)
        {
            if(specialInfos[idx].timesUnderValue)
            {
                logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], 
                        "\t\tReceived less than %.0f packets in interval %d times\n",
                        specialInfos[idx].minValueMs, specialInfos[idx].timesUnderValue);
            }
            if(specialInfos[idx].timesOverValue)
            {
                logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX],
                        "\t\tReceived less than %.0f packets in interval %d times\n",
                        specialInfos[idx].maxValueMs, specialInfos[idx].timesOverValue);
            }
        }
    }
    for(unsigned int idx=0; idx<numberOfPorts; ++idx)
    {
        if((notExpected[idx]) && (logFiles[idx] != NULL))
        {
            logBytesPrinted[idx] += fprintf(logFiles[idx], 
                    "\tReceived %d unexpected packets at %s port\n",
                    notExpected[idx], ports[idx].name);
            logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], 
                    "\tReceived %d unexpected packets at %s port\n",
                    notExpected[idx], ports[idx].name);
        }
        logBytesPrinted[idx] += fprintf(logFiles[idx], "\n");
    }
    logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], "\n");
    logFileMutex.unlock();
}

void Logs::printFilterMsgHeader(unsigned int logIdx, int step, int output,
        const char * portName, double valid)
{
    char msg[70];
    unsigned int msgLen = 0;
    msg[0] = '\0';
    msgLen = snprintf(msg, 70, "Step %d", step);
    if(output >= 0)
    {
        msgLen += snprintf(&msg[msgLen], 70-msgLen, ", Output %d", output);
    }
    msgLen += snprintf(&msg[msgLen], 70-msgLen, " received at %s port", portName);
    if(valid != 0.0)
    {
        msgLen += snprintf(&msg[msgLen], 70-msgLen, " %.0f ms late", valid);
    }
    msgLen += snprintf(&msg[msgLen], 70-msgLen, ". Filters:\n");
    logFileMutex.lock();
    if(logFiles[logIdx] != NULL)
        logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], "%s", msg);
    if(logFiles[MAIN_LOG_IDX] != NULL)
        logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], "%s", msg);
}

void Logs::printFilterBody(unsigned int logIdx, unsigned int levelNum,
        unsigned int filterNum, const char * filterText, const filter * pFilter,
        const boolVar * pBoolVars, const uint64_t * numbers,
        uint8_t ** strings)
{
    char msg[90];
    msg[0] = '\0';

    snprintf(msg, 60, "\tLevel %d %s filter %d:", levelNum, filterText, filterNum);
    if(logFiles[logIdx] != NULL)
        logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], "%s\n", msg);
    if(logFiles[MAIN_LOG_IDX] != NULL)
        logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], "%s\n", msg);

    for(unsigned int idx=0; idx<pFilter->numberOfBoolVarRefs; ++idx)
    {
        if((pBoolVars[pFilter->boolVarRef[idx]].result == 0) && (pFilter->result == 1))
        {
            /* avoid Logs::boolVars KO when filter is OK (for maxterms) */
            continue;
        }
        printSingleBoolVar(&pBoolVars[pFilter->boolVarRef[idx]],
                numbers[pFilter->boolVarRef[idx]],
                strings[pFilter->boolVarRef[idx]], PRINT_COMPLEX, msg, 90);
        if(logFiles[logIdx] != NULL)
            logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], "\t\t%s\n", msg);
        if(logFiles[MAIN_LOG_IDX] != NULL)
            logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], "\t\t%s\n", msg);
    }
}

void Logs::printFilterMsgEnd(unsigned int logIdx, const unsigned char * packet,
        unsigned int packetLength, unsigned int portPhyHeaderOffset)
{
    PrintPacketMsgInsideMutex(logIdx, "\tData: ", packet, packetLength, 
            portPhyHeaderOffset, true);
    logFileMutex.unlock();
}

void Logs::SetTimeInsideMutex (unsigned int logIdx, time_t timeRef, 
        const char * text, bool addNewLine, bool addDottedLine,
        const char * symbol)
{
    if(logFiles[logIdx] != NULL)
    {
        if(addNewLine)
        {
            if(addDottedLine)
                logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], 
                        "%s%s%s\n---------------------------------------"
                        "-----------------------------------------\n", text, symbol,
                        asctime(localtime(&timeRef)));
            else
                logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], "%s. %s\n", text,
                        asctime(localtime(&timeRef)));
        }
        else
        {
            if(addDottedLine)
                logBytesPrinted[logIdx] += fprintf(logFiles[logIdx],
                        "%s%s%s\n---------------------------------------"
                        "-----------------------------------------", text, symbol,
                        asctime(localtime(&timeRef)));
            else
                logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], "%s%s%s", text, symbol, 
                        asctime(localtime(&timeRef)));
        }

        if(logBytesPrinted[logIdx] >= MAX_LOG_FILE_BYTES)
        {
            ++logFileNumber[logIdx];
            logBytesPrinted[logIdx] = 0;
            snprintf(&logNames[logIdx][strlen(logNames[logIdx])-4-LOG_NAME_DIGITS],
                    MAX_LOGFILE_NAME_LEN-strlen(logNames[logIdx])+4+LOG_NAME_DIGITS,
                    "%.*d.txt", LOG_NAME_DIGITS, logFileNumber[logIdx]);
            fclose(logFiles[logIdx]);
            if(chdir(pInitialConfig->GetFile(GSS)) == 0)
            {
                logFiles[logIdx] = fopen(logNames[logIdx], "a+");
                chdir(pInitialConfig->GetFile(WORKSPACE));
            }
        }
    }
}

void Logs::getFirstBytesOfData(const unsigned char * dataString,
	char * printString, unsigned int size)
{
    unsigned int i=0;
    if(size < 8)
    {
        for(i=0; i<size; ++i)
            snprintf(&printString[2*i], 3, "%02X", dataString[i]);
        printString[2*size] = '\0';
    }
    else
    {
        for(i=0; i<8; ++i)
            snprintf(&printString[2*i], 3, "%02X", dataString[i]);
        snprintf(&printString[2*size], 4, "...");
    }
}

void Logs::printSingleBoolVar (const boolVar * boolVar,
	const uint64_t number, const unsigned char * string,
	printOption bVPrintMode, char * msg, unsigned int maxMsgSize)
{
    char tempString1[20], tempString2[20];
    switch(bVPrintMode)
    {
        case PRINT_COMPLEX:
            switch(boolVar->valueType)
            {
                case boolVar::DEC_BASE:
                    snprintf(msg, maxMsgSize, "%s (= %" PRId64 ") %s %" PRId64 " ? -> %s",
                            boolVar->field, number,
                            oper_string[boolVar->operation].c_str(),
                            boolVar->number, textResult[boolVar->result].c_str());
                    break;

                case boolVar::HEX_BASE: case boolVar::BIN_BASE:
                    if(boolVar->mask == 0xFFFFFFFFFFFFFFFF)
                    {
                        snprintf(msg, maxMsgSize,
                                "%s (= 0x%" PRIX64 ") %s 0x%" PRIX64 "? -> %s",
                                boolVar->field, number,
                                oper_string[boolVar->operation].c_str(),
                                boolVar->number, textResult[boolVar->result].c_str());
                    }
                    else
                    {
                        snprintf(msg, maxMsgSize,
                                "%s (= 0x%" PRIX64 " & 0x%" PRIX64 ") %s 0x%" PRIX64 "? -> %s",
                                boolVar->field, number, boolVar->mask,
                                oper_string[boolVar->operation].c_str(),
                                boolVar->number, textResult[boolVar->result].c_str());
                    }
                    break;

                case boolVar::STRING_CHAR:
                    snprintf(msg, maxMsgSize, "%s (= %c) %s %c? -> %s",
                            boolVar->field, string[0],
                            oper_string[boolVar->operation].c_str(),
                            boolVar->string[0], textResult[boolVar->result].c_str());
                    break;

                case boolVar::STRING:
                    getFirstBytesOfData(string, tempString1, FIRST_BYTES);
                    getFirstBytesOfData(boolVar->string, tempString2, FIRST_BYTES);
                    if(boolVar->number == boolVar->mask)
                    {
                        snprintf(msg, maxMsgSize,
                                "(%" PRId64 " bytes) %s (= %s) %s %s ? -> %s",
                                boolVar->number, boolVar->field, tempString1,
                                oper_string[boolVar->operation].c_str(),
                                tempString2, textResult[boolVar->result].c_str());
                    }
                    else
                    {
                        snprintf(msg, maxMsgSize,
                                "(%" PRId64 " != %" PRId64 " bytes) %s (= %s) %s %s ? -> %s",
                                boolVar->number, boolVar->mask, boolVar->field,
                                tempString1,
                                oper_string[boolVar->operation].c_str(),
                                tempString2, textResult[boolVar->result].c_str());
                    }
                    break;
            }
            break;

        case PRINT_SIMPLE:
            switch(boolVar->valueType)
            {
                case boolVar::DEC_BASE:
                    snprintf(msg, maxMsgSize, "%s --> %" PRId64 "", boolVar->field,
                            number);
                    break;

                case boolVar::HEX_BASE: case boolVar::BIN_BASE:
                    snprintf(msg, maxMsgSize, "%s --> 0x%" PRIX64 "", boolVar->field,
                            number);
                    break;

                case boolVar::STRING_CHAR:
                    snprintf(msg, maxMsgSize, "%s --> %c", boolVar->field,
                            string[0]);
                    break;

                case boolVar::STRING:
                    getFirstBytesOfData(string, tempString1, FIRST_BYTES);
                    getFirstBytesOfData(boolVar->string, tempString2, FIRST_BYTES);
                    if(boolVar->number == boolVar->mask)
                    {
                        snprintf(msg, maxMsgSize, "(%" PRId64 " bytes) %s --> %s",
                                boolVar->number, tempString1, tempString2);
                    }
                    else
                    {
                        snprintf(msg, maxMsgSize, "(%" PRId64 " != %" PRId64 " bytes) %s --> %s",
                                boolVar->number, boolVar->mask, tempString1, tempString2);
                    }
                    break;
            }
            break;

        default:
            switch (boolVar->valueType)
            {	 
                case boolVar::DEC_BASE:
                    if(boolVar->result)
                    {
                        snprintf(msg, maxMsgSize, "var %d (%s) %" PRId64 " %s %" PRId64 "",
                                bVPrintMode, boolVar->field, number,
                                oper_string[boolVar->operation].c_str(), boolVar->number);
                    }
                    else
                    {
                        snprintf(msg, maxMsgSize, "var %d (%s) %" PRId64 " %s %" PRId64 "",
                                bVPrintMode, boolVar->field, number,
                                inv_oper_string[boolVar->operation].c_str(), boolVar->number);
                    }
                    break;

                case boolVar::HEX_BASE: case boolVar::BIN_BASE:
                    if(boolVar->result)
                    {
                        if(boolVar->mask == 0xFFFFFFFFFFFFFFFF)
                        {
                            snprintf(msg, maxMsgSize,
                                    "var %d (%s) 0x%" PRIX64 " %s 0x%" PRIX64 "", bVPrintMode,
                                    boolVar->field, number,
                                    oper_string[boolVar->operation].c_str(), boolVar->number);
                        }
                        else
                        {
                            snprintf(msg, maxMsgSize,
                                    "var %d (%s) (0x%" PRIX64 " & 0x%" PRIX64 ") %s 0x%" PRIX64 "",
                                    bVPrintMode, boolVar->field, number,
                                    boolVar->mask, oper_string[boolVar->operation].c_str(),
                                    boolVar->number);
                        }
                    }
                    else
                    {
                        if(boolVar->mask == 0xFFFFFFFFFFFFFFFF)
                        {
                            snprintf(msg, maxMsgSize,
                                    "var %d (%s) 0x%" PRIX64 " %s 0x%" PRIX64 "",
                                    bVPrintMode, boolVar->field, number,
                                    inv_oper_string[boolVar->operation].c_str(),
                                    boolVar->number);
                        }
                        else
                        {
                            snprintf(msg, maxMsgSize,
                                    "var %d (%s) (0x%" PRIX64 " & 0x%" PRIX64 ") %s 0x%" PRIX64 "",
                                    bVPrintMode, boolVar->field, number,
                                    boolVar->mask, inv_oper_string[boolVar->operation].c_str(),
                                    boolVar->number);
                        }
                    }
                    break;

                case boolVar::STRING_CHAR:
                    if(boolVar->result)
                    {
                        snprintf(msg, maxMsgSize, "var %d (%s) %c %s %c",
                                bVPrintMode, boolVar->field, string[0],
                                oper_string[boolVar->operation].c_str(), boolVar->string[0]);
                    }
                    else
                    {
                        snprintf(msg, maxMsgSize, "var %d (%s) %c %s %c",
                                bVPrintMode, boolVar->field, string[0],
                                inv_oper_string[boolVar->operation].c_str(), boolVar->string[0]);
                    }
                    break;

                case boolVar::STRING:
                    getFirstBytesOfData(string, tempString1, FIRST_BYTES);
                    getFirstBytesOfData(boolVar->string, tempString2, FIRST_BYTES);
                    if(boolVar->result)
                    {
                        if(boolVar->number == boolVar->mask)
                        {
                            snprintf(msg, maxMsgSize, "(%" PRId64 " bytes) var %d (%s) %s %s %s",
                                    boolVar->number, bVPrintMode, boolVar->field,
                                    tempString1, oper_string[boolVar->operation].c_str(),
                                    tempString2);
                        }		 
                        else
                        {
                            snprintf(msg, maxMsgSize, "(%" PRId64 " != %" PRId64 " bytes) var %d (%s) %s %s %s",
                                    boolVar->number, boolVar->mask, bVPrintMode, boolVar->field,
                                    tempString1, oper_string[boolVar->operation].c_str(),
                                    tempString2);
                        }
                    }
                    else
                    {
                        if(boolVar->number == boolVar->mask)
                        {
                            snprintf(msg, maxMsgSize, "(%" PRId64 " bytes) var %d (%s) %s %s %s",
                                    boolVar->number, bVPrintMode, boolVar->field,
                                    tempString2, inv_oper_string[boolVar->operation].c_str(),
                                    tempString1);
                        }
                        else
                        {
                            snprintf(msg, maxMsgSize, "(%" PRId64 " != %" PRId64 " bytes) var %d (%s) %s %s %s",
                                    boolVar->number, boolVar->mask, bVPrintMode, boolVar->field,
                                    tempString2, inv_oper_string[boolVar->operation].c_str(),
                                    tempString1);
                        }
                    }
                    break;
            }
            break;
    }
    if(boolVar->type == boolVar::FROM_GROUP_ARRAY)
    {
        snprintf(&msg[strlen(msg)], maxMsgSize-strlen(msg), " [%d]",
                boolVar->indexGroup);
    }
}

void Logs::PrintLogInfoInsideMutex(const gss_options * pOptions, unsigned int logIdx)
{
    logBytesPrinted[logIdx] += fprintf(logFiles[logIdx],
            "Test campaign %s version %s. %s",
            pOptions->test_campaign, pOptions->version, pOptions->date);
    if(pOptions->version_control_url[0] != 0)
    {
        logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], " (%s)",
                pOptions->version_control_url);
    }
    logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], "\n");
}

void Logs::PrintPacketMsgInsideMutex(unsigned int logIdx, const char * msg,
        const unsigned char * packet, unsigned int packetLength,
        unsigned int portPhyHeaderOffset, bool date)
{
    time_t timeRef = time(NULL);
    
    char printBuffer[MAX_PRINT_LEN*2+1];
    GetPacketForPrint(packet, packetLength, printBuffer, MAX_PRINT_LEN*2+1);
    
    char * pBufferMain = printBuffer;
    char * pBufferPorts = printBuffer;
    /* 
     * move print buffer only if requested (disabled header print)
     * and length is bigger than offset (for printing errors)
     */
    if((phy_header_print[MAIN_LOG] == DISABLED) && (packetLength >= portPhyHeaderOffset))
        pBufferMain = &printBuffer[2*portPhyHeaderOffset];
    if((phy_header_print[PORT_LOGS] == DISABLED) && (packetLength >= portPhyHeaderOffset))
        pBufferPorts = &printBuffer[2*portPhyHeaderOffset];
    
    if(logFiles[MAIN_LOG_IDX] != NULL)
    {
        logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], "%s%s",
                msg, pBufferMain);
        if(date)
            SetTimeInsideMutex(MAIN_LOG_IDX, timeRef, "\n", false, false, "\t");
        logBytesPrinted[MAIN_LOG_IDX] += fprintf(logFiles[MAIN_LOG_IDX], "\n");
    }
    if(logFiles[logIdx] != NULL)
    {
        logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], "%s%s", msg, pBufferPorts);
        if(date)
            SetTimeInsideMutex(logIdx, timeRef, "\n", false, false, "\t");
        logBytesPrinted[logIdx] += fprintf(logFiles[logIdx], "\n");
    }
}

void Logs::GetPacketForPrint(const unsigned char * packet,
        unsigned int packetLen, char * pBuffer, unsigned int pBufferLen)
{
    unsigned int idx = 0;
    if(packetLen > ((pBufferLen-1)/2))
    {
        char finalText[15];
        unsigned int finalTextOffset = pBufferLen -
                snprintf(finalText, 15, "... (%d)", packetLen)-1;//including '\0'
        for(idx=0; (2*idx) < finalTextOffset; ++idx)
            snprintf(&pBuffer[2*idx], 3, "%02X", packet[idx]);
        snprintf(&pBuffer[finalTextOffset], pBufferLen-finalTextOffset, "%s", finalText);
    }
    else
    {
        for(idx=0; idx<packetLen; ++idx)
            snprintf(&pBuffer[2*idx], 3, "%02X", packet[idx]);
    }
}

int Logs::OpenCreateLog(unsigned int logIdx, const char *portName)
{
    logFileNumber[logIdx] = 1;
    DIR *dirp;
    struct dirent *dp;
    bool firstFound = false;

    /* look for last log file number */
    dirp = opendir(logFolderName);
    do {
        if((dp = readdir(dirp)) != NULL)
        {
            /* checks:
             * 1- file base name is same as portName
             * 2- file base name len is same as portNameLen (SpW_TC_XXX.txt)
             * 3- file name has "_" between base and XXX
             * 4- file name ends in ".txt"
             **/
            unsigned int d_nameLen = strlen(dp->d_name);
            unsigned int portNameLen = strlen(portName);
            if((strncmp(dp->d_name, portName, portNameLen) == 0) &&
                    ((d_nameLen-5-LOG_NAME_DIGITS) == portNameLen) &&
                    (dp->d_name[d_nameLen-5-LOG_NAME_DIGITS] == '_') &&
                    (strncmp(&dp->d_name[d_nameLen-4], ".txt", 4)) == 0)
            {
                if(!firstFound)
                    firstFound = true;
                logFileNumber[logIdx] = 
                        strtoul(&dp->d_name[strlen(dp->d_name)-4-LOG_NAME_DIGITS], NULL, 10);
            }
            else if(firstFound)
            {
                break;
            }
        }
    } while (dp != NULL);
    closedir(dirp);

    /* set definitive filename */
    snprintf(logNames[logIdx], MAX_LOGFILE_NAME_LEN, "%s/%s_%.*u.txt",
            logFolderName, portName, LOG_NAME_DIGITS, logFileNumber[logIdx]);

    /* open log file */
    logFiles[logIdx] = fopen(logNames[logIdx], "a+");
    if(logFiles[logIdx] == NULL)
        return -1;
    
    /* get number of bytes printed */
    logBytesPrinted[logIdx] = ftell(logFiles[logIdx]);
    return 0;
}

void Logs::CloseOpenLog(unsigned int logIdx, bool reopen)
{
    if(logFiles[logIdx] != NULL)
    {
        if(logIdx == RAW_LOG_IDX)
            rawLogFileMutex.lock();
        else
            logFileMutex.lock();
        fclose(logFiles[logIdx]);
        if(reopen)
        {
            if(chdir(pInitialConfig->GetFile(GSS)) == 0)
            {
                logFiles[logIdx] = fopen(logNames[logIdx], "a+");
                chdir(pInitialConfig->GetFile(WORKSPACE));
            }
        }
        else
        {
            logFiles[logIdx] = NULL;
        }
        if(logIdx == RAW_LOG_IDX)
            rawLogFileMutex.unlock();
        else
            logFileMutex.unlock();
    }
}