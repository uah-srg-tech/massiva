/**
 * \file	InitialConfig.c
 * \brief	functions for selecting initial worskpace folder (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		29/10/2014 at 17:01:03
 * Company:		Space Research Group, Universidad de Alcalá
 * Copyright:	Copyright (c) 2014, Aaron Montalvo
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
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#endif
#include <cstdio>			/* fopen */
#include <cstring>			/* memset, strncpy */
#include <unistd.h>			/* getcwd, chdir */
#include <fcntl.h>			/* O_RDWR  */
#include <sys/stat.h>		/* S_IREAD, S_IWRITE */
#include <stdint.h>			/* uint32_t */
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include "InitialConfig.h"

#define DEFAULT_GSS_CONFIG_INI		"gss_config.ini"
#define HOME_GSS_FOLDER 		"massiva"

InitialConfig::InitialConfig()
{
    closeGss = false;
    
    last_working_valid = false;
    memset(gssConfigFile_last_working, 0, strMaxLen);
    memset(workspace_dir_last_working, 0, strMaxLen);
    memset(configXMLFileCL, 0, strMaxLen);
    
    snprintf(gssIniConfigFile, strMaxLen, "%s", DEFAULT_GSS_CONFIG_INI);
    
#ifndef NO_QT
    QByteArray appDirPath = QCoreApplication::applicationDirPath().toLatin1();
    snprintf(manual_path, strMaxLen, "%s/MASSIVA-User-Manual.pdf",
            appDirPath.constData());
#endif
}

void InitialConfig::SetCloseGss(bool value)
{
    closeGss = value;
}

bool InitialConfig::GetCloseGss()
{
    return closeGss;
}

const char * InitialConfig::GetFile(enum dirType type)
{
    char * ret;
    switch(type)
    {
        case INI_CONFIG_FILE:
            ret = gssIniConfigFile;
            break;
            
        case XML_CONFIG_FILE:
            ret = gssConfigFile;
            break;
            
        case GSS:
            ret = gss_dir;
            break;
            
        case WORKSPACE:
            ret = workspace_dir_full;
            break;
            
        case GSS_LAST:
            ret = gss_dir_last;
            break;
            
        case WORKSPACE_LAST:
            ret = workspace_dir_last;
            break;
            
        case MANUAL:
            ret = manual_path;
            break;
            
        case LOG_FOLDER_NAME: default:
            ret = currentLogFolderName;
            break;
    }
    return ret;
}

bool InitialConfig::SetFile(enum dirType type, const char * newFile)
{
    bool ret = true;
    switch(type)
    {
        case GSS_LAST: case WORKSPACE_LAST:
            if(type == GSS_LAST)
                SetFileLast(gss_dir_last, newFile);
            else
                SetFileLast(workspace_dir_last, newFile);
            break;
            
        case LOG_FOLDER_NAME:
            ret = SetFileLog(newFile);
            break;
            
        default:
            break;
    }
    return ret;
}

void InitialConfig::SetFileLast(char * pFile, const char * newFile)
{
    char * fileCharInit = NULL;
    /* first all dir+file names are copied  */
    strncpy(pFile, newFile, strMaxLen);
    /* add \0 if newFile is longer than strMaxLen */
    if(strlen(newFile) > strMaxLen)
        pFile[strMaxLen-1] = '\0';

    /* then filename must be removed */
    fileCharInit = strrchr(pFile, '/');
    memset(fileCharInit, 0, strMaxLen-(uint32_t)(fileCharInit-pFile));
}

bool InitialConfig::SetFileLog(const char * newFile)
{
    memset(currentLogFolderName, 0, strMaxLen);
    snprintf(currentLogFolderName, strMaxLen, "%s", newFile);

    /* check if folderName is the same as currentDate */
    bool selectedCurrentDate = false;
    time_t timeRef = time(NULL);
    char timeArray[strMaxLen];
    strftime(timeArray, strMaxLen, "%Y-%m-%d", localtime(&timeRef));
    if(strncmp(newFile, timeArray, strlen(newFile)) == 0)
        selectedCurrentDate = true;

    /* go to GSS dir and open gss_config.ini */
    if(chdir(gss_dir) != 0)
    {
#ifdef NO_QT
        printf("MASSIVA folder not found");
#else
        QMessageBox::information(0, "Error",
                QObject::tr("MASSIVA folder not found"));
#endif
        return false;
    }
    FILE * fp = fopen(gssIniConfigFile, "rb");//binary mode for CR+LF
    if(fp == NULL)
    {
#ifdef NO_QT
        printf("INI file not editable");
#else
        QMessageBox::information(0, "Error",
                QObject::tr("INI file not editable"));
#endif
        return false;
    }

    /* write current log folder in gss_config.ini if necessary */
    unsigned int numberOfLines = 0;
    int lastWorkspacePosition = 0;
    while (!feof(fp))
    {
        char auxLine[strMaxLen];
        if(fgets(auxLine, strMaxLen, fp) == NULL)
        {
#ifdef NO_QT
            printf("INI file parsing error");
#else
            QMessageBox::information(0, "Error",
                    QObject::tr("INI file parsing error"));
#endif
            return false;
        }
        numberOfLines++;
        if(numberOfLines == 2)
        {
            if(auxLine[strlen(auxLine)-2] == '\r')
                lastWorkspacePosition = ftell(fp) - 2; //remove \r\n
            else
                lastWorkspacePosition = ftell(fp) - 1; //remove \n
        }
    }
    fclose(fp);
    if(numberOfLines == 3)
    {
        /* remove last line from ini file */
        int fd = open(gssIniConfigFile, O_RDWR, S_IREAD | S_IWRITE );
        ftruncate(fd, lastWorkspacePosition);
        close(fd);
    }
    else if(numberOfLines != 2)
    {
#ifdef NO_QT
        printf("INI file wrong format");
#else
        QMessageBox::information(0, "Error",
                QObject::tr("INI file wrong format"));
#endif
        return false;
    }
    if(!selectedCurrentDate)
    {
        /* add new file at a new line at ini file */
        fp = fopen(gssIniConfigFile, "a+");
        fprintf(fp, "\n%s", newFile);
        fclose(fp);
    }
    return true;
}

int InitialConfig::FindIni(char * auxMsg, unsigned int auxMsgLen)
{
    /* first look for gss_config.ini file in MASSIVA executable folder */
    if(chdir(exe_path) != 0)
    {
        snprintf(auxMsg, auxMsgLen, "MASSIVA executable folder not accesible");
        return -1;
    }

    FILE * fp = NULL;
    if((fp = fopen(gssIniConfigFile, "r")) == NULL)
    {
        /* look for check gss_config.ini file in home folder */
        snprintf(auxMsg, auxMsgLen, "%s%s%c%s", home_path, HOME_GSS_FOLDER, slashChar, gssIniConfigFile);
        if((fp = fopen(auxMsg, "r")) == NULL)
        {
            /* if not found, try to create folder and file in home */
            if(chdir(home_path) != 0)
            {
                snprintf(auxMsg, auxMsgLen, "home folder not accesible");
                return -1;
            }

            DIR *dir;
            if ((dir = opendir (HOME_GSS_FOLDER)) == NULL) {
                if (ENOENT == errno) {
                    int status = 0;
    #ifdef _WIN32
                    if((status = mkdir(HOME_GSS_FOLDER)) != 0)
    #else
                    if((status = mkdir(HOME_GSS_FOLDER, 0777)) != 0)
    #endif
                    {
                        snprintf(auxMsg, auxMsgLen,
                                 "Folder \"%s\" can't be created at \"%s\": %s",
                                 HOME_GSS_FOLDER, gss_dir, strerror(errno));
                        return -1;
                    }
                }
                else
                {
                    snprintf(auxMsg, auxMsgLen,
                             "Folder \"%s\" can't be accesed at \"%s\": %s",
                             HOME_GSS_FOLDER, gss_dir, strerror(errno));
                    return -1;
                }
            }
            if((fp = fopen(auxMsg, "w")) == NULL)
            {
                snprintf(auxMsg, auxMsgLen,
                         "File \"%s\" can't be created at \"%s%c%s\"",
                         gssIniConfigFile, gss_dir, slashChar, HOME_GSS_FOLDER);
                return -1;
            }
            fclose(fp);
            snprintf(gss_dir, strMaxLen, "%s%s", home_path, HOME_GSS_FOLDER);
        }
        if(chdir(gss_dir) != 0)
        {
            snprintf(auxMsg, auxMsgLen, "MASSIVA folder not found");
            return -1;
        }
    }
    return 0;
}

int InitialConfig::ConfigWorkspaceParseIni(char * auxMsg, unsigned int auxMsgLen)
{
    FILE * fp = NULL;

    /* parse gss_config.ini file */
    if((fp = fopen(gssIniConfigFile, "r")) == NULL)
    {
        snprintf(auxMsg, auxMsgLen, "INI file not found");
        return -1;
    }
    memset(workspace_dir, 0, strMaxLen);
    fseek(fp, 0, SEEK_END);/* seek the end of file */
    int length = ftell(fp);
    rewind(fp);

    if(fgets(workspace_dir, strMaxLen, fp) == NULL)
    {
        snprintf(auxMsg, auxMsgLen,
                "Error while getting workspace folder from \"%s%c%s\"", gss_dir,
                slashChar, gssIniConfigFile);
        fclose(fp);
        return -1;
    }
    if((workspace_dir[0] == '\n') || (workspace_dir[0] == '\r'))
    {
        snprintf(auxMsg, auxMsgLen,
                "Wrong empty line for folder at \"%s%c%s\"", gss_dir,
                slashChar, gssIniConfigFile);
        fclose(fp);
        return -1;
    }
    
    /* get config XML file name */
    memset(gssConfigFile, 0, strMaxLen);
    if(fgets(gssConfigFile, strMaxLen, fp) == NULL)
    {
        snprintf(auxMsg, auxMsgLen,
                "Error while getting XML file name from \"%s%c%s\"", gss_dir,
                slashChar, gssIniConfigFile);
        fclose(fp);
        return -1;
    }
    if((gssConfigFile[0] == '\n') || (gssConfigFile[0] == '\r'))
    {
        snprintf(auxMsg, auxMsgLen,
                "Wrong empty line for XML file name at \"%s%c%s\"", gss_dir,
                slashChar, gssIniConfigFile);
        fclose(fp);
        return -1;
    }
    int pos = ftell(fp);
    if(feof(fp) || (length == pos))
    {
        //no previous log file
        time_t timeRef = time(NULL);
        strftime(currentLogFolderName, strMaxLen, "%Y-%m-%d",
                localtime(&timeRef));
        fclose(fp);
    }
    else
    {
        memset(currentLogFolderName, 0, strMaxLen);
        if((fgets(currentLogFolderName, strMaxLen, fp)) == NULL)
        {
            snprintf(gssConfigFile, strMaxLen,
                    "Error while getting log folder name from \"%s%c%s\"",
                    gss_dir, slashChar, gssIniConfigFile);
            fclose(fp);
            return -1;
        }
        fclose(fp);
        if((currentLogFolderName[0] == '\n') || (currentLogFolderName[0] == '\r'))
        {
            snprintf(auxMsg, auxMsgLen,
                    "Wrong empty line for log folder name at \"%s%c%s\"",
                    gss_dir, slashChar, gssIniConfigFile);
            return -1;
        }
    }
    /* finally sanitize workspace configuration files */
    return SanitizeWorkspaceFile(auxMsg, auxMsgLen);
}

int InitialConfig::ConfigWorkspaceCommandOption(char * auxMsg,
        unsigned int auxMsgLen)
{
    if(configXMLFileCL[0] == 0)
    {
        /* if previous value has been already configured, use it */
        snprintf(configXMLFileCL, strMaxLen, "%s", auxMsg);
    }
    else
    {
        /* else copy it as the future previous value */
        snprintf(auxMsg, auxMsgLen, "%s", configXMLFileCL);
    }

    memset(gss_dir, 0, strMaxLen);
    char * getcwd_ret = getcwd(exe_path, strMaxLen);
    if(getcwd_ret == NULL)
    {
        snprintf(auxMsg, auxMsgLen,
                 "Error getting MASSIVA executable directory \"%s\": %s",
                 exe_path, strerror(errno));
        return -1;
    }
    memset(exe_path, 0, strMaxLen);
    strncpy(gss_dir, exe_path, strMaxLen);
    
    /* now parse workspace from command-line */
    /* get last / to split xml config path and file */
    char * xmlFileInit = NULL;
    xmlFileInit = strrchr(auxMsg, '\\');
    if(xmlFileInit == NULL)
        xmlFileInit = strrchr(auxMsg, '/');
    if(xmlFileInit == NULL)
    {
        strncpy(gss_dir, auxMsg, strMaxLen);
        snprintf(auxMsg, auxMsgLen,
            "Config parameter \"%s\" must be located inside a folder",
            gss_dir);
        strncpy(gss_dir, exe_path, strMaxLen);
        return -1;
    }
    xmlFileInit++;
    *(xmlFileInit-1) = '\0';
    snprintf(workspace_dir, strMaxLen, "%s", auxMsg);
    snprintf(gssConfigFile, strMaxLen, "%s", xmlFileInit);
    
    time_t timeRef = time(NULL);
    strftime(currentLogFolderName, strMaxLen, "%Y-%m-%d", localtime(&timeRef));
    
    /* finally sanitize workspace configuration files */
    return SanitizeWorkspaceFile(auxMsg, auxMsgLen);
}

void InitialConfig::ShowConfigFile(bool isIniFile)
{
#ifndef NO_QT
    QString configFilenameStr;
    if(isIniFile)
    {
        configFilenameStr = QString(gss_dir) + "/" + gssIniConfigFile;
    }
    else
    {
        configFilenameStr = QString(workspace_dir_full) + "/" + gssConfigFile;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(configFilenameStr));

#endif /* NO_QT */
    return;
}

int InitialConfig::UpdateConfigGSS(char * newConfigPath,
        unsigned int newConfigPathMaxLen)
{
    /* if XML command-line is being used, this file will be updated */
    if(configXMLFileCL[0] != 0)
    {
        snprintf(configXMLFileCL, strMaxLen, "%s", newConfigPath);
    }
    else
    {
        /* otherwise, the gss_config.ini file will be updated */
        
        /* get last / to split xml config path and file */
        char * xmlFileInit = strrchr(newConfigPath, '/')+1;
        *(xmlFileInit-1) = '\0';
        uint32_t minLen = strlen(workspace_dir);
        if(minLen > strlen(newConfigPath))
            minLen = strlen(newConfigPath);

        /* go to GSS dir and open gss_config.ini */
        if(chdir(gss_dir) != 0)
        {
            snprintf(newConfigPath, newConfigPathMaxLen, "MASSIVA folder not found");
            return -1;
        }
        /* check if there is a 3rd line (logs folder )*/
        FILE * fp = fopen(gssIniConfigFile, "r");
        if(fp == NULL)
        {
            snprintf(newConfigPath, newConfigPathMaxLen, "MASSIVA file not found");
            return -1;
        }
        fseek(fp, 0, SEEK_END);/* seek to end of file */
        int pos = 0, length = ftell(fp);
        rewind(fp);
        unsigned int numberOfLines = 0;
        char gssIniLogFolderInfo[strMaxLen];
        while (!feof(fp) && (pos < length))
        {
            if(fgets(gssIniLogFolderInfo, strMaxLen, fp) == NULL)
            {
                snprintf(newConfigPath, newConfigPathMaxLen, "Ini file parsing error");
                return -1;
            }
            pos = ftell(fp);
            numberOfLines++;
        }
        fclose(fp);
        fp = fopen(gssIniConfigFile, "w");
        if(fp == NULL)
        {
            snprintf(newConfigPath, newConfigPathMaxLen, "INI file not editable");
            return -1;
        }
    #ifdef _WIN32
        if(newConfigPath[0] != gss_dir[0])
        {
            /* in win32 path begins with drive letter */
            /* if they are different, there is no relative path */
            fprintf(fp, "%s", newConfigPath);
        }
        else
    #endif
        {
            /* convert absolute to relative (to GSS dir) xml config path */
            uint32_t idx=0;
            int32_t last_slash = -1;

            /* first compare both strings */
            while(idx < strMaxLen)
            {
    #ifdef _WIN32
                /* in Windows we will have '\\' and '/' for comparing */
                if(newConfigPath[idx] == '/')
                    newConfigPath[idx] = '\\';
    #endif
                if((gss_dir[idx] == '\0') || (newConfigPath[idx] == '\0') ||
                    (gss_dir[idx] != newConfigPath[idx]))
                {
                    if((gss_dir[idx] == '\0') && (newConfigPath[idx] != '\0'))
                    {
                        last_slash = idx;
                    }   
                    break;
                }
    #ifdef _WIN32
                if(newConfigPath[idx] == '\\')
                    newConfigPath[idx] = '/';
    #endif
                if(newConfigPath[idx] == '/')
                    last_slash = idx;
                idx++;
            }

            if((idx < strMaxLen) && (gss_dir[idx] != '\0'))
            {
                /* if gss_dir path remains, must go up in file tree */
                do
                {
                    if(gss_dir[idx] == slashChar)
                        fprintf(fp, "../");
                    idx++;
                }while(gss_dir[idx] != '\0');
                /* needed one last slash as there is no one in string */
                fprintf(fp, "../");
            }
            if(last_slash != -1)
            {
                /* if xml config path remains, must add it now */
                fprintf(fp, "%s", &newConfigPath[last_slash+1]);
            }
        }

        /* write xml config file to gss_config.ini */
        fprintf(fp, "\n%s", xmlFileInit);
        /* write log folder name to gss_config.ini if necessary*/
        if((numberOfLines == 3) && (currentLogFolderName[0] != '\n') &&
                (currentLogFolderName[0] != '\r'))
            fprintf(fp, "\n%s", gssIniLogFolderInfo);
        fclose(fp);
    }
    
    /* don't have to chdir again as gss_dir is needed for reset */
    return 0;
}

int InitialConfig::RestoreConfigGSS()
{
#ifndef NO_QT
    /* get to dir and open gss_config.ini */
    if(chdir(gss_dir) != 0)
    {
        QMessageBox::information(0, "Error",
                QObject::tr("INI folder not found"));
        return -1;
    }
    FILE * fp = fopen(gssIniConfigFile, "w");
    if(fp == NULL)
    {
        QMessageBox::information(0, "Error",
                QObject::tr("INI file not editable"));
        return -1;
    }
    /* copy folder and xml file (to be backwards compatible) to gss_config.ini */
    fprintf(fp, "%s\n%s", workspace_dir_last_working,
            gssConfigFile_last_working);
    fclose(fp);
#endif
    return 0;
}

void InitialConfig::SetLastWorkingInfo()
{
    memcpy(gssConfigFile_last_working, gssConfigFile, strMaxLen);
    memcpy(workspace_dir_last_working, workspace_dir, strMaxLen);
    last_working_valid = true;
}

bool InitialConfig::GetLastWorkingValid()
{
    return last_working_valid;
}

int InitialConfig::SanitizeWorkspaceFile(char * auxMsg, unsigned int auxMsgLen)
{    
    /* sanitize gssConfigFile and workspace_dir */
    if(gssConfigFile[strlen(gssConfigFile)-2] == '\r')
    {
        gssConfigFile[strlen(gssConfigFile)-2] = '\0'; //remove \r
        gssConfigFile[strlen(gssConfigFile)+1] = '\0'; //remove \n
    }
    else if(gssConfigFile[strlen(gssConfigFile)-1] == '\n')
    {
        gssConfigFile[strlen(gssConfigFile)-1] = '\0'; //remove \n
    }
    
    if(workspace_dir[strlen(workspace_dir)-2] == '\r')
    {
        workspace_dir[strlen(workspace_dir)-2] = '\0'; //remove \r
        workspace_dir[strlen(workspace_dir)+1] = '\0'; //remove \n
    }
    else if(workspace_dir[strlen(workspace_dir)-1] == '\n')
    {
        workspace_dir[strlen(workspace_dir)-1] = '\0'; //remove \n
    }

    /* check if it is full or relative workspace path */
#if defined (_WIN32) || defined (__CYGWIN__)
    if(workspace_dir[1] == ':')
#else
    if(workspace_dir[0] == '/')
#endif
    {
        memset(workspace_dir_full, 0, strMaxLen),
        snprintf(workspace_dir_full, strMaxLen, "%s", workspace_dir);
    }
    else
    {
        std::string temp_workspace_dir(workspace_dir);
        std::string temp_workspace_dir_full(gss_dir);
        size_t posWS = 0, posGSS = temp_workspace_dir_full.size();
        while(temp_workspace_dir.find("../", posWS) != std::string::npos)
        {
            posWS += 3; //move forward
            posGSS = temp_workspace_dir_full.rfind(slashChar, posGSS-1);
        }
        snprintf(workspace_dir_full, strMaxLen, "%s%c%s",
                temp_workspace_dir_full.substr(0, posGSS).c_str(),
                slashChar, temp_workspace_dir.substr(posWS).c_str());
    }
    
    /* check if XML file exists */
    if(chdir(workspace_dir) != 0)
    {
        snprintf(auxMsg, auxMsgLen, "Error while changing dir to \"%s\": %s",
                workspace_dir, strerror(errno));
        return -1;
    }
    FILE * fp = fopen(gssConfigFile, "r");
    
    /* go back to gss_dir before any check */
    if(chdir(gss_dir) != 0)
    {
        snprintf(auxMsg, auxMsgLen, "Error while changing dir to \"%s\": %s",
                gss_dir, strerror(errno));
        fclose(fp);
        return -1;
    }
        
    /* check if config XML file exists */
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    else
    {
        snprintf(auxMsg, auxMsgLen, "\"%s%c%s\" not found", workspace_dir,
                slashChar, gssConfigFile);
        return -1;
    }
    
    /* create workspace_dir_last to be used with direct options */
    strncpy(workspace_dir_last, workspace_dir, strMaxLen);
    return 0;
}

int InitialConfig::CheckGssFolderWritable(char * auxMsg, unsigned int auxMsgLen)
{
    memset(exe_path, 0, strMaxLen);
    char * getcwd_ret = getcwd(exe_path, strMaxLen);
    if(getcwd_ret == NULL)
    {
        snprintf(auxMsg, auxMsgLen,
                 "Error getting MASSIVA executable directory \"%s\": %s",
                 exe_path, strerror(errno));
        return -1;
    }

    memset(home_path, 0, strMaxLen);
#if defined (_WIN32) || defined (__CYGWIN__)
    snprintf(home_path, strMaxLen, "%s%s\\",
             getenv("HOMEDRIVE"), getenv("HOMEPATH"));
#else
    snprintf(home_path, strMaxLen, "%s/", getenv("HOME"));
#endif
    snprintf(gss_dir, strMaxLen, "%s%s", home_path, HOME_GSS_FOLDER);

    if(access(gss_dir, W_OK | R_OK) != 0)
    {
        /* if home folder is not readable and writable try gss exe folder  */
        char wrongHomeDir[strMaxLen];
        memset(wrongHomeDir, 0, strMaxLen);
        strncpy(wrongHomeDir, gss_dir, strMaxLen);

        if(access(exe_path, W_OK | R_OK) != 0)
        {
            snprintf(auxMsg, auxMsgLen,
                     "No writable folder found: home \"%s\", exe: \"%s\"",
                     wrongHomeDir, exe_path);
            return -1;
        }
        /* if found and accesible, gss_dir is exe_path */
        strncpy(gss_dir, exe_path, strMaxLen);
    }
    if(chdir(gss_dir) != 0)
    {
        snprintf(auxMsg, auxMsgLen, "Error while opening \"%s\" folder",
                 gss_dir);
        return -1;
    }
    strncpy(gss_dir_last, gss_dir, strMaxLen);
    return 0;
}
