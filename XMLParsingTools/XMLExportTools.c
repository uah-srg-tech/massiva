/**
 * \file	XMLExportTools.c
 * \brief	functions for parsing XML export files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		01/03/2013 at 17:24:02
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#pragma GCC diagnostic ignored "-Wunused-result"

#include <string.h>
#include "XMLTools.h"			/* GetXMLValueByTag, ... */
#include "XMLExportSettingTools.h"	/* CreateExportSetting */
					/* sizesType, exportStruct, formatField */ 
/*
 * Maximum number of export sizes
 */
#define MAX_SIZES			10

enum {
    TOO_MUCH_SIZES = -50,
    WRONG_SIZE_ID = -51,
    WRONG_SIZE_FIELD_REF = -52,
    FILENAME_SIZE_TOO_BIG = -53,
    FILE_NOT_FOUND = -54,
    TOO_MUCH_EXPORT_FIELDS = -55,
    TOO_MUCH_DICS = -56,
    WRONG_DIC_ID = -57,
    WRONG_SOURCE_FIELD_DIC_REF = -58,
    MORE_DICS_THAN_FDIC_DEFINED = -59,
    EXPORT_FILE_NOT_FOUND = -60,
    EXPORT_FILE_PARSING_ERROR = -61,
    FILE_LINE_NOT_FOUND = -62,
    SETTINGS_TAG_NOT_FOUND = -63,
};

static char errorNames[MAX_ERR_LEN];
static int errorNumbers = 0, errorSizes = -1, errorSettings = -1, errorDics = -1;

static int ParseSize(xmlNodePtr element, formatField * sourceFields,
	unsigned int numberOfSourceFields, char attrData[ATTR_EXPORT_SIZE], sizesType * sizes,
	int index);

int CreateExportArray(const char * filename, const char * relativePath,
        levelExport ** exportField, unsigned int * numberOfExportFields,
	unsigned int ** DICs, unsigned int * numberOfDICs,
	formatField * targetFields, unsigned int numberOfTargetFields,
	formatField * sourceFields, unsigned int numberOfSourceFields,
	unsigned int numberOfTargetFDIC)
{
    int status = 0;
    unsigned int item = 0, src = 0, numberOfSizes = 0, headerItems = 0;
    char attrData[ATTR_EXPORT_SIZE];
    sizesType sizes[MAX_SIZES];
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL, child = NULL, greatchild = NULL;
    char fullPath[MAX_STR_LEN];
    
    errorNumbers = 0;
    errorSizes = -1;
    errorSettings = -1;
    errorDics = -1;
    
    /* look for file */
    if(fileExists(filename, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
        return EXPORT_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if(doc == NULL)
    {
        return EXPORT_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return EXPORT_FILE_PARSING_ERROR;
    }
    /* check if "from" or "to" tags exist */
    if((status = GetXMLChildElementByIndex (root, 0, &child)) != 0)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return status;
    }
    if(strncmp((char*)child->name, "from", 4) == 0)
    {
        headerItems++;
        if((status = GetXMLChildElementByIndex (root, 1, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        if(strncmp((char*)child->name, "to", 2) == 0)
            headerItems++;
    }
    else if(strncmp((char*)child->name, "to", 2) == 0)
        headerItems++;
    
    if(headerItems)
    {
        if((status = GetXMLChildElementByIndex (root, headerItems, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
    }

    /* check if "Sizes" exists */
    if(strncmp((char*)child->name, "Sizes", 5) == 0)
    {
        /* check number of children */
        GetXMLNumChildren(child, &numberOfSizes);
        if(numberOfSizes > MAX_SIZES)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            errorNumbers = numberOfSizes;
            return TOO_MUCH_SIZES;
        }
        for(item=0; item<numberOfSizes; item++)
        {
            sizes[item].sizeOrRef = 0;
            sizes[item].addSizeInBits = 0;
            if((status = GetXMLChildElementByIndex (child, 0, &greatchild)) != 0)
            {
                xmlFreeDoc(doc);
                xmlCleanupParser();
                return status;
            }

            if((status = ParseSize(greatchild, sourceFields,
                    numberOfSourceFields, attrData, sizes, item)) != 0)
            {
                xmlFreeDoc(doc);
                xmlCleanupParser();
                errorSizes = item;
                return status;
            }
        }
        /* next big field must be "settings" because is a compulsory field */
        /* get handle for settings */
        if((status = GetXMLChildElementByIndex (root, headerItems+1, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
    }

    if(strncmp((char*)child->name, "settings", 8) != 0)
    {
        return SETTINGS_TAG_NOT_FOUND;
    }
    /* check number of settings */
    GetXMLNumChildren(child, numberOfExportFields);
    if(*numberOfExportFields> MAX_EXPORT_FIELDS)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        errorNumbers = *numberOfExportFields;
        *numberOfExportFields = 0;
        return TOO_MUCH_EXPORT_FIELDS;
    }
    *exportField = calloc((*numberOfExportFields), sizeof(levelExport));

    for(item=0; item<*numberOfExportFields; item++)
    {
        if((status = GetXMLChildElementByIndex (child, item, &greatchild)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            errorSettings = item;
            return status;
        }

        if((status = CreateExportSetting(greatchild, exportField, targetFields,
                numberOfTargetFields, sourceFields, numberOfSourceFields,
                attrData, errorNames, sizes, numberOfSizes, item)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            errorSettings = item;
            return status;
        }
    }
    closeXMLExportFileOpened ();

    /* check if "activateDICs" exists */
    GetXMLNumChildren(root, &src);
    if(src != 1)
    {
        /* get last handle */
        if((status = GetXMLChildElementByIndex (root, src-1, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        /* check if last handle tag is "activateDICs" */
        if(strncmp((char*)child->name, "activateDICs", 12) == 0)
        {
            /* check number of children */
            GetXMLNumChildren(child, numberOfDICs);
            if(*numberOfDICs > MAX_DICS)
            {
                xmlFreeDoc(doc);
                xmlCleanupParser();
                errorNumbers = *numberOfDICs;
                return TOO_MUCH_DICS;
            }
            if(*numberOfDICs > numberOfTargetFDIC)
            {
                xmlFreeDoc(doc);
                xmlCleanupParser();
                errorNumbers = *numberOfDICs;
                return MORE_DICS_THAN_FDIC_DEFINED;
            }

            *DICs = calloc((*numberOfDICs), sizeof(int));

            for(item=0; item<*numberOfDICs; item++)
            {
                if((status = GetXMLChildElementByIndex (child, 0, &greatchild)) != 0)
                {
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    errorDics = item;
                    return status;
                }
                /* get DIC id */
                if((status = GetXMLAttributeValueByName (greatchild, "id", attrData, ATTR_EXPORT_SIZE)) != 0)
                {
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    errorDics = item;
                    return status;
                }
                if(atoi(attrData) != (int)item)
                { 
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    errorNumbers = atoi(attrData);
                    return WRONG_DIC_ID;
                }
                /* get DIC DICRef */
                if((status = GetXMLAttributeValueByName (greatchild, "DICRef",
                        attrData, ATTR_EXPORT_SIZE)) != 0)
                {
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    errorDics = item;
                    return status;
                }
                for(src=0; src<numberOfTargetFields; ++src)
                {
                    if(strncmp(targetFields[src].name, attrData, MAX_FIELD_NAME_LEN) == 0)
                        break;
                }
                if(src == numberOfTargetFields)
                {  
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    memset(errorNames, 0, MAX_ERR_LEN);
                    strncpy(errorNames, attrData, MAX_ERR_LEN);
                    errorDics = item;
                    return WRONG_SOURCE_FIELD_DIC_REF;
                }
                DICs[0][item] = src;
            }
        }
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}

static int ParseSize(xmlNodePtr element, formatField * sourceFields,
	unsigned int numberOfSourceFields, char attrData[ATTR_EXPORT_SIZE], sizesType * sizes,
	int index)
{
    int status;
    unsigned int src=0, numAttributes = 0;
    unsigned char src_exists = 0;
    char lastByte;

    /* get size id */
    if((status = GetXMLAttributeValueByName (element, "id", attrData, ATTR_EXPORT_SIZE)) != 0)
    {
        return status;
    }
    if(atoi(attrData) != index)
    { 
        errorNumbers = atoi(attrData);
        return WRONG_SIZE_ID;
    }
    GetXMLNumAttributes(element, &numAttributes);
    
    char sizeType[20];
    if((status = SearchXMLAttributeValueByName(element, "type", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(attrData[0] != '\0')
    {
        numAttributes--;
        strncpy(sizeType, &attrData[13], 20);//remove initial "gss:GSSExport"
        sizeType[0] = 's';//overwrite Uppercase "S"
    }
    else
    {
        strncpy(sizeType, (char*)(char*)element->name, 20);
    }
    if(strncmp(sizeType, "sizeFromFileLength", 18) == 0)
    {
        FILE * fp = NULL;
        unsigned int data = 0;
        sizes[index].type = C_SIZE;

        if((status = GetXMLAttributeLengthByName(element, "fromFile", &data)) != 0)
        {
            return status;
        }
        if(data > MAX_EXPORT_FILENAME_LEN)
        {
            errorNumbers = data;
            return FILENAME_SIZE_TOO_BIG;
        }

        if((status = GetXMLAttributeValueByName(element, "fromFile", attrData, ATTR_EXPORT_SIZE)) != 0)
        {
            return status;
        }

        /* file opened in hex mode */
        if((fp = fopen(attrData, "r")) == NULL)
        {
            memset(errorNames, 0, MAX_ERR_LEN);
            strncpy(errorNames, attrData, MAX_ERR_LEN);
            return FILE_NOT_FOUND;
        }

        /* look for file size */
        fseek(fp, 0, SEEK_END);  /* seek to end of file */
        sizes[index].sizeOrRef = ftell(fp);
        fseek(fp, -1, SEEK_END);  /* seek to last char of file */
        fread((char *)&lastByte, 1, 1, fp);
        fclose(fp);

        if(lastByte == '\n') /* check if file has a \n as last char */
            sizes[index].sizeOrRef--;
        sizes[index].sizeOrRef = sizes[index].sizeOrRef * 8 / 2;
        /* file in hex, size in bits*/
        /* get current file pointer in bits (hex mode) */
    }
    else if(strncmp(sizeType, "sizeFromFileLine", 16) == 0)
    {
        FILE * fp = NULL;
        unsigned int data = 0;
        char lineData[MAX_STR_OPERAND_LEN*2];
        unsigned int file_line = 1, aux = 0;
        sizes[index].type = C_SIZE;

        if((status = GetXMLAttributeLengthByName(element, "fromFile", &data)) != 0)
        {
            return status;
        }
        if(data > MAX_EXPORT_FILENAME_LEN)
        {
            errorNumbers = data;
            return FILENAME_SIZE_TOO_BIG;
        }

        if((status = GetXMLAttributeValueByName(element, "line", attrData, ATTR_EXPORT_SIZE)) != 0)
        {
            return status;
        }
        file_line = atoi(attrData);

        if((status = GetXMLAttributeValueByName(element, "fromFile", attrData, ATTR_EXPORT_SIZE)) != 0)
        {
            return status;
        }

        /* file opened in hex mode */
        if((fp = fopen(attrData, "r")) == NULL)
        {
            memset(errorNames, 0, MAX_ERR_LEN);
            strncpy(errorNames, attrData, MAX_ERR_LEN);
            return FILE_NOT_FOUND;
        }

        for(aux=0; aux<file_line; ++aux)
        {
            if(fgets(lineData, MAX_STR_OPERAND_LEN*2, fp) == NULL)
            {
                fclose(fp);
                fp = NULL;
                errorNumbers = file_line;
                return FILE_LINE_NOT_FOUND;
            }
        }
        fclose(fp);

        sizes[index].sizeOrRef = strlen(lineData) * 8 / 2;
        /* file in hex, size in bits*/
        /* get current file pointer in bits (hex mode) */
    }
    else if(strncmp(sizeType, "sizeInBits", 4) == 0)
    {
        sizes[index].type = C_SIZE;

        if((status = GetXMLAttributeValueByName (element, "from", attrData, ATTR_EXPORT_SIZE)) != 0)
        {
            return status;
        }

        for(src=0; src<numberOfSourceFields; ++src)
        {
            if(strncmp(sourceFields[src].name, attrData, MAX_FIELD_NAME_LEN) == 0)
                break;
        }
        if(src == numberOfSourceFields)
        {  
            memset(errorNames, 0, MAX_ERR_LEN);
            strncpy(errorNames, attrData, MAX_ERR_LEN);
            return WRONG_SIZE_FIELD_REF;
        }
        src_exists = 1;
        switch(sourceFields[src].type)
        {
            case CSFIELD:
                sizes[index].sizeOrRef = sourceFields[src].info.sizeInBits;
                break;

            case CSFORMULAFIELD:
                sizes[index].sizeOrRef = sourceFields[src].info.formula.sizeInBits;
                break;

            case VSFIELD: case AFIELD:
                sizes[index].type = V_SIZE;
                /* exported max size of variable fields can't be done */
                /* until actual size of field is known */
                /* in sizes[] will be stored the variable field ref */
                sizes[index].sizeOrRef = src;
                break;

            case FDICFIELD:
                sizes[index].sizeOrRef = sourceFields[src].info.check.sizeInBits;
                break;

            case VRFIELDSIZE:
                /* doesn't make sense export size of virtual fields */
                memset(errorNames, 0, MAX_ERR_LEN);
                strncpy(errorNames, sourceFields[src].name, MAX_ERR_LEN);
                return WRONG_SIZE_FIELD_REF;

            case AIFIELD:
                sizes[index].sizeOrRef = sourceFields[src].info.item.sizeInBits;
                break;
                
            default:
                break;
        }
    }
    /* check if it is needed to add size */
    if((!strncmp(sizeType, "sizeFromFileLine", 16) && (numAttributes == 5)) ||
            (numAttributes == 4))
    {
        unsigned int data = 0;
        if((status = GetXMLAttributeValueByName (element, "addSize", attrData, ATTR_EXPORT_SIZE)) != 0)
        {
            return status;
        }
        data = atoi(attrData);

        if((status = GetXMLAttributeValueByName(element, "unit", attrData, ATTR_EXPORT_SIZE)) != 0)
        {
            return status;
        }
        if((src_exists) && ((sourceFields[src].type == VSFIELD) ||
                (sourceFields[src].type == AFIELD)))
        {
            /* variable sizes */
            if(strncmp(attrData, "bytes", 5) == 0)
            {
                sizes[index].addSizeInBits = data * 8;
            }
            else //if(strncmp(attrData, "bits", 4) == 0)
            {
                sizes[index].addSizeInBits = data;
            }
        }
        else
        {
            /* constant sizes */
            if(strncmp(attrData, "bytes", 5) == 0)
            {
                sizes[index].sizeOrRef += data * 8;
            }
            else //if(strncmp(attrData, "bits", 4) == 0)
            {
                sizes[index].sizeOrRef += data;
            }
        }
    }
    return 0;
}

void DisplayCreateExportArrayError (int status, char * msg,
        unsigned int maxMsgSize)
{
    unsigned int len = 0;
    switch (status)
    {
        case EXPORT_FILE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "Export file not found");
            break;
            
        case EXPORT_FILE_PARSING_ERROR:
            snprintf(msg, maxMsgSize, "Export parsing error");
            break;

        case TOO_MUCH_SIZES:
            snprintf(msg, maxMsgSize, "Too much sizes (%d). "
                    "Maximum are %d", errorNumbers, MAX_SIZES);
            break;

        case WRONG_SIZE_ID:
            snprintf(msg, maxMsgSize, "Wrong size id (%d). "
                    "They must be correlative", errorNumbers);
            break;

        case WRONG_SIZE_FIELD_REF:
            snprintf(msg, maxMsgSize, "Wrong size field \"from\" (%s)",
                    errorNames);
            break;

        case FILENAME_SIZE_TOO_BIG:
            snprintf(msg, maxMsgSize, "In size %d, "
                    "export file name too big (%d). Max length is %d",
                    errorSizes, errorNumbers, MAX_EXPORT_FILENAME_LEN);
            break;

        case FILE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "In size %d, File not found (%s)",
                    errorSizes, errorNames);
            break;

        case FILE_LINE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "In size %d, File line not found (%d)",
                    errorSizes, errorNumbers);
            break;

        case TOO_MUCH_EXPORT_FIELDS:
            snprintf(msg, maxMsgSize, "Too much export fields (%d). "
                    "Maximum number are %d", errorNumbers, MAX_EXPORT_FIELDS);
            break;

        case TOO_MUCH_DICS:
            snprintf(msg, maxMsgSize, "Too much DICS (%d). Maximum are %d",
                    errorNumbers, MAX_DICS);
            break;

        case WRONG_DIC_ID:
            snprintf(msg, maxMsgSize, "Wrong DIC id (%d). "
                    "They must be correlative", errorNumbers);
            break;

        case WRONG_SOURCE_FIELD_DIC_REF:
            snprintf(msg, maxMsgSize, "In DIC %d, Wrong \"DICRef\"", errorDics);
            break;

        case MORE_DICS_THAN_FDIC_DEFINED:
            snprintf(msg, maxMsgSize, "More DICS (%d) than FDICFields",
                    errorNumbers);
            break;

        case SETTINGS_TAG_NOT_FOUND:
            snprintf(msg, maxMsgSize, "\"settings\" tag not found");
            break;

        default:
            if(errorSizes != -1)
            {
                len = snprintf(msg, maxMsgSize, "In Size %d: ", errorSizes);
            }
            else if(errorSettings != -1)
            {
                len = snprintf(msg, maxMsgSize, "In Setting %d: ", errorSettings);
            }
            else if(errorDics != -1)
            {
                len = snprintf(msg, maxMsgSize, "In DIC %d: ", errorDics);
            }
            DisplayCreateExportSettingError(status, errorNames, &msg[len],
                    maxMsgSize-len);
            break;
    }
    return;
}