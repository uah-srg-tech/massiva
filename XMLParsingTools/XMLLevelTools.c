/**
 * \file	XMLLevelTools.c
 * \brief	functions for parsing XML level files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		21/03/2013 at 10:44:12
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <libxml/xmlreader.h>
#include <string.h>
#include "XMLTools.h"					/* GetXMLAttributeValueByName, ... */
#include "XMLFormatTools.h"				/* CreateFormatArray */
#include "XMLFilterTools.h"				/* CreateFilterArray */
#include "XMLExportTools.h"				/* CreateExportArray */
#include "XMLImportTools.h"				/* CreateImportArray */

enum {
    WRONG_LEVEL_ID = -101,
    WRONG_LEVEL_IOTYPE_INOUT = -102,
    WRONG_LEVEL_IOTYPE_IN = -103,
    WRONG_LEVEL_IOTYPE_OUT = -104
};

static char filenameError[ATTR_SIZE];
static unsigned int errorNumbers;
static levelTypeError levelError;

int ParseLevel(xmlNodePtr interf, level * currentLevel, level * previousLevel,
        unsigned int levelNumber, unsigned int elementNumber, ioTypes portType,
        const char * relativePath)
{
    int status;
    xmlNodePtr level = NULL;

    levelError = CONFIG_OTHER;
    filenameError[0] = '0';
    errorNumbers = 0;

    /* get "Level" handle */
    if((status = GetXMLChildElementByIndex(interf, elementNumber, &level)) != 0)
    {
        return status;
    }

    if((portType == INOUT_PORT) && (strncmp((char*)level->name, "LevelInOut", 10) != 0))
    {
        return WRONG_LEVEL_IOTYPE_INOUT;
    }
    else if((portType == IN_PORT) && (strncmp((char*)level->name, "LevelIn", 7) != 0))
    {
        return WRONG_LEVEL_IOTYPE_IN;
    }
    else if((portType == OUT_PORT) && (strncmp((char*)level->name, "LevelOut", 8) != 0))
    {
        return WRONG_LEVEL_IOTYPE_OUT;
    }

    /* check if levelNumber is the same as int tag */
    if((status = GetXMLAttributeValueByName (level, "id", filenameError, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(atoi(filenameError) != (int)levelNumber)
    {
        errorNumbers = atoi(filenameError);
        return WRONG_LEVEL_ID;
    }
    if(portType != OUT_PORT)
    {
        memset(filenameError, 0, ATTR_SIZE);
        /* get xml TCformat file */ 
        if((status = GetFileAsXMLChildOnlyAttribute (level, "TCformat",
                filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* parse TCformat file */
        if((status = CreateFormatArray(filenameError, relativePath,
                &currentLevel->in.TCFields, &currentLevel->in.numberOfTCFields,
                &currentLevel->in.numberOfFDICTCFields, 
                &currentLevel->in.crcTCFieldRefs,
                &currentLevel->in.numberOfcrcTCFields)) != 0)
        {
            levelError = TC_FORMAT;
            return status;
        }
    }
    if(portType != IN_PORT)
    {
        memset(filenameError, 0, ATTR_SIZE);
        /* get xml TMformat file */ 
        if((status = GetFileAsXMLChildOnlyAttribute (level, "TMformat",
                filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* parse TMformat file */
        if((status = CreateFormatArray(filenameError, relativePath,
                &currentLevel->out.TMFields, &currentLevel->out.numberOfTMFields,
                &currentLevel->out.numberOfFDICTMFields, 
                &currentLevel->out.crcTMFieldRefs,
                &currentLevel->out.numberOfcrcTMFields)) != 0)
        {
            levelError = TM_FORMAT;
            return status;
        }
        memset(filenameError, 0, ATTR_SIZE);
        /* get xml filter file */
        if((status = GetFileAsXMLChildOnlyAttribute (level, "inputFilter",
                filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* parse filter file */
        if((status = CreateFilterArray(filenameError, relativePath,
                &currentLevel->out.boolVars, &currentLevel->out.numberOfBoolVars,
                &currentLevel->out.filters, &currentLevel->out.numberOfFilters,
                &currentLevel->out.typeOfFilters, currentLevel->out.TMFields,
                currentLevel->out.numberOfTMFields)) != 0)
        {
            levelError = FILTER;
            return status;
        }
    }

    /* if level==0, no import/export */
    if(levelNumber==0)
    {
        return 0;
    }

    if(portType != OUT_PORT)
    {
        memset(filenameError, 0, ATTR_SIZE);
        /* get xml export file */
        if((status = GetFileAsXMLChildOnlyAttribute (level,
                "export_to_prev_Level", filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* parse export file */
        if((status = CreateExportArray(filenameError, relativePath,
                &previousLevel->in.exportFields, &previousLevel->in.numberOfExportFields,
                &previousLevel->in.ActiveDICs,  &previousLevel->in.numberOfActiveDICs,
                previousLevel->in.TCFields, previousLevel->in.numberOfTCFields,
                currentLevel->in.TCFields, currentLevel->in.numberOfTCFields,
                previousLevel->in.numberOfFDICTCFields)) != 0)
        {
            levelError = EXPORT;
            return status;
        }
    }

    if(portType != IN_PORT)
    {
        memset(filenameError, 0, ATTR_SIZE);
        /* get xml import file */
        if((status = GetFileAsXMLChildOnlyAttribute (level,
                "import_from_prev_Level", filenameError, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* parse import file */
        if((status = CreateImportArray(filenameError, relativePath, 
                &currentLevel->out.importInBytes, &currentLevel->out.virtualFields,
                &currentLevel->out.numberOfVirtualFields,
                previousLevel->out.TMFields, previousLevel->out.numberOfTMFields,
                currentLevel->out.TMFields, currentLevel->out.numberOfTMFields))
                != 0)
        {
            levelError = IMPORT;
            return status;
        }
    }
    return 0;
}

void DisplayParseLevelError (int status, int port, int level, 
	char * msg, unsigned int maxMsgSize)
{
    unsigned int len = snprintf(msg, maxMsgSize, "Error in port %d level %d",
            port, level);
    switch(levelError)
    {
        case TC_FORMAT: 
            len += snprintf(&msg[len], maxMsgSize-len,
                    " TCformat file \"%s\". ", filenameError);
            DisplayCreateFormatArrayError(status, &msg[len], maxMsgSize-len);
            break;

        case TM_FORMAT: 
            len += snprintf(&msg[len], maxMsgSize-len,
                    " TMformat file \"%s\". ", filenameError);
            DisplayCreateFormatArrayError(status, &msg[len], maxMsgSize-len);
            break;

        case FILTER:
            len += snprintf(&msg[len], maxMsgSize-len,
                    " filter file \"%s\". ", filenameError);
            DisplayCreateFilterArrayError(status, &msg[len], maxMsgSize-len);
            break;

        case EXPORT:
            len += snprintf(&msg[len], maxMsgSize-len,
                    " export file \"%s\". ", filenameError);
            DisplayCreateExportArrayError(status, &msg[len], maxMsgSize-len);
            break;

        case IMPORT:
            len += snprintf(&msg[len], maxMsgSize-len,
                    " import file \"%s\". ", filenameError);
            DisplayCreateImportArrayError(status, &msg[len], maxMsgSize-len);
            break;

        default:
            len += snprintf(&msg[len], maxMsgSize-len, ":  ");
            switch (status)
            {
                case WRONG_LEVEL_ID:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Wrong level ID %d", errorNumbers);
                    break;

                case WRONG_LEVEL_IOTYPE_INOUT:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Different I/O type (In/Out) in port %d level %d",
                            port, level);
                    break;

                case WRONG_LEVEL_IOTYPE_IN:
                    snprintf(&msg[len], maxMsgSize-len,
                            "Different I/O type (In) in port %d level %d",
                            port, level);
                    break;

                case WRONG_LEVEL_IOTYPE_OUT:	  
                    snprintf(&msg[len], maxMsgSize-len,
                            "Different I/O type (Out) in port %d level %d",
                            port, level);
                    break;

                default:
                    XMLerrorInfo(status, &msg[len], maxMsgSize-len);
                    break;
            }
            break;
    }
    memset(filenameError, 0, ATTR_SIZE);
    return;
}