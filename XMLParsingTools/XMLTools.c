/**
 * \file	XMLTools.c
 * \brief	useful functions for parsing XML files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		18/02/2013 at 12:54:25
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdio.h>
#include <string.h>
#include "XMLTools.h"

enum {
    ELEMENT_NOT_FOUND_TAG = -1001,
    ELEMENT_NOT_FOUND_INDEX = -1002,
    ATTRIBUTE_NOT_FOUND_INDEX = -1003,
    ATTRIBUTE_NOT_FOUND_NAME = -1004,
    ATTRIBUTE_VALUE_TOO_LONG = -1005,
    ATTRIBUTE_VALUE_TOO_LONG_INDEX = -1006,
    ATTRIBUTE_VALUE_TOO_LONG_NAME = -1007,
    ATTRIBUTE_NAME_TOO_LONG = -1008,
    ELEMENT_VALUE_TOO_LONG = -1009,
    ELEMENT_VALUE_TOO_LONG_TAG = -1010,
    FILE_IS_NOT_FIRST_ATTR = -1011,
    NAME_IS_NOT_FIRST_ATTR = -1012,
    ATTRIBUTE_OR_ELEMENT_NOT_FOUND = -1013,
    ERROR_IN_XPATH_NEWCONTEXT = -1014,
    ERROR_IN_XPATH_EVALEXPRESSION = -1015,
    ERROR_IN_XPATH_NO_RESULT = -1016,
};

static char errorTag[4096];
static unsigned int errorIndex = 0;

void XMLerrorInfo(int status, char * msg, int msgSize)
{
    switch(status)
    {
        case ELEMENT_NOT_FOUND_TAG:
            snprintf(msg, msgSize, "Element \"%s\" not found", errorTag);
            break;

        case ELEMENT_NOT_FOUND_INDEX:
            snprintf(msg, msgSize, "Element %d not found", errorIndex);
            break;

        case ATTRIBUTE_NOT_FOUND_INDEX:
            snprintf(msg, msgSize, "Attribute %d not found", errorIndex);
            break;

        case ATTRIBUTE_NOT_FOUND_NAME:
            snprintf(msg, msgSize, "Attribute \"%s\" not found", errorTag);
            break;

        case ATTRIBUTE_VALUE_TOO_LONG:
            snprintf(msg, msgSize, "Attribute value too long");
            break;
            
        case ATTRIBUTE_VALUE_TOO_LONG_INDEX:
            snprintf(msg, msgSize, "Attribute \"%d\" value too long", errorIndex);
            break;

        case ATTRIBUTE_VALUE_TOO_LONG_NAME:
            snprintf(msg, msgSize, "Attribute \"%s\" value too long", errorTag);
            break;
            
        case ATTRIBUTE_NAME_TOO_LONG:
            snprintf(msg, msgSize, "Attribute name too long");
            break;

        case ELEMENT_VALUE_TOO_LONG:
            snprintf(msg, msgSize, "Element name too long");
            break;

        case ELEMENT_VALUE_TOO_LONG_TAG:
            snprintf(msg, msgSize, "Element \"%s\" name too long", errorTag);
            break;

        case FILE_IS_NOT_FIRST_ATTR:
            snprintf(msg, msgSize, "Attribute \"file\" (or \"href\") "
                    "is not the first or the only one");
            break;

        case NAME_IS_NOT_FIRST_ATTR:
            snprintf(msg, msgSize, "Attribute \"%s\" (or \"href\") "
                    "is not the first or the only one", errorTag);
            break;

        case ATTRIBUTE_OR_ELEMENT_NOT_FOUND:
            snprintf(msg, msgSize, "Can not found \"%s\" as attribute or as "
                    "child tag", errorTag);
            break;
                    
        case ERROR_IN_XPATH_NEWCONTEXT:
            snprintf(msg, msgSize, "Error in xmlXPathNewContext");
            break;

        case ERROR_IN_XPATH_EVALEXPRESSION:
            snprintf(msg, msgSize, "Error in xmlXPathEvalExpression");
            break;

        case ERROR_IN_XPATH_NO_RESULT:
            snprintf(msg, msgSize, "No result in xPath");
            break;
            
        default:
            snprintf(msg, msgSize, "Unknown XML error %d", status);
            break;
    }
    return;
}

int GetXMLNumChildren (xmlNodePtr element, unsigned int * pNum)
{
    *pNum = (unsigned int)xmlChildElementCount(element);
    return 0;
}

int GetXMLChildElementByTag(xmlNodePtr element, const char * tag, xmlNodePtr * child)
{
    xmlNodePtr temp = NULL;
    for (temp = element->children; temp; temp = temp->next)
    {
        if((temp->type == XML_ELEMENT_NODE) && (!strcmp((char*)temp->name, tag)))
        {
            break;
        }
    }
    if(temp == NULL)
    {
    	memset(errorTag, 0, 4096);
    	strncpy(errorTag, tag, 4096);
    	return ELEMENT_NOT_FOUND_TAG;
    }
    *child = temp;
    return 0;
}

int GetXMLChildElementByIndex(xmlNodePtr element, unsigned int idx, xmlNodePtr * child)
{
    xmlNodePtr temp = NULL;
    unsigned int current_idx=0;
    for (temp = element->children; temp; temp = temp->next)
    {
        if(temp->type == XML_ELEMENT_NODE)
        {
            if(current_idx == idx)
            {
            	break;
            }
            current_idx++;
        }
    }
    if(temp == NULL)
    {
    	errorIndex = idx;
    	return ELEMENT_NOT_FOUND_INDEX;
    }
    *child = temp;
    return 0;
}

int GetXMLChildNumAttributes (xmlNodePtr element, const char * tag,
        unsigned int * pNum)
{
    int status = 0;
    xmlNodePtr child = NULL;
    if((status = GetXMLChildElementByTag(element, tag, &child)) != 0)
    {
        return status;
    }
    status = GetXMLNumAttributes (child, pNum);
    return status;
}

int GetXMLNumAttributes (xmlNodePtr element, unsigned int * pNum)
{
    unsigned int ret=0;
    xmlAttrPtr attr;
    for (attr = element->properties; attr; attr = attr->next)
    {
        if(attr->type == XML_ATTRIBUTE_NODE)
        {
            ret++;
        }
    }
    *pNum = ret;
    return 0;
}

int GetXMLAttributeByIndex (xmlNodePtr element, int idx, xmlAttrPtr * attr)
{
    unsigned int current_idx=0;
    xmlAttrPtr temp;
    for (temp = element->properties; temp; temp = temp->next)
    {
        if(temp->type == XML_ATTRIBUTE_NODE)
        {
            if(current_idx == (unsigned int)idx)
            {
                break;
            }
            current_idx++;
        }
    }

    //if idx is -1, last attribute. iterate again
    if(idx == -1)
    {
    	idx = current_idx-1; //last XML_ATTRIBUTE_NODE
    	current_idx = 0;
        for (temp = element->properties; temp; temp = temp->next)
        {
            if(temp->type == XML_ATTRIBUTE_NODE)
            {
                if(current_idx == (unsigned int)idx)
                {
                    break;
                }
                current_idx++;
            }
        }
    }
    if(temp == NULL)
    {
    	errorIndex = idx;
    	return ATTRIBUTE_NOT_FOUND_INDEX;
    }
    *attr = temp;
    return 0;
}

int GetXMLAttributeValue (xmlAttrPtr attr, char * value, unsigned int maxSize)
{
    xmlChar * tempData = NULL;
    unsigned int length = 0;
    tempData = xmlNodeListGetString(attr->doc, attr->children, 1);
    length = strlen((char*)tempData);
    if(length > maxSize)
   {
        xmlFree(tempData);
        return ATTRIBUTE_VALUE_TOO_LONG;
    }
    memset(value, 0, maxSize);
    strncpy(value, (const char*)tempData, length);
    xmlFree(tempData);
    return 0;
}

int GetXMLAttributeLength (xmlAttrPtr attr, unsigned int * pLength)
{
    xmlChar * tempData;
    tempData = xmlNodeListGetString(attr->doc, attr->children, 1);
    *pLength = strlen((char*)tempData) + 1;// plus '\0'
    xmlFree(tempData);
    return 0;
}

int GetXMLAttributeName (xmlAttrPtr attr, char * name, unsigned int maxSize)
{
    unsigned int length = 0;
    length = strlen((char*)attr->name);
    if(length > maxSize)
    {
        return ATTRIBUTE_NAME_TOO_LONG;
    }
    memset(name, 0, maxSize);
    strncpy(name, (const char*)attr->name, length);
    return 0;
}

int GetXMLAttributeValueByName(xmlNodePtr element, const char * name,
	char * value, unsigned int maxSize)
{
    xmlChar * tempData;
    xmlAttrPtr attr;
    unsigned int length = 0;
    for (attr = element->properties; attr; attr = attr->next)
    {
        if((attr->type == XML_ATTRIBUTE_NODE) && (!strcmp((char*)attr->name, name)))
        {
            tempData = xmlNodeListGetString(element->doc, attr->children, 1);
            length = strlen((char*)tempData);
            if(length > maxSize)
            {
                memset(errorTag, 0, 4096);
                strncpy(errorTag, name, 4096);
                xmlFree(tempData);
                return ATTRIBUTE_VALUE_TOO_LONG_NAME;
            }
            memset(value, 0, maxSize);
            strncpy(value, (const char*)tempData, length);
            xmlFree(tempData);
            break;
        }
    }
    if(attr == NULL)
    {
    	memset(errorTag, 0, 4096);
    	strncpy(errorTag, name, 4096);
        return ATTRIBUTE_NOT_FOUND_NAME;
    }
    else
    {
        return 0;
    }
}
int SearchXMLAttributeValueByName (xmlNodePtr element, const char * name,
	char * value, unsigned int maxSize)
{
    int status = GetXMLAttributeValueByName(element, name, value, maxSize);
    if(status == ATTRIBUTE_NOT_FOUND_NAME)
    {
        value[0] = '\0';
        status = 0;
    }
    return status;
}

int GetXMLAttributeValueByNameSkip (xmlNodePtr element, const char * name,
	unsigned int skip, char * value, unsigned int maxSize)
{
    if(skip == 0)
    {
        return GetXMLAttributeValueByName(element, name, value, maxSize);
    }
    else
    {
        unsigned int skipped = 0;
        xmlChar * tempData;
        xmlAttrPtr attr;
        unsigned int length = 0;
        for (attr = element->properties; attr; attr = attr->next)
        {
            if((attr->type == XML_ATTRIBUTE_NODE) && (!strcmp((char*)attr->name, name)))
            {
                if(skipped == skip)
                {
                    tempData = xmlNodeListGetString(element->doc, attr->children, 1);
                    length = strlen((char*)tempData);
                    if(length > maxSize)
                    {
                        memset(errorTag, 0, 4096);
                        strncpy(errorTag, name, 4096);
                        xmlFree(tempData);
                        return ATTRIBUTE_VALUE_TOO_LONG_NAME;
                    }
                    memset(value, 0, maxSize);
                    strncpy(value, (const char*)tempData, length);
                    xmlFree(tempData);
                    break;
                }
                else
                {
                    skipped++;
                }
            }
        }
        if(attr == NULL)
        {
            memset(errorTag, 0, 4096);
            strncpy(errorTag, name, 4096);
            return ATTRIBUTE_NOT_FOUND_NAME;
        }
        else
        {
            return 0;
        }
    }
}

int GetXMLAttributeLengthByName (xmlNodePtr element, const char * name,
	unsigned int * pLength)
{
    xmlChar * tempData = NULL;
    xmlAttrPtr attr;
    for (attr = element->properties; attr; attr = attr->next)
    {
        if((attr->type == XML_ATTRIBUTE_NODE) && (!strcmp((char*)attr->name, name)))
        {
            tempData = xmlNodeListGetString(element->doc, attr->children, 1);
            break;
        }
    }
    if(attr == NULL)
    {
    	memset(errorTag, 0, 4096);
    	strncpy(errorTag, name, 4096);
        return ATTRIBUTE_NOT_FOUND_NAME;
    }
    *pLength = strlen((char*)tempData) + 1;// plus '\0'
    xmlFree(tempData);
    return 0;
}

int GetXMLAttributeValueByIndex (xmlNodePtr element, unsigned int idx,
	char * value, unsigned int maxSize)
{
    int status = 0;
    xmlAttrPtr attr;
    
    if((status = GetXMLAttributeByIndex(element, idx, &attr)) == 0)
        status = GetXMLAttributeValue (attr, value, maxSize);
    return status;
}

int GetXMLAttributeLengthByIndex (xmlNodePtr element, unsigned int idx,
	unsigned int * pLength)
{
    int status = 0;
    xmlAttrPtr attr;
    
    if((status = GetXMLAttributeByIndex(element, idx, &attr)) == 0)
        status = GetXMLAttributeLength (attr, pLength);
    return status;
}

int GetXMLAttributeNameByIndex (xmlNodePtr element, unsigned int idx,
        char * name, unsigned int maxSize)
{
    int status = 0;
    xmlAttrPtr attr;
    
    if((status = GetXMLAttributeByIndex(element, idx, &attr)) == 0)
        status = GetXMLAttributeName (attr, name, maxSize);
    return status;
}

int GetXMLAttributeNameValueByIndex (xmlNodePtr element, unsigned int idx,
        char * name, unsigned int maxNameSize, char * value,
        unsigned int maxValueSize)
{
    int status = 0;
    if((status = GetXMLAttributeNameByIndex (element, idx, name, maxNameSize)) != 0)
        return status;
    status = GetXMLAttributeValueByIndex (element, idx, value, maxValueSize);
    return status;
}

int GetXMLChildAttributeValueByName (xmlNodePtr element, const char * tag,
	const char * name, char * value, unsigned int maxSize)
{
    int status = 0;
    xmlNodePtr child = NULL;
    if((status = GetXMLChildElementByTag(element, tag, &child)) == 0)
        status = GetXMLAttributeValueByName (child, name, value, maxSize);
    return status;
}

int GetXMLChildAttributeLengthByName (xmlNodePtr element, const char * tag,
	const char * name, unsigned int * pLength)
{
    int status = 0;
    xmlNodePtr child = NULL;
    if((status = GetXMLChildElementByTag(element, tag, &child)) == 0)
        status = GetXMLAttributeLengthByName (child, name, pLength);
    return status;
}

int GetXMLChildAttributeNameByIndex (xmlNodePtr element, const char * tag,
        unsigned int idx, char * name, unsigned int maxSize)
{
    int status = 0;
    xmlNodePtr child = NULL;
    if((status = GetXMLChildElementByTag(element, tag, &child)) == 0)
        status = GetXMLAttributeNameByIndex (child, idx, name, maxSize);
    return status;
}

int GetXMLChildAttributeValueByIndex (xmlNodePtr element, const char * tag,
	unsigned int idx, char * value, unsigned int maxSize)
{
    int status = 0;
    xmlNodePtr child = NULL;
    if((status = GetXMLChildElementByTag(element, tag, &child)) == 0)
        status = GetXMLAttributeValueByIndex (child, idx, value, maxSize);
    return status;
}

int GetXMLChildAttributeLengthByIndex (xmlNodePtr element, const char * tag,
	unsigned int idx, unsigned int * pLength)
{
    int status = 0;
    xmlNodePtr child = NULL;
    if((status = GetXMLChildElementByTag(element, tag, &child)) == 0)
        status = GetXMLAttributeLengthByIndex (child, idx, pLength);
    return status;
}

int GetXMLChildAttributeNameValueByIndex (xmlNodePtr element, const char * tag,
        unsigned int idx, char * name, unsigned int maxNameSize,
        char * value, unsigned int maxValueSize)
{
    int status = 0;
    xmlNodePtr child = NULL;
    if((status = GetXMLChildElementByTag(element, tag, &child)) != 0)
        return status;
    if((status = GetXMLAttributeNameByIndex (child, idx, name, maxNameSize)) != 0)
        return status;
    status = GetXMLAttributeValueByIndex (child, idx, value, maxValueSize);
    return status;
}

int GetFileAsXMLChildOnlyAttribute (xmlNodePtr element, const char * tag,
        char * value, unsigned int maxValueSize)
{
    int status = 0;
    char name[5];
    if((status = GetXMLChildAttributeNameValueByIndex (element, tag, 0,
            name, 5, value, maxValueSize)) != 0)
    {
        return status;
    }
    if(!strncmp(name, "type", 4))
    {
        if((status = GetXMLChildAttributeNameValueByIndex (element, tag, 1,
            name, 5, value, maxValueSize)) != 0)
        {
            return status;
        }
    }
    if(!strncmp(name, "href", 4))
    {
        value[strlen(value)-2] = '\0'; //remove "#/"
    }
    else if(strncmp(name, "file", 4))
    {
        return FILE_IS_NOT_FIRST_ATTR;
    }
    return status;
}

int TryGetXMLOnlyChildAttributeValueByName(xmlNodePtr element, const char * tag,
	const char * name, char * value, unsigned int maxSize)
{
    int status = 0;
    xmlNodePtr child = NULL;
    
    if((status = GetXMLChildElementByTag(element, tag, &child)) != 0)
    {
        if(status == ELEMENT_NOT_FOUND_TAG)
        {
            strncpy(value, "DEFAULT", 7);
            status = 0;
        }
        return status;
    }
    status = TryGetXMLAttributeValueByName(child, 0, name, value, maxSize);
    return status;
}

int TryGetXMLAttributeValueByName(xmlNodePtr element, unsigned int idx,
        const char * name, char * value, unsigned int maxSize)
{
    int status = 0;
    char attr_name[7];
    
    if((status = GetXMLAttributeNameValueByIndex (element, idx, attr_name,
            7, value, maxSize)) != 0)
    {
        if(status == ATTRIBUTE_NOT_FOUND_INDEX)
        {
            strncpy(value, "DEFAULT", 7);
            status = 0;
        }
        return status;
    }
    if(!strncmp(attr_name, "href", 4))
    {
        value[strlen(value)-2] = '\0'; //remove "#/"
    }
    else if(strncmp(attr_name, name, 7))
    {
    	memset(errorTag, 0, 4096);
    	strncpy(errorTag, name, 4096);
        return NAME_IS_NOT_FIRST_ATTR;
    }
    return status;
}

int GetXMLValueFromAttributeOrHRefTag(xmlNodePtr element,
        const char * name_tag, char * value, unsigned int maxValueSize)
{
    int status = 0;
    if(((status = GetXMLAttributeValueByName (element, name_tag, value, maxValueSize)) != 0)
            && (status == ATTRIBUTE_NOT_FOUND_INDEX))
    {
        return status;
    }
    if(status == ATTRIBUTE_NOT_FOUND_NAME)
    {
        xmlNodePtr child = NULL;
        if((status = GetXMLChildElementByTag (element, name_tag, &child)) == 0)
        {
            status = GetXMLAttributeValueByName (child, "href", value, maxValueSize);
            if((status == 0) && (value[strlen(value)-2] == '#'))
            {
                value[strlen(value)-2] = '\0'; //remove "#/"
            }
        }
        else if(status == ELEMENT_NOT_FOUND_TAG)
        {
            memset(errorTag, 0, 4096);
            strncpy(errorTag, name_tag, 4096);
            status = ATTRIBUTE_OR_ELEMENT_NOT_FOUND;
        }
    }
    return status;
}

int GetXMLLengthFromAttributeOrHRefTag(xmlNodePtr element,
        const char * name_tag, unsigned int * pLength)
{
    int status = 0;
    
    if(((status = GetXMLAttributeLengthByName (element, name_tag, pLength)) != 0)
            && (status == ATTRIBUTE_NOT_FOUND_INDEX))
    {
        return status;
    }
    if(status == ATTRIBUTE_NOT_FOUND_NAME)
    {
        xmlNodePtr child = NULL;
        if((status = GetXMLChildElementByTag (element, name_tag, &child)) == 0)
        {
            status = GetXMLAttributeLengthByName (child, "href", pLength);
            if(status == 0)
            {
                *pLength = (*pLength)-2; //remove "#/"
            }
        }
        else if(status == ELEMENT_NOT_FOUND_TAG)
        {
            memset(errorTag, 0, 4096);
            strncpy(errorTag, name_tag, 4096);
            status = ATTRIBUTE_OR_ELEMENT_NOT_FOUND;
        }
    }
    return status;
}

int GetXMLLengthFromChildAttributeOrHRefTag(xmlNodePtr element, const char * tag,
        const char * name_tag, unsigned int * pLength)
{
    int status = 0;
    xmlNodePtr child = NULL;
    if((status = GetXMLChildElementByTag(element, tag, &child)) == 0)
        status = GetXMLLengthFromAttributeOrHRefTag (child, name_tag, pLength);
    return status;
}

int GetXMLValueFromChildAttributeOrHRefTag(xmlNodePtr element, const char * tag, 
        const char * name_tag, char * value, unsigned int maxValueSize)
{
    int status = 0;
    xmlNodePtr child = NULL;
    if((status = GetXMLChildElementByTag(element, tag, &child)) == 0)
        status = GetXMLValueFromAttributeOrHRefTag (child, name_tag, value, maxValueSize);
    return status;
}

int TryGetXMLValueFromAttributeOrHRefTag(xmlNodePtr element,
        const char * name_tag, char * value, unsigned int maxValueSize,
        const char * defaultValue, unsigned int defaultValueSize)
{
    int status = 0;
    if((status = GetXMLValueFromAttributeOrHRefTag(element, name_tag, value,
            maxValueSize)) == ATTRIBUTE_OR_ELEMENT_NOT_FOUND)
    {
        strncpy(value, defaultValue, defaultValueSize);
        status = 0;
    }
    return status;
}

int TryGetXMLValueFromChildAttributeOrHRefTag (xmlNodePtr element,
        const char * tag, const char * name_tag, char * value,
        unsigned int maxSize, const char * defaultValue,
        unsigned int defaultValueSize)
{
    int status = 0;
    xmlNodePtr child = NULL;
    if((status = GetXMLChildElementByTag (element, tag, &child)) != 0)
    {
        return status;
    }
    return TryGetXMLValueFromAttributeOrHRefTag(child, name_tag, value,
            maxSize, defaultValue, defaultValueSize);
}

int GetXMLElementValue (xmlNodePtr element, char * value, unsigned int maxSize)
{
    int status = 0;
    xmlChar * tempData;
    unsigned int length = 0;
    
    tempData = xmlNodeListGetString(element->doc, element->xmlChildrenNode, 1);
    if(tempData == NULL)
    {
        value[0] = '\0';
    }
    else
    {
        length = strlen((char*)tempData);
        if(length > maxSize)
        {
            xmlFree(tempData);
            return ELEMENT_VALUE_TOO_LONG;
        }
        memset(value, 0, maxSize);
        strncpy(value, (const char *)tempData, maxSize);
        xmlFree(tempData);
    }
    return status;
}

int GetXMLElementLength (xmlNodePtr element, unsigned int * pLength)
{
    int status = 0;
    xmlChar * tempData;
    tempData = xmlNodeListGetString(element->doc, element->xmlChildrenNode, 1);
    *pLength = (unsigned int)strlen((char*)tempData);
    xmlFree(tempData);
    return status;
}

char * fileExists(const char * filename, const char * relativePath,
        char * foundFilePath, unsigned int maxPathLen)
{
    char * ret = NULL;
    FILE * file = NULL;
    if((file = fopen(filename, "r")) != NULL)
    {
        fclose(file);
        strncpy(foundFilePath, filename, maxPathLen);
        ret = foundFilePath;
    }
    else if(relativePath != NULL)
    {
        memset(foundFilePath, 0, maxPathLen);
        strncpy(foundFilePath, relativePath, maxPathLen);
        strncat(foundFilePath, filename, maxPathLen-strlen(relativePath)-1);
        
        if((file = fopen(foundFilePath, "r")) != NULL)
        {
            fclose(file);
            ret = foundFilePath;
        }
    }
    return ret;
}

int GetFieldInBits(xmlNodePtr element, const char tag[], int * pValue,
	char * attrData, int attrDataSize)
{
    int status = 0;
    xmlNodePtr elementChild = NULL;
    if((status = GetXMLChildElementByTag(element, tag, &elementChild)) != 0)
        return status;

    if((status = GetXMLAttributeValueByName (elementChild, "bytes", attrData,
            attrDataSize)) != 0)
        return status;
    if(attrData[0] == '-') //for "constSize
        (*pValue) = -atoi((char*)&attrData[1])* 8;
    else
        (*pValue) = atoi((char*)attrData)* 8;

    if((status = GetXMLAttributeValueByName (elementChild, "bits", attrData,
            attrDataSize)) != 0)
        return status;
    if(attrData[0] == '-') //for "constSize
        (*pValue) -= atoi((char*)&attrData[1]);
    else
        (*pValue) += atoi((char*)attrData);
    return 0;
}

unsigned int getUnderscoredNumber(const char * str)
{
    unsigned int aux = 0;
    if(str[0] == '_')
        aux = 1;
    return((unsigned int)strtoul(&str[aux], NULL, 0));
}

unsigned int getRelativePath(const char * filename, char * relativePath,
        unsigned int maxLen)
{
    int idx = 0;
    strncpy(relativePath, filename, strlen(filename)+1);
    for(idx=strlen(relativePath); idx>=0; --idx)
    {
        if((relativePath[idx] == '/') || (relativePath[idx] == '\\'))
            break;
    }
    memset(&relativePath[idx+1], 0, maxLen-idx-1);
    return idx+1;
}

int getXPathObject(xmlDocPtr doc, xmlChar *xpath, xmlXPathObjectPtr * pResult)
{
    int status = 0;
    xmlXPathContextPtr context = NULL;

    if((context = xmlXPathNewContext(doc)) == NULL)
    {
        return ERROR_IN_XPATH_NEWCONTEXT;
    }
    *pResult = xmlXPathEvalExpression(xpath, context);
    xmlXPathFreeContext(context);
    if(*pResult == NULL)
    {
        return ERROR_IN_XPATH_EVALEXPRESSION;
    }
    if(xmlXPathNodeSetIsEmpty((*pResult)->nodesetval))
    {
        xmlXPathFreeObject(*pResult);
        status = ERROR_IN_XPATH_NO_RESULT;
    }
    return status;
}