/**
 * \file	XMLFilterTools.c
 * \brief	functions for parsing XML filter files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		30/01/2013 at 12:52:35
 * Company:		Space Research Group, Universidad de AlcalÃ¡.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef _WIN32
#pragma GCC diagnostic ignored "-Wformat-truncation="
#pragma GCC diagnostic ignored "-Wunused-result"
#endif

#include <inttypes.h>
#include <string.h>
#include "XMLTools.h"				/* GetXMLValueByTag, ... */
#include "XMLFormatTools.h"			/* getFormatFromXtext */
#include "../definitions.h"			/* boolVar, filter */
#include "../CommonTools/crc16.h"               /* calculate_CRC_word16, stdint.h */

#define ATTR_FILTER_SIZE MAX_STR_OPERAND_LEN*2+1

enum {
    FILTER_FILE_NOT_FOUND = -11,
    FILTER_FILE_PARSING_ERROR = -12,
    TOO_MUCH_BVARS = -13,
    TOO_MUCH_FILTERS = -14,
    WRONG_BOOLVAR_ID = -15,
    NAME_TOO_LONG = -16,
    WRONG_OPERATOR = -17,
    CONST_VALUE_TOO_LONG = -18,
    WRONG_CONSTANT_TYPE = -19,
    WRONG_FILTER_ID = -20,
    TOO_MUCH_BOOL_VAL_REFS = -21,
    SELECT_FILE_NOT_FOUND = -22,
    SELECT_FILE_TOO_BIG = -23,
    WRONG_SELECT_FILE_TYPE = -24,
    WRONG_SELECT_SIZE = -25,
    WRONG_FIELD_NAME = -26,
    WRONG_FDIC_BV = -27,
    WRONG_SELECT_LINE_0 = -28,
    SELECT_LINE_TOO_BIG = -29,
    WRONG_BVREF = -30,
    FILTER_TYPE_ERROR = -31
};

static char errorNames[MAX_ERR_LEN];
static int errorBoolVars = -1, errorFilters = -1, errorRef = -1;

static int ParseSingleBoolVar(xmlNodePtr bvNode, boolVar * pBoolVar,
	char attrData[ATTR_FILTER_SIZE], formatField * LevelFields,
	unsigned int numberOfLevelFields);
static int ParseBoolVarRefs(xmlNodePtr child, filter * pFilter,
	unsigned int numberOfBoolVars, char attrData[ATTR_FILTER_SIZE]);

int CreateFilterArray(const char * filename, const char * relativePath,
        boolVar ** boolVars, unsigned int * pNumberOfBoolVars,
	filter ** filters, unsigned int * pNumberOfFilters,
	filterTypes * typeOfFilter, formatField * LevelFields,
	unsigned int numberOfLevelFields)
{
    unsigned int numberOfChildren = 0, idx = 0, formatFile = 0;
    char attrData[ATTR_FILTER_SIZE];
    int status = 0;
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL, child = NULL;
    char fullPath[MAX_STR_LEN];
    
    errorBoolVars = -1;
    errorFilters = -1;
    errorRef = -1;
    
    /* look for file */
    if(fileExists(filename, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
        return FILTER_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return FILTER_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return FILTER_FILE_PARSING_ERROR;
    }
	
    /* get element tag to distinguish between Minterm and Maxterm filters */
    if((strncmp((char*)root->name, "MintermFilter", 13) == 0) ||
            (strncmp((char*)root->name, "GSSFilterMintermFilter", 22) == 0))
    {
        *typeOfFilter = MINTERM;
    }
    else if((strncmp((char*)root->name, "MaxtermFilter", 13) == 0) ||
            (strncmp((char*)root->name, "GSSFilterMaxtermFilter", 22) == 0))
    {
        *typeOfFilter = MAXTERM;
    }
    else
    {
        return FILTER_TYPE_ERROR;
    }

    /* NOTE: "formatFile" is redundant, checked at gss_config or tp XML files */

    /* check number of children (numberOfBoolVars + numberOfFilters) */
    GetXMLNumChildren(root, &numberOfChildren);
    /* numberOfChildren = [formatFile] + BoolVars + Filters */
    for(idx=0; idx<numberOfChildren; ++idx)
    {
        /* get element */
        if((status = GetXMLChildElementByIndex (root, idx, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        if((idx == 0) && (strncmp((char*)child->name, "formatFile", 10) == 0))
        {
            formatFile++;
            continue;
        }

        /* get element tag to distinguish between const and vble fields */
        if(strncmp((char*)child->name, "BoolVar", 7) != 0)
            break;
    }

    *pNumberOfBoolVars = idx - formatFile;
    if(*pNumberOfBoolVars > MAX_BOOL_VARS)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        errorBoolVars = *pNumberOfBoolVars;
        *pNumberOfBoolVars = 0;
        return TOO_MUCH_BVARS;
    }
    *boolVars = calloc((*pNumberOfBoolVars), sizeof(boolVar));

    *pNumberOfFilters = numberOfChildren - *pNumberOfBoolVars - formatFile;
    if(*pNumberOfFilters> MAX_FILTERS)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        errorFilters = *pNumberOfFilters;
        *pNumberOfFilters = 0;
        return TOO_MUCH_FILTERS;
    }
    *filters = calloc((*pNumberOfFilters), sizeof(filter));

    for(idx=0; idx<*pNumberOfFilters; ++idx)
    {
        filters[0][idx].boolVarRef = NULL;
    }
	
    for(idx=0; idx<*pNumberOfBoolVars+formatFile; ++idx)
    {
        /* get boolVar */
        if((status = GetXMLChildElementByIndex (root, idx, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        
        if((idx == 0) && (formatFile == 1))
        {
            continue;
        }

        /* check boolVar id */
        if((status = GetXMLAttributeValueByName(child, "id", attrData,
                ATTR_FILTER_SIZE)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        if(atoi(attrData) != (int)(idx-formatFile))
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            errorBoolVars = atoi(attrData);
            return WRONG_BOOLVAR_ID;
        }
        if((status = ParseSingleBoolVar(child, &boolVars[0][idx-formatFile],
                attrData, LevelFields, numberOfLevelFields)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            errorBoolVars = idx-formatFile;
            return status;
        }
    }

    for(idx=0; idx<*pNumberOfFilters; ++idx)
    {
        filters[0][idx].boolVarRef = NULL;
    }

    for(idx=0; idx<*pNumberOfFilters; ++idx)
    {
        /* get filter */
        if((status = GetXMLChildElementByIndex (root,
                formatFile+*pNumberOfBoolVars+idx, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }

        /* check filter id */
        if((status = GetXMLAttributeValueByName(child, "id", attrData,
                ATTR_FILTER_SIZE)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        if(atoi(attrData) != (int)idx)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            errorFilters = atoi(attrData);
            return WRONG_FILTER_ID;
        }

        if((status = ParseBoolVarRefs(child, &filters[0][idx],
                *pNumberOfBoolVars, attrData)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            errorFilters = idx;
            return status;
        }
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}

void DisplayCreateFilterArrayError (int status, char * msg,
        unsigned int maxMsgSize)
{
    unsigned int len = 0;
    switch(status)
    {
        case FILTER_FILE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "Filter file not found");
            break;
            
        case FILTER_FILE_PARSING_ERROR:
            snprintf(msg, maxMsgSize, "Filter file parsing error");
            break;
            
        case FILTER_TYPE_ERROR:
            snprintf(msg, maxMsgSize, "Wrong filter tag. It must be "
                    "\"MintermFilter\" or \"MaxtermFilter\"");
            break;

        case TOO_MUCH_BVARS:
            snprintf(msg, maxMsgSize, "Too much boolVars (%d). "
                    "Maximum are %d", errorBoolVars, MAX_BOOL_VARS);
            break;

        case TOO_MUCH_FILTERS:
            snprintf(msg, maxMsgSize, "Too much filters (%d). "
                    "Maximum are %d", errorFilters, MAX_FILTERS);
            break;

        case WRONG_BOOLVAR_ID:
            snprintf(msg, maxMsgSize, "Wrong boolVar id (%d). "
                    "They must be correlative", errorBoolVars);
            break;

        case NAME_TOO_LONG:
            snprintf(msg, maxMsgSize, "In BoolVar %d, "
                    "\"FieldRef\" name too long (%d). Maximum length is %d",
                    errorBoolVars, errorRef, MAX_FIELD_NAME_LEN);
            break; 

        case WRONG_OPERATOR:
            snprintf(msg, maxMsgSize, "In BoolVar %d, "
                    "wrong \"Op\"", errorBoolVars);
            break;

        case CONST_VALUE_TOO_LONG:
            snprintf(msg, maxMsgSize, "In BoolVar %d, "
                    "constant too long (%d)", errorBoolVars, errorRef);
            break;

        case WRONG_CONSTANT_TYPE:
            snprintf(msg, maxMsgSize, "In BoolVar %d, "
                    "wrong \"constantType\"", errorBoolVars);
            break;

        case WRONG_FILTER_ID:
            snprintf(msg, maxMsgSize, "Wrong filter id (%d). "
                    "They must be correlative", errorFilters);
            break;

        case TOO_MUCH_BOOL_VAL_REFS:
            snprintf(msg, maxMsgSize, "In filter %d, "
                    "Too much boolVarRefs (%d). Maximum are %d",
                    errorFilters, errorRef, MAX_BOOL_VARS);
            break;

        case SELECT_FILE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "In BoolVar %d Select, "
                    "\"fromFile\" not found", errorBoolVars);
            break;

        case SELECT_FILE_TOO_BIG:
            snprintf(msg, maxMsgSize, "In BoolVar %d Select, "
                    "\"fromFile\" too big. Max size is %d",
                    errorBoolVars, MAX_STR_OPERAND_LEN);
            break;

        case WRONG_SELECT_FILE_TYPE:
            snprintf(msg, maxMsgSize, "In BoolVar %d Select, "
                    "wrong \"type\"", errorBoolVars);
            break;

        case WRONG_SELECT_SIZE:
            snprintf(msg, maxMsgSize, "In BoolVar %d Select, "
                    "wrong \"size\"", errorBoolVars);
            break;

        case WRONG_FIELD_NAME:
            snprintf(msg, maxMsgSize, "In BoolVar %d, "
                    "wrong FieldRef \"%s\"", errorBoolVars, errorNames);
            break;

        case WRONG_FDIC_BV:
            snprintf(msg, maxMsgSize, "In BoolVar %d, "
                    "FieldRef \"%s\" is not a \"FDICField\"",
                    errorBoolVars, errorNames);
            break;

        case WRONG_SELECT_LINE_0:
            snprintf(msg, maxMsgSize, "In BoolVar %d \"SelectLine\", "
                    "\"line\" can't be \"0\"", errorBoolVars);
            break;

        case SELECT_LINE_TOO_BIG:
            snprintf(msg, maxMsgSize, "In BoolVar %d \"SelectLine\", "
                    "\"line\" doesn't exist at file", errorBoolVars);
            break;

        case WRONG_BVREF:
            snprintf(msg, maxMsgSize, "In filter %d, "
                    "BoolVarRef idRef \"%d\" doesn't exist",
                    errorFilters, errorRef);
            break;            

        default:
            if(errorBoolVars != -1)
            {
                len = snprintf(msg, maxMsgSize, "In boolVar %d: ", errorBoolVars);
            }
            else if(errorFilters != -1)
            {
                len = snprintf(msg, maxMsgSize, "In filter %d: ", errorFilters);
            }
            XMLerrorInfo(status, &msg[len], maxMsgSize-len);
            break;
    }
    return;
}

static int ParseSingleBoolVar(xmlNodePtr bvNode, boolVar * pBoolVar,
	char attrData[ATTR_FILTER_SIZE], formatField * LevelFields,
	unsigned int numberOfLevelFields)
{
    int status = 0;
    unsigned int fileOffset = 0, fileSize = 0, fileLen, maxLen = 0;
    unsigned int idxField = 0;
    unsigned int numAttributes = 0;
    char dataType[28], fieldRefName[12];
    unsigned char maskIsDefined = 0;
    xmlNodePtr child = NULL;
    xmlAttrPtr attrib = NULL;

    char filterType[28];
    unsigned int isFromXMI = 0;
    if((status = SearchXMLAttributeValueByName(bvNode, "type", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if(attrData[0] != '\0')
    {
        isFromXMI = 1;
        strncpy(filterType, &attrData[13], 28);//remove initial "gss:GSSFilter"
    }
    else
    {
        strncpy(filterType, (char*)bvNode->name, 28);
    }
    /* get element tag to distinguish between const and vble fields */
    if(strncmp(filterType, "BoolVarFromArrayItem", 20) == 0)
    {
        strncpy(fieldRefName, "AIFieldRef", 11);
        pBoolVar->type = FROM_ARRAY;

        /* get index of AIFieldRef */
        if((status = GetXMLChildAttributeValueByName(bvNode, "AIFieldRef",
                "index", attrData, ATTR_FILTER_SIZE)) != 0)
        {
            return status;
        }
        pBoolVar->indexGroup = atoi(attrData);
    }
    else if(strncmp(filterType, "BoolVarFromGroupedArrayItem", 27) == 0)
    {
        strncpy(fieldRefName, "AIFieldRefs", 12);
        pBoolVar->type = FROM_GROUP_ARRAY;

        /* get group of BoolVarFromGroupedArrayItem */
        if((status = GetXMLAttributeValueByName(bvNode, "group", attrData,
                ATTR_FILTER_SIZE)) != 0)
        {
            return status;
        }
        pBoolVar->indexGroup = atoi(attrData);
    }
    else if(strncmp(filterType, "BoolVarFDIC", 12) == 0)
    {
        strncpy(fieldRefName, "FieldRef", 9);
        pBoolVar->type = FDIC_BV;
        pBoolVar->indexGroup = -1;
    }
    else //if(strncmp(filterType, "BoolVar", 8) == 0)
    {
        strncpy(fieldRefName, "FieldRef", 9);
        pBoolVar->type = NORMAL;
        pBoolVar->indexGroup = -1;
    }
    if(isFromXMI)
    {
        strncpy(fieldRefName, "FieldRef", 9);
    }

    /* get FieldRef of BoolVar */
    if((status = GetXMLChildElementByTag(bvNode, fieldRefName, &child)) != 0)
    {
        return status;
    }
    if((status = GetXMLAttributeNameByIndex(child, 0, attrData,
            ATTR_FILTER_SIZE)) != 0)
    {
        return status;
    }
    
    /* get FieldRef of BoolVar */
    if(strncmp(attrData, "name", 4) == 0)
    {
        if((status = GetXMLAttributeLengthByName(child, "name", &fileLen)) != 0)
        {
            return status;
        }
        if(fileLen > MAX_FIELD_NAME_LEN)
        {
            errorRef = fileLen;
            return NAME_TOO_LONG;
        }
        if((status = GetXMLAttributeValueByName(child, "name", attrData,
                ATTR_FILTER_SIZE)) != 0)
        {
            return status;
        }
    
        for(idxField=0; idxField<numberOfLevelFields; ++idxField)
        {
            if(strncmp(LevelFields[idxField].name, attrData, MAX_FIELD_NAME_LEN) == 0)
            {
                break;
            }
        }
        if((idxField == numberOfLevelFields) && (idxField != 0))
        {
            memset(errorNames, 0, MAX_ERR_LEN);
            strncpy(errorNames, attrData, MAX_ERR_LEN);
            return WRONG_FIELD_NAME;
        }
    }
    else
    {
        if((status = GetXMLAttributeLengthByName(child, "href", &fileLen)) != 0)
        {
            return status;
        }
        if((status = GetXMLAttributeValueByName(child, "href", attrData,
                ATTR_FILTER_SIZE)) != 0)
        {
            return status;
        }
        if((status = getFormatFromXtext(attrData, fileLen, LevelFields, 
                numberOfLevelFields, &idxField)) != 0)
        {
            memset(errorNames, 0, MAX_ERR_LEN);
            strncpy(errorNames, attrData, MAX_ERR_LEN);
            return WRONG_FIELD_NAME;
        }
    }
    snprintf(pBoolVar->field, MAX_FIELD_NAME_LEN, "%s", attrData);
	
    /* get Op of BoolVar */
    if((status = GetXMLChildAttributeValueByName(bvNode, "Op", "type",
            attrData, ATTR_FILTER_SIZE)) != 0)
    {
        return status;
    }
    if(strncmp(attrData, "equal", 6) == 0)
    {
        pBoolVar->operation = EQUAL;
    }
    else if(strncmp(attrData, "different", 10) == 0)
    {
        pBoolVar->operation = DIFFERENT;
    }
    else if(strncmp(attrData, "bigger_than", 12) == 0)
    {
        pBoolVar->operation = BIGGER_THAN;
    }
    else if(strncmp(attrData, "smaller_than", 13) == 0)
    {
        pBoolVar->operation = SMALLER_THAN;
    }
    else if(strncmp(attrData, "bigger_or_equal", 16) == 0)
    {
        pBoolVar->operation = BIGGER_OR_EQUAL;
    }
    else if(strncmp(attrData, "smaller_or_equal", 17) == 0)
    {
        pBoolVar->operation = SMALLER_OR_EQUAL;
    }
    else
    {
        return WRONG_OPERATOR;
    }
    
    if(pBoolVar->type == FDIC_BV)
    {	
        unsigned char DICs = 0;
        if(LevelFields[idxField].type != FDICFIELD)
        {
            memset(errorNames, 0, MAX_ERR_LEN);
            strncpy(errorNames, attrData, MAX_ERR_LEN);
            return WRONG_FDIC_BV;
        }

        pBoolVar->indexGroup = idxField;
        for(idxField=0; idxField<numberOfLevelFields; ++idxField)
        {
            if((int)idxField == pBoolVar->indexGroup)
                break;
            if(LevelFields[idxField].type == FDICFIELD)
                DICs++;
        }
        pBoolVar->indexGroup = DICs;
        pBoolVar->number = 0;
        memset(pBoolVar->string, 0, MAX_STR_OPERAND_LEN);
        pBoolVar->valueType = HEX_BASE;
        pBoolVar->mask = 0xFFFFFFFFFFFFFFFF;
        pBoolVar->result = 0;
        return 0;
    }
    
    /* get constantType of filter */
    if((status = GetXMLAttributeValueByName (bvNode, "constantType",
            dataType, 28)) != 0)
    {
        return status;
    }
    if(strncmp(dataType, "decimal", 8) == 0)
    {
        pBoolVar->valueType = DEC_BASE;
        maxLen = 19; //2^64 = 18446744073709551616
    }
    else if(strncmp(dataType, "hex", 4) == 0)
    {
        pBoolVar->valueType = HEX_BASE;
        maxLen = 2 + (MAX_NUM_OPERAND_LEN*2) ; //0x + 8*2 hex bytes
    }
    else if(strncmp(dataType, "binary", 7) == 0)
    {
        pBoolVar->valueType = BIN_BASE;
        maxLen = MAX_NUM_OPERAND_LEN*8;  //8*8 bits
    }
    else if(strncmp(dataType, "char", 5) == 0)
    {
        pBoolVar->valueType = STRING_CHAR;
        maxLen = 1;
    }
    else if(strncmp(dataType, "string", 7) == 0)
    {
        pBoolVar->valueType = STRING;
        maxLen = MAX_STR_OPERAND_LEN;
    }
    else
    {
        return WRONG_CONSTANT_TYPE;
    }
    maxLen++;// plus '\0'
    
    /* get operand tag value of BoolVar */
    if((status = GetXMLChildElementByIndex(bvNode, 2, &child)) != 0)
    {
        return status;
    }
    if((status = GetXMLNumAttributes(child, (unsigned int *)&numAttributes)) != 0)
    {
        return status;
    }
    pBoolVar->mask = 0xFFFFFFFFFFFFFFFF;

    /* get type of operand (constant or select) */
    char constantSelectTag[20];
    if(strncmp((char*)child->name, "value", 5) == 0)
    {
        numAttributes--;
        if((status = GetXMLAttributeValueByName(child, "type", attrData,
                ATTR_FILTER_SIZE)) != 0)
        {
            return status;
        }
        //Remove initial "gss:GSSFilter"
        strncpy(constantSelectTag, &attrData[13], strlen(attrData)-13);
    }
    else
    {
        strncpy(constantSelectTag, (const char*)child->name, strlen((char*)child->name));
    }
    if(strncmp(constantSelectTag, "Constant", 8) == 0)
    {
        if((status = GetXMLAttributeLengthByName(child, "value", &fileLen)) != 0)
        {
            return status;
        }

        if((status = GetXMLAttributeValueByName(child, "value", attrData,
                ATTR_FILTER_SIZE)) != 0)
        {
            return status;
        }
        if(numAttributes == 2)
        {
            maskIsDefined = 1;
        }
    }
    else if(strncmp(constantSelectTag, "Select", 6) == 0)
    /* both "Select" and "SelectLine" */
    {
        unsigned int idx=2;
        for(idx=2; idx<numAttributes; ++idx)
        {   
            if((status = GetXMLAttributeByIndex(child, idx, &attrib)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeValue (attrib, attrData,
                    ATTR_FILTER_SIZE)) != 0)
            {
                return status;
            }
            if((status = GetXMLAttributeName (attrib, fieldRefName, 12)) != 0)
            {
                return status;
            }
            if((strncmp(fieldRefName, "offset", 6) == 0) ||
                    (strncmp(fieldRefName, "leftTrim", 8) == 0))
            {
                fileOffset = (unsigned int)atoi(attrData);
            }
            else if((strncmp(fieldRefName, "size", 4) == 0) ||
                    (strncmp(fieldRefName, "rightTrim", 9) == 0))
            {
                fileSize = (unsigned int)atoi(attrData);
            }
            else if(strncmp(fieldRefName, "mask", 4) == 0)
            {
                maskIsDefined = 1;
            }
        }

        if((status = GetXMLAttributeValueByName(child, "fromFile", attrData,
                ATTR_FILTER_SIZE)) != 0)
        {
            return status;
        }

        FILE * fp = fopen(attrData, "r");
        if(fp==NULL)
        {
            return SELECT_FILE_NOT_FOUND;
        }

        if(strncmp((char*)child->name, "SelectLine", 10) == 0)
        {
            unsigned int fileLine, numberOfLines = 0;
            int readFileLen;
            if((status = GetXMLAttributeValueByName(child, "line", dataType,
                    28)) != 0)
            {
                return status;
            } 
            fileLine = atoi(dataType);
            if(fileLine < 1)
            {
                return WRONG_SELECT_LINE_0;
            }
            /* Get line from file */
            for(numberOfLines=0; numberOfLines<fileLine; ++numberOfLines)
            {
                if(fgets(attrData, MAX_STR_OPERAND_LEN*2, fp) == NULL)
                {
                    fclose(fp);
                    return SELECT_LINE_TOO_BIG;
                }
            }
            fclose(fp);
            readFileLen = strlen(attrData)/2 - fileSize - fileOffset;
            if(readFileLen < 0)
            {
                return WRONG_SELECT_SIZE;
            }
            fileLen = readFileLen;
            if(attrData[strlen(attrData)-2] == '\r')
            {
                fileLen--;
            }
            /* fileSize is rightTrim, fileOffset is leftTrim */         
            if(fileOffset != 0)
            {
                for(idx=0; idx<2*fileLen; ++idx)
                {
                    attrData[idx] = attrData[idx+2*fileOffset];
                }
            }
            attrData[2*fileLen] = '\0';
        }
        else
        {
            /* look for file size */
            fseek(fp, 0, SEEK_END);  /* seek to end of file */
            fileLen = ftell(fp) / 2; //hex
            fclose(fp);

            if((status = GetXMLAttributeValueByName(child, "type", dataType, 28)) != 0)
            {
                return status;
            } 
            if(strncmp(dataType, "data", 4) == 0)
            {
                if(fileLen > MAX_STR_OPERAND_LEN)
                {
                    return SELECT_FILE_TOO_BIG;
                }
                /* file must be always opened in HEX mode */
                fp = fopen(attrData, "r");
                fread(attrData, 2, fileLen, fp);
                attrData[2*fileLen] = '\0'; 
                fclose(fp);

                if(((fileOffset+fileSize) > fileLen) || (fileSize > fileLen))
                {
                    return WRONG_SELECT_SIZE;
                }
                if(fileSize == 0)
                {
                    fileSize = fileLen-fileOffset;
                }
                if(fileOffset != 0)
                {
                    for(idx=0; idx<2*fileSize; ++idx)
                    {
                        attrData[idx] = attrData[idx+2*fileOffset];
                    }
                }
                attrData[2*fileSize] = '\0';
                fileLen = fileSize;
            }
            else if(strncmp(dataType, "size", 4) == 0)
            {
                memset(attrData, 0, MAX_STR_OPERAND_LEN);
                snprintf(attrData, ATTR_FILTER_SIZE, "%d", fileLen);
                fileLen = sizeof(unsigned int);
            }
            else if(strncmp(dataType, "crc", 1) == 0)
            {
                unsigned int aux = 0, CRC = 0xFFFF;
                fp = fopen(attrData, "r");

                while(fileLen > MAX_STR_OPERAND_LEN)
                {
                    memset(pBoolVar->string, 0, MAX_STR_OPERAND_LEN);
                    fread(attrData, 2, MAX_STR_OPERAND_LEN, fp);
                    attrData[2*MAX_STR_OPERAND_LEN] = '\0'; 
                    for(idx=0; idx<MAX_STR_OPERAND_LEN; ++idx)
                    {
                        sscanf(&attrData[2*idx], "%02X", &aux);
                        pBoolVar->string[idx] = (unsigned char)aux;
                    }
                    CRC = set_seed_and_calculate_CRC_word16(MAX_STR_OPERAND_LEN,
                            pBoolVar->string, CRC);
                    fileLen -= MAX_STR_OPERAND_LEN;
                }

                memset(pBoolVar->string, 0, MAX_STR_OPERAND_LEN);
                fread(attrData, 2, fileLen, fp);
                attrData[2*fileLen] = '\0'; 
                for(idx=0; idx<fileLen; ++idx)
                {
                    sscanf(&attrData[2*idx], "%02X", &aux);
                    pBoolVar->string[idx] = (unsigned char)aux;
                }
                fclose(fp);
                CRC = set_seed_and_calculate_CRC_word16(fileLen,
                        pBoolVar->string, CRC);
                memset(pBoolVar->string, 0, MAX_STR_OPERAND_LEN);
                snprintf(attrData, ATTR_FILTER_SIZE, "%04X%c", CRC, '\0');
                fileLen = 2;
            }
            else
            {
                return WRONG_SELECT_FILE_TYPE;
            }
        }
    }
    if(fileLen > maxLen)
    {
        errorRef = fileLen;
        return CONST_VALUE_TOO_LONG;
    }
	
    /* assign attrData previously read */
    unsigned int idxLen = 0; 
    switch(pBoolVar->valueType)
    {
        case DEC_BASE:
            sscanf(attrData, "%"PRIu64"", &pBoolVar->number);
            break;

        case HEX_BASE:
            if((attrData[0] == '0') && (attrData[0] == 'x'))
                sscanf(&attrData[2], "%"PRIx64"", &pBoolVar->number);
            else
                sscanf(attrData, "%"PRIx64"", &pBoolVar->number);
            break;

        case BIN_BASE:
            pBoolVar->number = 0;
            for(idxLen=0; idxLen<fileLen; ++idxLen)
            {
                /* convert character for binary */
                attrData[idxLen] -= 48;
                if ((attrData[idxLen] != 0) && (attrData[idxLen] != 1))
                {
                    /* invalid character */
                    break;
                }
                /* multiply by the base */
                pBoolVar->number *= 2;
                /* add current value */
                pBoolVar->number += attrData[idxLen];
            }
            break;

        case STRING_CHAR:
            pBoolVar->string[0] = attrData[0];
            break;

        case STRING:
            pBoolVar->mask = fileLen;
            memset(pBoolVar->string, 0, MAX_STR_OPERAND_LEN);
            for(idxLen=0; idxLen<fileLen; ++idxLen)
            {
                sscanf(&attrData[2*idxLen], "%02X",
                        (int *)&pBoolVar->string[idxLen]);
            }
            break;
    }

    if((maskIsDefined) && (pBoolVar->valueType == HEX_BASE))
    {
        /* get mask (only for HEX values) */
        if((status = GetXMLAttributeValueByName(child, "mask", attrData, ATTR_FILTER_SIZE)) != 0)
        {
            return status;
        }
        sscanf(attrData, "%"PRIx64"", &pBoolVar->mask);
    }
    pBoolVar->result = 0;
    return 0;
}

static int ParseBoolVarRefs(xmlNodePtr child, filter * pFilter,
	unsigned int numberOfBoolVars, char attrData[ATTR_FILTER_SIZE])
{
    int status = 0;
    unsigned int currentBVRef = 0;
    xmlNodePtr bVr = NULL;

    /* check number of boolVarRefs */
    GetXMLNumChildren(child, &pFilter->numberOfBoolVarRefs);
    if(pFilter->numberOfBoolVarRefs > MAX_BOOL_VARS)
    {
        errorRef = pFilter->numberOfBoolVarRefs;
        pFilter->numberOfBoolVarRefs = 0;
        return TOO_MUCH_BOOL_VAL_REFS;
    }
    pFilter->boolVarRef = calloc(pFilter->numberOfBoolVarRefs, sizeof(unsigned int));

    for(currentBVRef=0; currentBVRef<pFilter->numberOfBoolVarRefs; ++currentBVRef)
    {
        /* get next boolVarRef */
        if((status = GetXMLChildElementByIndex (child, currentBVRef, &bVr)) != 0)
        {
            return status;
        }

        /* get boolVarRef idRef */
        if((status = GetXMLAttributeValueByName(bVr, "idRef", attrData, ATTR_FILTER_SIZE)) != 0)
        {
            return status;
        }
        pFilter->boolVarRef[currentBVRef] = atoi(attrData);
        if(pFilter->boolVarRef[currentBVRef] >= numberOfBoolVars)
        {
            errorRef = currentBVRef;
            return WRONG_BVREF;
        }
    }
    pFilter->result = 0;
    return 0;
}