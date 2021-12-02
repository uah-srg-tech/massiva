/**
 * \file	ImportFields.c
 * \brief	functions for exporting info from import struct (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		04/03/2013 at 09:33:21
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdio.h>				/* snprintf */
#include <unistd.h>				/* uint8_t */
#include "../definitions.h"		/* formatField, boolVar, filter, ... */
#include "../CommonTools/GetSetFieldTools.h"		/* GetFieldAsUint */

enum {
    WRONG_VIRTUAL_REF_TYPE = -1
};

int importVirtualFields (const unsigned char * buffer,
	importVirtual * virtualFld, int numberOfImportVirtualFld,
	formatField * sourceFields, formatField * targetFields)
{
    int status, src, tgt, i=0;

    for(i=0; i<numberOfImportVirtualFld; ++i)
    {	 
        src = virtualFld[i].sourceRef;
        tgt = virtualFld[i].targetRef;

        switch(sourceFields[src].type)
        {
            case CSFIELD:
                if((status = GetFieldFromBufferAsUllong(buffer, 0,
                        MAX_PACKET_SIZE, sourceFields[src].offsetInBits,
                        sourceFields[src].info.sizeInBits,
                        &targetFields[tgt].info.virtualSizeValue)) < 0)
                {
                    return WRONG_VIRTUAL_REF_TYPE;
                }
                break;

            case CSFORMULAFIELD:
                if((status = GetFieldFromBufferAsUllong(buffer, 0,
                        MAX_PACKET_SIZE, sourceFields[src].offsetInBits,
                        sourceFields[src].info.formula.sizeInBits,
                        &targetFields[tgt].info.virtualSizeValue)) < 0)
                {
                    return WRONG_VIRTUAL_REF_TYPE;
                }
                break;

            case VSFIELD:
                    return WRONG_VIRTUAL_REF_TYPE;

            case FDICFIELD:
                if((status = GetFieldFromBufferAsUllong(buffer, 0,
                        MAX_PACKET_SIZE, sourceFields[src].offsetInBits,
                        sourceFields[src].info.check.sizeInBits,
                        &targetFields[tgt].info.virtualSizeValue)) < 0)
                {
                    return WRONG_VIRTUAL_REF_TYPE;
                }
                break;

            case VRFIELDSIZE:
                targetFields[tgt].info.virtualSizeValue =
                        sourceFields[src].info.virtualSizeValue;
                break;

            default:
                break;
        }
        if(((long long)(targetFields[tgt].info.virtualSizeValue) + (virtualFld[i].addSizeInBits / 8)) >= 0)
            targetFields[tgt].info.virtualSizeValue += virtualFld[i].addSizeInBits / 8;
        else
            targetFields[tgt].info.virtualSizeValue = 0;
    }
    return 0;
}

void DisplayImportFieldsError (int status, char * msg, int msgSize)
{
    switch (status)
    {
        case WRONG_VIRTUAL_REF_TYPE:
            snprintf(msg, msgSize, "Wrong Virtual ref type");
            break;
    }
    return;
}
