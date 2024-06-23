/**
 * \file	XMLFormatTools.c
 * \brief	functions for parsing XML format files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		18/02/2013 at 16:05:55
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c)2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdlib.h>
#include <string.h>
#include "XMLTools.h"					/* GetXMLValueByTag, ... */
#include "../definitions.h"				/* MAX_MSG_SIZE */

enum {
    FORMAT_FILE_NOT_FOUND = -11,
    FORMAT_FILE_PARSING_ERROR = -12,
    TOO_MUCH_FIELDS = -13,
    WRONG_FID = -14,
    TOO_MUCH_DIC_FIELDS = -15,
    FIELD_NAME_TOO_LONG = -16,
    WRONG_PACKET_TAG = -17,
    VBLE_SIZE_WRONG_UNIT = -18,
    VBLE_SIZE_WRONG_POWER = -19,
    CRC_FIELD_REF_NO_FULL_BYTE = -20,
    FORMULA_TOO_LONG = -21
};

static int errorNumbers;

typedef enum {
    NO_OP,
    ADD,
    SUB,
    MUL,
    DIV
}operation;

/**
 * \brief Function to get the value of a variable size field in bits
 * \param element		XML handle where is located the variable size field
 * \param[out] value	Variable to save the variable size field into (in bits)
 */
static int GetVariableSize(xmlNodePtr element, variableField * value,
	char attrData[ATTR_SIZE]);
static int GetFormulaDouble(xmlNodePtr element, const char attrName[],
	double * value, char attrData[ATTR_SIZE]);
static int calculateFormula(const char * formula, double * value,
	unsigned int * retPos);

/**
 * \brief Function to parse the format of every field
 */
static int ParseFormatField(xmlNodePtr element, formatField ** fields,
	unsigned int *** crcFieldRefs, unsigned int ** numberOfcrcFields,
	char attrData[ATTR_SIZE], int idx);
/**
 * \brief Function to parse sorted fields for DICs
 */
static int ParseSortedFields(xmlNodePtr FDICelement,
	unsigned int *** crcFieldRefs, unsigned int ** numberOfcrcFields,
	char attrData[ATTR_SIZE], int curDIC, formatField * fields);

int CreateFormatArray(const char * filename, const char * relativePath,
        formatField ** fields,
	unsigned int * numberOfFields, unsigned int * numberOfFDICFields,
	unsigned int *** crcFieldRefs, unsigned int ** numberOfcrcFields)
{
    char attrData[ATTR_SIZE];
    int status = 0;
    unsigned int idx = 0;
    unsigned int * fieldIdx = NULL;
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL, child = NULL;
    char fullPath[MAX_STR_LEN];
    
    /* look for file */
    if(fileExists(filename, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
        return FORMAT_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return FORMAT_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return FORMAT_FILE_PARSING_ERROR;
    }
	
    /* NOTE: "protocol" and "type" are redudant, as they are assigned at gss_config or tp XML files */

    /* check number of fields */
    GetXMLNumChildren(root, numberOfFields);
    if(*numberOfFields > MAX_FIELDS)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        errorNumbers = *numberOfFields;
        *numberOfFields = 0;
        return TOO_MUCH_FIELDS;
    }
    *fields = calloc((*numberOfFields), sizeof(formatField));
    *numberOfFDICFields = 0;
    
    fieldIdx = calloc((*numberOfFields), sizeof(unsigned int));
    /* first look for FDICs and reorder idx */
    for(idx=0; idx<*numberOfFields; ++idx)
    {
        /* get next Field */
        if((status = GetXMLChildElementByIndex (root, idx, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        /* get next element tag to look for FDICs */
        char fieldType[15];
        if(strncmp((char*)child->name, "Field", 5) == 0)
        {
            if((status = GetXMLAttributeValueByName(child, "type", attrData,
                    ATTR_SIZE)) != 0)
            {
                return status;
            }
            strncpy(fieldType, &attrData[13], 15);//remove initial "gss:GSSFormat"
        }
        else
        {
            strncpy(fieldType, (char*)child->name, 15);
        }
        if(strncmp(fieldType, "FDICField", 9) == 0)
        {
            *numberOfFDICFields = *numberOfFDICFields+1;
        }
        
        /* reorder idx */
        if((status = GetXMLAttributeValueByName(child, "fid", attrData, ATTR_SIZE)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
        fieldIdx[idx] = atoi(attrData);
    }
    
    *numberOfcrcFields = calloc((*numberOfFDICFields), sizeof(unsigned int));
    *crcFieldRefs = calloc((*numberOfFDICFields), sizeof(unsigned int));

    /* now we process each field */
    for(idx=0; idx<*numberOfFields; ++idx)
    {
        /* get next Field */
        if((status = GetXMLChildElementByIndex (root, idx, &child)) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }

        /* parse every Field */
        if((status = ParseFormatField(child, fields, crcFieldRefs,
                numberOfcrcFields, attrData, fieldIdx[idx])) != 0)
        {
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return status;
        }
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}

static int ParseFormatField(xmlNodePtr element, formatField ** fields,
	unsigned int *** crcFieldRefs, unsigned int ** numberOfcrcFields,
	char attrData[ATTR_SIZE], int idx)
{
    int status, curDIC = 0;
    unsigned int nameLen = 0, numAttributes = 0;

    /* get fid (it must be array index)*/
    if((status = GetXMLAttributeValueByName (element, "fid", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if (atoi(attrData) != idx)
    {
        errorNumbers = atoi(attrData);
        return WRONG_FID;
    }

    /* get pfid */
    if((status = GetXMLAttributeValueByName (element, "pfid", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    fields[0][idx].pfid = atoi(attrData);
    fields[0][idx].exported = 1;//initialize for rx ports

    /* get name (first checking name length)*/
    if((status = GetXMLAttributeLengthByName(element, "name", &nameLen)) != 0)
    {
        return status;
    }
    if(nameLen > (MAX_FIELD_NAME_LEN-1))
    {
        errorNumbers = nameLen;
        return FIELD_NAME_TOO_LONG;
    }
    if((status = GetXMLAttributeValueByName (element, "name", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    strncpy(fields[0][idx].name, attrData, MAX_FIELD_NAME_LEN);
    /* get number of attributes and store in 'numAttributes' */
    if((status = GetXMLNumAttributes(element, &numAttributes)) != 0)
    {
        return status;
    }
    
    char fieldType[15];
    if(strncmp((char*)element->name, "Field", 5) == 0)
    {
        numAttributes--;
        if((status = GetXMLAttributeValueByName(element, "type", attrData,
                ATTR_SIZE)) != 0)
        {
            return status;
        }
        strncpy(fieldType, &attrData[13], 15);//remove initial "gss:GSSFormat"
    }
    else
    {
        strncpy(fieldType, (char*)element->name, 15);
    }
    if(numAttributes == 7)
    {
        char descriptionAttr[12] = "descr";
        /* get descr (first checking name length)*/
        if((status = GetXMLAttributeLengthByName(element, "descr", &nameLen)) != 0)
        {
            if((status = GetXMLAttributeLengthByName(element, "description", &nameLen)) != 0)
            {
                return status;
            }
            strncpy(descriptionAttr, "description", 12);
        }
        if(nameLen > (MAX_FIELD_NAME_LEN-1))
        {
            errorNumbers = nameLen;
            return FIELD_NAME_TOO_LONG;
        }
        if((status = GetXMLAttributeValueByName (element, descriptionAttr, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        strncpy(fields[0][idx].descr, attrData, MAX_FIELD_NAME_LEN);
    }
    else
    {
        strncpy(fields[0][idx].descr, fields[0][idx].name, MAX_FIELD_NAME_LEN);
    }

    /* TODO: get and use "type", "byteOrder" and "firstBit" */

    /* get next element tag to distinguish between const and vble fields */
    if(strcmp(fieldType, "CSField") == 0)
    {
        /* get size */
        if((status = GetFieldInBits(element, "size",
                (int *)&fields[0][idx].info.sizeInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }

        /* get globalOffset */
        if((status = GetFieldInBits(element, "globalOffset",
                &fields[0][idx].offsetInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        fields[0][idx].type = CSFIELD;
        fields[0][idx].totalSizeInBits = fields[0][idx].info.sizeInBits;
    }
    else if(strcmp(fieldType, "CSFormulaField") == 0)
    {
        /* get size */
        if((status = GetFieldInBits(element, "size",
                (int *)&fields[0][idx].info.formula.sizeInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        /* get globalOffset */
        if((status = GetFieldInBits(element, "globalOffset",
                &fields[0][idx].offsetInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        fields[0][idx].type = CSFORMULAFIELD;
        fields[0][idx].totalSizeInBits = fields[0][idx].info.formula.sizeInBits;
        /* get formula slope */
        if((status = GetFormulaDouble(element, "slope",
                &fields[0][idx].info.formula.slope, attrData)) != 0)
        {
            return status;
        }
        /* get formula intercept */
        if((status = GetFormulaDouble(element, "intercept",
                &fields[0][idx].info.formula.intercept, attrData)) != 0)
        {
            return status;
        }
    }
    else if(strcmp(fieldType, "VSField") == 0)
    {
        /* get constSize */
        if((status = GetFieldInBits(element, "constSize",
                &fields[0][idx].info.variable.constSizeInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }

        /* get variableSize */
        if((status = GetVariableSize(element, &fields[0][idx].info.variable,
                attrData)) != 0)
        {
            return status;
        }

        /* get maxSize */
        if((status = GetFieldInBits(element, "maxSize",
                (int *)&fields[0][idx].info.variable.maxSizeInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }

        /* get globalOffset */
        if((status = GetFieldInBits(element, "globalOffset",
                &fields[0][idx].offsetInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        fields[0][idx].type = VSFIELD;
        fields[0][idx].totalSizeInBits = 0;/* invalid */
    }
    else if(strcmp(fieldType, "FDICField") == 0)
    {
        xmlNodePtr FDICelement = NULL;
        fields[0][idx].offsetInBits = -1;/* invalid */;

        /* get checkType */
        if((status = GetXMLAttributeValueByName (element, "checkType", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(!strncmp(attrData, "crc16", 5))
            fields[0][idx].info.check.type = CRC_16;
        else //if(!strncmp(attrData, "checksum16", 10))
            fields[0][idx].info.check.type = CHECKSUM_16;

        /* get size */
        if((status = GetFieldInBits(element, "size",
                (int *)&fields[0][idx].info.check.sizeInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }

        /* get field from with float offset */
        if((status = GetXMLChildAttributeValueByName (element, "floatingOffset",
                "fidRef", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        fields[0][idx].info.check.fieldRef = atoi(attrData);

        /* get field references */
        if((status = GetXMLChildElementByTag (element,
                "sortedFieldsToCheck", &FDICelement)) != 0)
        {
            return status;
        }

        /* parse every Field */
        if((status = ParseSortedFields(FDICelement, crcFieldRefs,
                numberOfcrcFields, attrData, curDIC, fields[0])) != 0)
        {
            return status;
        }

        fields[0][idx].type = FDICFIELD;
        /* reserve an offset size */
        fields[0][idx].totalSizeInBits = fields[0][idx].info.check.sizeInBits;
        curDIC++;
    }
    else if(strcmp(fieldType, "VRFieldSize") == 0)
    {
        fields[0][idx].offsetInBits = -1; /* invalid, not used */ 
        fields[0][idx].info.virtualSizeValue = 0; /* invalid */
        fields[0][idx].type = VRFIELDSIZE;
        fields[0][idx].totalSizeInBits = 0;
    }
    else if(strcmp(fieldType, "AField") == 0)
    {
        /* get arrayDimension fidRef */
        if((status = GetXMLChildAttributeValueByName (element, "arrayDimension",
                "fidRef", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        fields[0][idx].info.array.fieldRef = atoi(attrData);

        /* get arrayDimension maxItems */
        if((status = GetXMLChildAttributeValueByName (element, "arrayDimension",
                "maxItems", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        fields[0][idx].info.array.maxItems = atoi(attrData);

        /* get size of one item */
        if((status = GetFieldInBits(element, "size",
                (int *)&fields[0][idx].info.array.sizeOfItem, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }

        /* get globalOffset */
        if((status = GetFieldInBits(element, "globalOffset",
                &fields[0][idx].offsetInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }

        fields[0][idx].type = AFIELD;
        fields[0][idx].totalSizeInBits = 0;/* invalid */
    }
    else if(strcmp(fieldType, "AIField") == 0)
    {
        /* get array field in which item is located */
        if((status = GetXMLChildAttributeValueByName (element, "arrayRef",
                "fidRef", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        fields[0][idx].info.item.arrayRef = atoi(attrData);

        /* get size */
        if((status = GetFieldInBits(element, "size",
                (int *)&fields[0][idx].info.item.sizeInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }

        /* get localOffset */
        if((status = GetFieldInBits(element, "localOffset",
                &fields[0][idx].offsetInBits, attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        fields[0][idx].type = AIFIELD;
        fields[0][idx].totalSizeInBits = fields[0][idx].info.item.sizeInBits;
    }
    else
    {
        return WRONG_PACKET_TAG;
    }
    return 0;
}

static int ParseSortedFields(xmlNodePtr FDICelement,
	unsigned int *** crcFieldRefs, unsigned int ** numberOfcrcFields,
	char attrData[ATTR_SIZE], int curDIC, formatField * fields)
{
    int status;
    unsigned int field = 0;
    xmlNodePtr sortedFields = NULL;

    /* get number of sorted fields */
    GetXMLNumChildren(FDICelement, &numberOfcrcFields[0][curDIC]);
    if(numberOfcrcFields[0][curDIC] > MAX_FIELDS)
    {
        errorNumbers = numberOfcrcFields[0][curDIC];
        numberOfcrcFields[0][curDIC] = 0;
        return TOO_MUCH_DIC_FIELDS;
    }
    crcFieldRefs[0][curDIC] = calloc(numberOfcrcFields[0][curDIC], sizeof(unsigned int));

    for(field=0; field<numberOfcrcFields[0][curDIC]; ++field)
    {
        if((status = GetXMLChildElementByIndex (FDICelement, field,
                &sortedFields)) != 0)
        {
            return status;
        }

        if((status = GetXMLAttributeValueByName (sortedFields, "fidRef",
                attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        crcFieldRefs[0][curDIC][field] = atoi(attrData);
        if(((fields[crcFieldRefs[0][curDIC][field]].totalSizeInBits % 8) ||
                (fields[crcFieldRefs[0][curDIC][field]].totalSizeInBits < 8)) &&
                (fields[crcFieldRefs[0][curDIC][field]].totalSizeInBits != 0))
        {
            errorNumbers = crcFieldRefs[0][curDIC][field];
            return CRC_FIELD_REF_NO_FULL_BYTE;
        }
    }
    return 0;
}

void DisplayCreateFormatArrayError(int status, char * msg,
        unsigned int maxMsgSize)
{
    switch (status)
    {
        case FORMAT_FILE_NOT_FOUND:
            snprintf(msg, maxMsgSize, "Format file not found");
            break;
            
        case FORMAT_FILE_PARSING_ERROR:
            snprintf(msg, maxMsgSize, "Format file parsing error");
            break;

        case TOO_MUCH_FIELDS:
            snprintf(msg, maxMsgSize, "Too much fields (%d). "
                    "Maximum are %d", errorNumbers, MAX_FIELDS);
            break;

        case TOO_MUCH_DIC_FIELDS:
            snprintf(msg, maxMsgSize, "Too much DIC fields (%d). "
                    "Maximum are %d", errorNumbers, MAX_FIELDS);
            break;

        case WRONG_FID:
            snprintf(msg, maxMsgSize,
                    "Wrong fid (%d). They must be correlative", errorNumbers);
            break;

        case FIELD_NAME_TOO_LONG:
            snprintf(msg, maxMsgSize, "Field name too big (%d). "
                    "Max length is %d", errorNumbers, MAX_FIELD_NAME_LEN-1);
            break;

        case WRONG_PACKET_TAG:
                snprintf(msg, maxMsgSize, "Wrong packet tag");
                break;

        case VBLE_SIZE_WRONG_UNIT:
            snprintf(msg, maxMsgSize, "Wrong variableSize unit. Only valid "
                    "\"bytes\", \"bits\", \"halfword\" and \"string10\"");
            break;

        case VBLE_SIZE_WRONG_POWER:
            snprintf(msg, maxMsgSize, "Wrong variableSize power. "
                    "Only valid \"2\" and \"2_with_0\"");
            break;

        case CRC_FIELD_REF_NO_FULL_BYTE:
            snprintf(msg, maxMsgSize, "fieldToCheck \"fidRef\" %d "
                    "size is not a full byte or multiple", errorNumbers);
            break;

        case FORMULA_TOO_LONG:
            snprintf(msg, maxMsgSize, "formula \"slope\" or "
                    "\"intercept\" too long. Maximum size is %d", ATTR_SIZE);
            break;

        default:
            XMLerrorInfo(status, msg, maxMsgSize);
            break;
    }
    return;
}

static int GetVariableSize(xmlNodePtr element, variableField * value,
	char attrData[ATTR_SIZE])
{
    int status;
    unsigned int numberOfAttrib;
    xmlNodePtr subelement = NULL;
    xmlAttrPtr attribute = NULL;
    if((status = GetXMLChildElementByTag(element, "variableSize", &subelement)) != 0)
    {
        return status;
    }

    if((status = GetXMLAttributeValueByName (subelement, "fidRef", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    value->fieldRef = atoi(attrData);

    if((status = GetXMLAttributeByIndex(subelement, 1, &attribute)) != 0)
    {
        return status;
    }
    if((status = GetXMLAttributeName(attribute, attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    if((status = GetXMLAttributeValueByName (subelement, "unit", attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }

    if(strncmp(attrData, "bytes", 5) == 0)
    {
        value->refUnit = 8;
    }
    else if(strncmp(attrData, "halfword", 8) == 0)
    {
        value->refUnit = 16;
    }
    else if(strncmp(attrData, "string", 6) == 0)
    {
        value->refUnit = 8 * atoi(&attrData[6]);
    }
    else if(strncmp(attrData, "bits", 4) == 0)
    {
        value->refUnit = 1;
    }
    else
    {
        return VBLE_SIZE_WRONG_POWER;
    }

    GetXMLNumAttributes(subelement, &numberOfAttrib);
    if(numberOfAttrib == 3)
    {
        if((status = GetXMLAttributeValueByName (subelement, "power", attrData, ATTR_SIZE)) != 0)
        {
            return status;
        }
        if(strncmp(attrData, "2_with_0", 8) == 0)
        {
            value->refPower = BASE_2_WITH_0;
        }
        else if(strncmp(attrData, "2", 1) == 0)
        {
            value->refPower = BASE_2;
        }
        else
        {
            return VBLE_SIZE_WRONG_UNIT;
        }
    }
    else //if(numberOfAttrib == 2)
    {
        value->refPower = NO_POWER;
    }
    return 0;
}

static int GetFormulaDouble(xmlNodePtr element, const char attrName[],
	double * value, char attrData[ATTR_SIZE])
{
    int status = 0;
    unsigned int length;
    if((status = GetXMLChildAttributeLengthByName (element, "formula",
            attrName, &length)) != 0)
    {
        return status;
    }
    if(length > ATTR_SIZE)
        return FORMULA_TOO_LONG;

    if((status = GetXMLChildAttributeValueByName (element, "formula",
            attrName, attrData, ATTR_SIZE)) != 0)
    {
        return status;
    }
    status = calculateFormula((const char *)attrData, value, NULL);
    return status;
}

static int calculateFormula(const char * formula, double * value,
	unsigned int * retPos)
{
    int status = 0;
    double n1 = 0.0, n2 = 0.0;
    char auxChar, prevAuxChar;
    operation op = NO_OP;
    unsigned int len = strlen(formula), curPos = 1;
    unsigned char minusFirst = 0, endBracket = 0;

    /* first character */
    auxChar = formula[0];
    switch(auxChar)
    {
        case '1': case '2': case '3': case '4': case '5': case '6': case '7':
        case '8': case '9': case '0':
            n1 = atof(&formula[0]);
            break;

        case '-':
            minusFirst = 1;
            break;

        case '(':
            calculateFormula(&formula[1], &n1, &curPos);
            break;

        default:
            return -1;
    }

    /* other characters */
    for(curPos = 1; curPos<len; ++curPos)
    {
        prevAuxChar = formula[curPos-1];
        auxChar = formula[curPos];
        switch(auxChar)
        {
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '0':
                    switch(prevAuxChar)
                    {
                        case '1': case '2': case '3': case '4': case '5':
                        case '6': case '7': case '8': case '9': case '0':
                            break;

                        case '+': case '-': case '*': case '/':
                            if(minusFirst && (prevAuxChar == '-'))
                            {
                                minusFirst = 0;
                                n1 = -atof(&formula[curPos]);
                            }
                            else
                            {
                                n2 = atof(&formula[curPos]);
                            }
                            break;

                        case '.':
                            break;

                        default:
                            return -1;
                    }
                    break;

            case '+': case '-': case '*': case '/':
                    switch(prevAuxChar)
                    {
                        case '1': case '2': case '3': case '4': case '5': case '6':
                        case '7': case '8': case '9': case '0': case ')':
                            if(op != NO_OP)
                            {
                                switch (op)
                                {
                                    case ADD:
                                        n1 = n1 + n2;
                                        break;
                                    case SUB:
                                        n1 = n1 - n2;
                                        break;
                                    case MUL:
                                        n1 = n1 * n2;
                                        break;
                                    case DIV:
                                        n1 = n1 / n2;
                                        break;
                                    default:
                                        break;
                                }
                            }
                            switch (auxChar)
                            {
                                case '+':
                                    op = ADD;
                                    break;
                                case '-':
                                    op = SUB;
                                    break;
                                case '*':
                                    op = MUL;
                                    break;
                                case '/':
                                    op = DIV;
                                    break;
                            }
                            break;

                        case '(':
                            if(auxChar == '-')
                                calculateFormula(&formula[curPos], &n2, &curPos);
                            else
                                return -1;
                            break;

                        default:
                            return -1;
                    }
                    break;

            case '(':
                    switch(prevAuxChar)
                    {
                        case '+': case '-': case '*': case '/':
                            if(minusFirst && (prevAuxChar == '-'))
                            {
                                minusFirst = 0;
                                calculateFormula(&formula[curPos+1], &n1, &curPos);
                                n1 = -n1;
                            }
                            else
                            {
                                calculateFormula(&formula[curPos+1], &n2, &curPos);
                            }
                            break;

                        case '(':
                            if(op == NO_OP)
                                calculateFormula(&formula[curPos+1], &n1, &curPos);
                            else
                                calculateFormula(&formula[curPos+1], &n2, &curPos);
                            break;

                        default:
                            return -1;
                }
                break;

            case ')':
                switch(prevAuxChar)
                {
                    case '1': case '2': case '3': case '4': case '5': case '6':
                    case '7': case '8': case '9': case '0':
                        endBracket = 1;
                        break;

                    default:
                        return -1;
                }
                break;

            case '.':
                switch(prevAuxChar)
                {
                    case '1': case '2': case '3': case '4': case '5': case '6':
                    case '7': case '8': case '9': case '0':
                        break;

                    default:
                        return -1;
                }
                break;

            default:
                return -1;
        }
        if(endBracket)
            break;
    }
    if(retPos != NULL)
        *retPos = *retPos+curPos+1;

    if(op == NO_OP)
        *value = n1;
    else
    {
        switch (op)
        {
            case ADD:
                *value = n1 + n2;
                break;
            case SUB:
                *value = n1 - n2;
                break;
            case MUL:
                *value = n1 * n2;
                break;
            case DIV:
                *value = n1 / n2;
                break;
            default:
                break;
        }
    }
    return status;
}

unsigned int getFormatFromXtext(char * data, unsigned int dataLen,
        formatField * LevelFields, unsigned int numberOfLevelFields,
        unsigned int * pFieldIdx)
{
    unsigned int idx = 0, fieldRef = 0;
    
    //look for field name after "@"
    for(idx=dataLen; idx>0; --idx)
    {
        if(data[idx] == '.')
        {
            fieldRef = (unsigned int)atoi(&data[idx+1]);
            break;
        }
    }
    
    for(idx=0; idx<numberOfLevelFields; ++idx)
    {
        if(idx == fieldRef)
            break;
    }
    if(idx == numberOfLevelFields)
        return -1;
    *pFieldIdx = idx;
    memset(data, 0, dataLen);
    strncpy(data, LevelFields[idx].name, strlen(LevelFields[idx].name)+1);
    return 0;
}
