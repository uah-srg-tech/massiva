/**
 * \file	XMLTools.h
 * \brief	useful functions for parsing XML files (declaration)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		18/02/2013 at 12:54:25
 * Company:		Space Research Group, Universidad de Alcalï¿½.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __XMLTools_H__
#define __XMLTools_H__

#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ATTR_SIZE       90
#define MAX_STR_LEN     260
    
void XMLerrorInfo(int status, char * msg, int msgSize);

int GetXMLNumChildren (xmlNodePtr element, unsigned int * pNum);
int GetXMLChildElementByTag(xmlNodePtr element, const char * tag,
	xmlNodePtr * child);
int GetXMLChildElementByIndex(xmlNodePtr element, unsigned int idx,
	xmlNodePtr * child);
int GetXMLChildNumAttributes (xmlNodePtr element, const char * tag,
        unsigned int * pNum);

int GetXMLNumAttributes (xmlNodePtr element, unsigned int * pNum);
int GetXMLAttributeByIndex (xmlNodePtr element, int idx, xmlAttrPtr * attr);

int GetXMLAttributeValue (xmlAttrPtr attr, char * value, unsigned int maxSize);
int GetXMLAttributeLength (xmlAttrPtr attr, unsigned int * pLength);
int GetXMLAttributeName (xmlAttrPtr attr, char * name, unsigned int maxSize);

int GetXMLAttributeValueByName (xmlNodePtr element, const char * name,
	char * value, unsigned int maxSize);
int SearchXMLAttributeValueByName (xmlNodePtr element, const char * name,
	char * value, unsigned int maxSize);
int GetXMLAttributeValueByNameSkip (xmlNodePtr element, const char * name,
	unsigned int skip, char * value, unsigned int maxSize);
int GetXMLAttributeLengthByName (xmlNodePtr element, const char * name,
	unsigned int * pLength);

int GetXMLAttributeValueByIndex (xmlNodePtr element, unsigned int idx,
	char * value, unsigned int maxSize);
int GetXMLAttributeLengthByIndex (xmlNodePtr element, unsigned int idx,
	unsigned int * pLength);
int GetXMLAttributeNameByIndex (xmlNodePtr element, unsigned int idx,
        char * name, unsigned int maxSize);
int GetXMLAttributeNameValueByIndex (xmlNodePtr element, unsigned int idx,
        char * name, unsigned int maxNameSize, char * value,
        unsigned int maxValueSize);

int GetXMLChildAttributeValueByName (xmlNodePtr element, const char * tag,
	const char * name, char * value, unsigned int maxSize);
int GetXMLChildAttributeLengthByName (xmlNodePtr element, const char * tag,
        const char * name, unsigned int * pLength);

int GetXMLChildAttributeNameByIndex (xmlNodePtr element, const char * tag,
        unsigned int idx, char * name, unsigned int maxSize);
int GetXMLChildAttributeValueByIndex (xmlNodePtr element, const char * tag,
	unsigned int idx, char * value, unsigned int maxSize);
int GetXMLChildAttributeLengthByIndex (xmlNodePtr element, const char * tag,
        unsigned int idx, unsigned int * pLength);
int GetXMLChildAttributeNameValueByIndex (xmlNodePtr element, const char * tag,
        unsigned int idx, char * name, unsigned int maxNameSize,
        char * value, unsigned int maxValueSize);

int GetFileAsXMLChildOnlyAttribute (xmlNodePtr element, const char * tag,
        char * value, unsigned int maxValueSize);
int TryGetXMLOnlyChildAttributeValueByName(xmlNodePtr element, const char * tag,
	const char * name, char * value, unsigned int maxSize);
int TryGetXMLAttributeValueByName(xmlNodePtr element, unsigned int idx,
        const char * name, char * value, unsigned int maxSize);

int GetXMLValueFromAttributeOrHRefTag(xmlNodePtr element,
        const char * name_tag, char * value, unsigned int maxValueSize);
int GetXMLLengthFromAttributeOrHRefTag(xmlNodePtr element,
        const char * name_tag, unsigned int * pLength);
int TryGetXMLValueFromAttributeOrHRefTag(xmlNodePtr element,
        const char * name_tag, char * value, unsigned int maxValueSizemax,
        const char * defaultValue, unsigned int defaultValueSize);
int TryGetXMLValueFromChildAttributeOrHRefTag (xmlNodePtr element,
        const char * tag, const char * name_tag, char * value,
        unsigned int maxSize, const char * defaultValue,
        unsigned int defaultValueSize);

int GetXMLElementValue (xmlNodePtr element, char * value, unsigned int maxSize);
int GetXMLElementLength (xmlNodePtr element, unsigned int * pLength);

char * fileExists(const char * filename, const char * relativePath,
        char * foundFilePath, unsigned int maxPathLen);

int GetFieldInBits(xmlNodePtr element, const char tag[], int * pValue,
	char * attrData, int attrDataSize);

unsigned int getUnderscoredNumber(const char * str);

unsigned int getRelativePath(const char * filename, char * relativePath,
        unsigned int maxLen);

int getXPathObject(xmlDocPtr doc, xmlChar *xpath, xmlXPathObjectPtr * pResult);

#ifdef __cplusplus
}
#endif
#endif  /* ndef __XMLTools__ */
