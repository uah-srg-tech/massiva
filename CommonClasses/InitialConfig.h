/**
 * \file	InitialConfig.h
 * \brief	functions for selecting initial worskpace folder (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		29/10/2014 at 17:01:03
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef INITIALCONFIG_H
#define INITIALCONFIG_H

enum dirType {
    INI_CONFIG_FILE,
    XML_CONFIG_FILE,
    GSS,
    WORKSPACE,
    GSS_LAST,
    WORKSPACE_LAST,
    MANUAL,
    LOG_FOLDER_NAME
};

class InitialConfig
{
public:
    static const unsigned int strMaxLen = 400;
    
    InitialConfig();
    int CheckGssFolderWritable(char * auxMsg, unsigned int auxMsgLen);
    void SetCloseGss(bool value);
    bool GetCloseGss();
    const char * GetFile(enum dirType type);
    bool SetFile(enum dirType type, const char * newFile);
    void SetFileLast(char * pFile, const char * newFile);
    bool SetFileLog(const char * newFile);
    int FindIni(char * auxMsg, unsigned int auxMsgLen);
    int ConfigWorkspaceParseIni(char * auxMsg, unsigned int auxMsgLen);
    int ConfigWorkspaceCommandOption(char * auxMsg, unsigned int auxMsgLen);
    void ShowConfigFile(bool isIniFile);
    int UpdateConfigGSS(char * newConfigPath, unsigned int newConfigPathLen);
    int RestoreConfigGSS();
    void SetLastWorkingInfo();
    bool GetLastWorkingValid();
    
private:
    int SanitizeWorkspaceFile(char * auxMsg, unsigned int auxMsgLen);
    
#ifdef _WIN32
    static const char slashChar = '\\';
#else
    static const char slashChar = '/';
#endif
        
    bool closeGss;
    bool last_working_valid;
    char gssIniConfigFile[strMaxLen];
    char gssConfigFile[strMaxLen];
    char currentLogFolderName[strMaxLen];

    char exe_path[strMaxLen];
    char home_path[strMaxLen];

    char gss_dir[strMaxLen];
    char workspace_dir[strMaxLen];
    char workspace_dir_full[strMaxLen];
    char gss_dir_last[strMaxLen];
    char workspace_dir_last[strMaxLen];
    char manual_path[strMaxLen];
    
    char gssConfigFile_last_working[strMaxLen];
    char workspace_dir_last_working[strMaxLen];
    
    char configXMLFileCL[strMaxLen];
};

#endif /* INITIALCONFIG_H */
