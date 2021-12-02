/**
 * \file	XMLImportTools.c
 * \brief	functions for parsing XML import files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/01/2013 at 10:46:22
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <string.h>
#include "XMLTools.h"                           /* GetXMLValueByTag, ... */
#include "XMLFormatTools.h"			/* getFormatFromXtext */
#include "../definitions.h"                     /* importStruct, formatField */

enum {
    IMPORT_FILE_NOT_FOUND = -11,
    IMPORT_FILE_PARSING_ERROR = -12,
    WRONG_FIELD_REF = -13,
    IMPORT_NOT_BEGIN_AT_BYTE = -14,
    FIELDS_NOT_CONSECUTIVE = -15,
    WRONG_VIRTUAL_FIELD_NAME = -16
};

static char errorNames[MAX_ERR_LEN];
static int errorImport = -1;

int CreateImportArray(const char * filename, const char * relativePath,
        unsigned int * import,
        importVirtual ** virtual, int * numberOfImportVirtual,
	formatField * sourceFields, unsigned int numberOfSourceFields, 
	formatField * targetFields, unsigned int numberOfTargetFields)
{
    int currentOffsetInBits = -1, currentVirtual = 0;
    unsigned int numberOfChildren = 0, item = 0, headerItems = 0;
    unsigned int src = 0, tgt = 0;
    char attrData[ATTR_SIZE];
    int status;
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL, child = NULL;
    char fullPath[MAX_STR_LEN];
    
    errorImport = -1;
    
    /* look for file */
    if(fileExists(filename, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
        return IMPORT_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return IMPORT_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return IMPORT_FILE_PARSING_ERROR;
    }
    /* NOTE: "from", "to" are redundant, checked at gss_config or TP files */

    /* check number of children */
    GetXMLNumChildren(root, &numberOfChildren);
    /* first pass to get element tags */
    for(item=0; item<numberOfChildren; ++item)
    {
        /* get next element */
        if((status = GetXMLChildElementByIndex (root, item, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }

        if(strncmp((char*)child->name, "virtualSize", 11) == 0)
        {
            *numberOfImportVirtual = *numberOfImportVirtual+1;
        }
        else if((strncmp((char*)child->name, "from", 4) == 0) ||
                (strncmp((char*)child->name, "to", 2) == 0))
        {
            headerItems++;
        }
    }
    *virtual = calloc((*numberOfImportVirtual), sizeof(importVirtual));
    
    /* sencond pass to get data */
    for(errorImport=0, item=headerItems; item<numberOfChildren; ++errorImport, ++item)
    {
        /* get next element */
        if((status = GetXMLChildElementByIndex (root, item, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        
        /* get field ref name */
        if((status = GetXMLValueFromAttributeOrHRefTag(child, "FieldRef",
                attrData, ATTR_SIZE)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        GetXMLNumChildren(child, &src);
        
        if(src == 0)//FieldRef is an attribute
        {
            for(src=0; src<numberOfSourceFields; ++src)
            {
                if(strncmp(sourceFields[src].name, attrData,
                        MAX_FIELD_NAME_LEN) == 0)
                    break;
            }
            if(src == numberOfSourceFields)
            {
                xmlFreeDoc(doc);
                xmlCleanupParser();
                memset(errorNames, 0, MAX_ERR_LEN);
                strncpy(errorNames, attrData, MAX_ERR_LEN);
                return WRONG_FIELD_REF;	
            }
        }
        else//FieldRef is a tag 
        {
            if((status = getFormatFromXtext(attrData, strlen(attrData),
                    sourceFields, numberOfSourceFields, &src)) != 0)
            {
                xmlFreeDoc(doc);
                xmlCleanupParser();
                memset(errorNames, 0, MAX_ERR_LEN);
                strncpy(errorNames, attrData, MAX_ERR_LEN);
                return WRONG_FIELD_REF;
            }
        }

        /* get element tag to distinguish between dataSource and virtualSize */
        if(strncmp((char*)child->name, "dataSource", 10) == 0)
        {
            /* FIXME: What if no-consecutive "dataSource" fields ? */
            int previousFieldRef = 0;

            if(currentOffsetInBits < 0)
            {
                *import = sourceFields[src].offsetInBits / 8;
                if((sourceFields[src].offsetInBits % 8) != 0)
                {
                    /* import doesn't begin at a byte position */
                    *import = 0;
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    memset(errorNames, 0, MAX_ERR_LEN);
                    strncpy(errorNames, sourceFields[src].name, MAX_ERR_LEN);
                    return IMPORT_NOT_BEGIN_AT_BYTE;
                }

                switch(sourceFields[src].type)
                {
                    case CSFIELD:
                        currentOffsetInBits = sourceFields[src].offsetInBits +
                                sourceFields[src].info.sizeInBits;
                        previousFieldRef = -1;
                        break;  

                    case CSFORMULAFIELD:
                        currentOffsetInBits = sourceFields[src].offsetInBits +
                                sourceFields[src].info.formula.sizeInBits;
                        previousFieldRef = -1;
                        break;  

                    case VSFIELD:
                        currentOffsetInBits = sourceFields[src].offsetInBits +
                                sourceFields[src].info.variable.maxSizeInBits;
                        previousFieldRef = src;  
                        break;

                    case FDICFIELD:
                        currentOffsetInBits = sourceFields[src].offsetInBits +
                                sourceFields[src].info.check.sizeInBits;
                        previousFieldRef = -1;
                        break;

                    case VRFIELDSIZE:
                        xmlFreeDoc(doc);
                        xmlCleanupParser();
                        memset(errorNames, 0, MAX_ERR_LEN);
                        strncpy(errorNames, sourceFields[src].name, MAX_ERR_LEN);
                        return WRONG_FIELD_REF;

                    default:
                            break;
                }
            }
            else if(((previousFieldRef < 0) && (currentOffsetInBits != sourceFields[src].offsetInBits)) ||
                    ((previousFieldRef > 0) && (sourceFields[src].type != FDICFIELD)) ||
                    ((previousFieldRef > 0) && (sourceFields[src].type == FDICFIELD) && (sourceFields[src].info.check.fieldRef != (unsigned int)previousFieldRef)))
            {
                    /* fields are not consecutive. not yet implemented */
                xmlFreeDoc(doc);
                xmlCleanupParser();
                memset(errorNames, 0, MAX_ERR_LEN);
                strncpy(errorNames, sourceFields[src].name, MAX_ERR_LEN);
                return FIELDS_NOT_CONSECUTIVE;
            }
            /* TODO: get and use "leftTrim" and "rightTrim" */
        }
        else // if(strncmp((char*)child->name, "virtualSize", 11) == 0)
        {
            virtual[0][currentVirtual].sourceRef = src;

            /* get virtual target field size name "to" */
            if((status = GetXMLValueFromAttributeOrHRefTag(child, "to",
                    attrData, ATTR_SIZE)) != 0)
            {
                xmlFreeDoc(doc);
                xmlCleanupParser();
                return status;
            }
            GetXMLNumChildren(child, &tgt);
            if(tgt < 2)//to is an attribute
            {
                for(tgt=0; tgt<numberOfTargetFields; ++tgt)
                {
                    if(strncmp(targetFields[tgt].name, attrData, MAX_FIELD_NAME_LEN) == 0)
                        break;
                }
                if((tgt == numberOfTargetFields) || (targetFields[tgt].type != VRFIELDSIZE))
                {
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    memset(errorNames, 0, MAX_ERR_LEN);
                    strncpy(errorNames, attrData, MAX_ERR_LEN);
                    return WRONG_VIRTUAL_FIELD_NAME;
                }
            }
            else//to is a tag 
            {
                if((status = getFormatFromXtext(attrData, strlen(attrData),
                        targetFields, numberOfTargetFields, &tgt)) != 0)
                {
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    memset(errorNames, 0, MAX_ERR_LEN);
                    strncpy(errorNames, attrData, MAX_ERR_LEN);
                    return WRONG_VIRTUAL_FIELD_NAME;
                }
            }
            virtual[0][currentVirtual].targetRef = tgt;

            /* get virtual target field size name "addSize" */
            if((status = GetXMLAttributeValueByName(child, "addSize",
                    attrData, ATTR_SIZE)) != 0) 
            {
                xmlFreeDoc(doc);
                xmlCleanupParser();
                return status;
            }
            virtual[0][currentVirtual].addSizeInBits = atoi(attrData);

            /* get virtual target field size name "unit" */
            if((status = GetXMLAttributeValueByName(child, "unit",
                    attrData, ATTR_SIZE)) != 0)
            {
                xmlFreeDoc(doc);
                xmlCleanupParser();
                return status;
            }
            if(strncmp(attrData, "bytes", 5) == 0)
            {
                virtual[0][currentVirtual].addSizeInBits *= 8;
            }
            else //if(strncmp(attrData, "bits", 4) == 0)
            {
                virtual[0][currentVirtual].addSizeInBits *= 1; 
            }
            currentVirtual++;
        }
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}

void DisplayCreateImportArrayError (int status, char * msg,
        unsigned int maxMsgSize)
{
    unsigned int len = 0;
    switch (status)
    {
        case IMPORT_FILE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "Import file not found");
            break;

        case IMPORT_FILE_PARSING_ERROR:
            snprintf(msg, maxMsgSize, "Import file parsing error");
            break;
            
        case WRONG_FIELD_REF:
            snprintf(msg, maxMsgSize, "Wrong FieldRef %s", errorNames);
            break;

        case IMPORT_NOT_BEGIN_AT_BYTE:
            snprintf(msg, maxMsgSize, "Import field %s doesn't "
                    "begin at a byte position", errorNames);
            break;

        case FIELDS_NOT_CONSECUTIVE:
            snprintf(msg, maxMsgSize, "Field %s is not consecutive "
                    "to previous one. Feature not yet implemented", errorNames);
            break;

        case WRONG_VIRTUAL_FIELD_NAME:
            snprintf(msg, maxMsgSize, "Field %s is not a virtual one",
                    errorNames);
            break;

        default:
            if(errorImport != -1)
            {
                len = snprintf(msg, maxMsgSize, "In Import %d: ", errorImport);
            }
            XMLerrorInfo(status, &msg[len], maxMsgSize-len);
            break; 
    }
    return;
}
