/**
 * \file	XMLExImTools.c
 * \brief	functions for parsing XML test procs import / export (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		07/03/2013 at 17:32:12
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include "XMLExportTools.h"		/* CreateExportArray, levelIn, levelOut */
#include "XMLImportTools.h"		/* CreateImportArray */

int CreateExportArraySelectDefault(const char * filename, const char * relativePath,
        levelIn * Previous, levelIn * Current,
        levelIn DefaultPrevious, levelIn DefaultCurrent)
{
    int status = 0;
    if((Previous->numberOfTCFields == 0) && (Current->numberOfTCFields == 0))
    {
        status = CreateExportArray(filename, relativePath,
                &Previous->exportFields, &Previous->numberOfExportFields,
                &Previous->ActiveDICs, &Previous->numberOfActiveDICs,
                DefaultPrevious.TCFields, DefaultPrevious.numberOfTCFields,
                DefaultCurrent.TCFields, DefaultCurrent.numberOfTCFields,
                DefaultPrevious.numberOfFDICTCFields);
    }
    else if(Previous->numberOfTCFields == 0)
    {
        status = CreateExportArray(filename, relativePath,
                &Previous->exportFields, &Previous->numberOfExportFields,
                &Previous->ActiveDICs, &Previous->numberOfActiveDICs,
                DefaultPrevious.TCFields, DefaultPrevious.numberOfTCFields,
                Current->TCFields, Current->numberOfTCFields,
                DefaultPrevious.numberOfFDICTCFields);
    }
    else if(Current->numberOfTCFields == 0)
    {
        status = CreateExportArray(filename, relativePath,
                &Previous->exportFields, &Previous->numberOfExportFields,
                &Previous->ActiveDICs, &Previous->numberOfActiveDICs,
                Previous->TCFields, Previous->numberOfTCFields,
                DefaultCurrent.TCFields, DefaultCurrent.numberOfTCFields,
                Previous->numberOfFDICTCFields);
    }
    else
    {
        status = CreateExportArray(filename, relativePath,
                &Previous->exportFields, &Previous->numberOfExportFields,
                &Previous->ActiveDICs, &Previous->numberOfActiveDICs,
                Previous->TCFields, Previous->numberOfTCFields,
                Current->TCFields, Current->numberOfTCFields,
                Previous->numberOfFDICTCFields);
    }
    return status;
}

int CreateImportArraySelectDefault(const char * filename, const char * relativePath,
        levelOut Previous, levelOut * Current,
        levelOut DefaultPrevious, levelOut DefaultCurrent)
{
    int status = 0;
    if((Previous.numberOfTMFields == 0) && (Current->numberOfTMFields == 0))
    {
        status = CreateImportArray(filename, relativePath,
                &Current->importInBytes,
                &Current->virtualFields, &Current->numberOfVirtualFields,
                DefaultPrevious.TMFields, DefaultPrevious.numberOfTMFields,
                DefaultCurrent.TMFields, DefaultCurrent.numberOfTMFields);
    }
    else if(Previous.numberOfTMFields == 0)
    {
        status = CreateImportArray(filename, relativePath,
                &Current->importInBytes,
                &Current->virtualFields, &Current->numberOfVirtualFields,
                DefaultPrevious.TMFields, DefaultPrevious.numberOfTMFields,
                Current->TMFields, Current->numberOfTMFields);
    }
    else if(Current->numberOfTMFields == 0)
    {
        status = CreateImportArray(filename, relativePath,
                &Current->importInBytes,
                &Current->virtualFields, &Current->numberOfVirtualFields,
                Previous.TMFields, Previous.numberOfTMFields,
                DefaultCurrent.TMFields, DefaultCurrent.numberOfTMFields);
    }
    else
    {
        status = CreateImportArray(filename, relativePath,
                &Current->importInBytes,
                &Current->virtualFields, &Current->numberOfVirtualFields,
                Previous.TMFields, Previous.numberOfTMFields,
                Current->TMFields, Current->numberOfTMFields);
    }
    return status;
}

void DisplayParseTestProcImExError(int status, levelTypeError levelError,
        char * msg, unsigned int maxMsgSize)
{
switch(levelError)
{
        case EXPORT:
            DisplayCreateExportArrayError (status, msg, maxMsgSize);
            break;

        case IMPORT:
            DisplayCreateImportArrayError (status, msg, maxMsgSize);
            break;

        default:
            break;
    }
}

void DisplayParsePeriodicTCExError(int status, levelTypeError levelError,
        char * msg, unsigned int maxMsgSize)
{
    switch(levelError)
    {
        case EXPORT:
            DisplayCreateExportArrayError (status, msg, maxMsgSize);
            break;

        default:
            break;
    }
}
