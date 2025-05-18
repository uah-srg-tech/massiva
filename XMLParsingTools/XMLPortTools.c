/**
 * \file	XMLPortTools.c
 * \brief	functions for parsing XML port files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		05/03/2012 at 17:48:22
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdio.h>							/* sprintf */
#include <stdlib.h>
#include <string.h>
#include "XMLTools.h"						/* GetXMLAttributeValueByName, ... */
#include "../definitions.h"					/* portConfig, MAX_MSG_SIZE */


#define ATTR_PORT_SIZE  32
enum {
    PORT_FILE_NOT_FOUND = -111,
    PORT_FILE_PARSING_ERROR = -112,
    ONE_PORT_PER_FILE = -113,
    UNKNOWN_PORT = -114,
    SPW_REF_NOT_FOUND = -115,
    WRONG_PORT_SPW = -116,
    WRONG_PORT_SPW_TC = -117,
    WRONG_PORT_UART = -118,
    WRONG_PORT_SOCKET = -119,
    WRONG_SOCKET_ROLE = -120,
    WRONG_SYNC_PATTERN_LEN = -121,
    BIG_SYNC_PATTERN_LEN = -122,
    WRONG_PORT_SPW_TYPE = -123,
    WRONG_PARITY_VALUE = -124,
    WRONG_FLOW_CONTROL = -125,
};

static int deviceError = 0;
static char portFileError[65];

static int ParsePortProtocol(xmlNodePtr ptclRoot, char attrData[32],
        protocolConfig * pPtcl, unsigned int * pPortPhyHeaderOffsetTM,
        unsigned int * pPortPhyHeaderOffsetTC);

/**
 * \brief Function to parse the file where SpW device is defined
 * \param	portConfigFile	File where the XML SpW device is defined
 * \param[out]	port		A struct to save the port configuration
 * \return	0 if correct, a negative value if error occurred
 */
static int ParseSpWPort (const char * portConfigFile, const char * relativePath,
        portConfig * pPort, unsigned int portIndex);
/**
 * \brief Function to parse the file where SpW timecodes device is defined
 * \param	portConfigFile	File where the XML SpW timecodes device is defined
 * \param[out]	port		A struct to save the port configuration
 * \return	0 if correct, a negative value if error occurred
 */
static int ParseSpWAuxPort (const char * portConfigFile, const char * relativePath,
        portConfig * ports, unsigned int portIndex);

static int ParseTCPSocketPort (const char * portConfigFile,
        const char * relativePath, portConfig * pPort, unsigned int portIndex);
		
static int ParseUDPSocketPort (const char * portConfigFile,
        const char * relativePath, portConfig * pPort, unsigned int portIndex);
/**
 * \brief Function to parse the file where XML serial port is defined
 * \param	portConfigFile	File where the XML serial port is defined
 * \param[out]	port		A struct to save the port configuration
 * \return	0 if correct, a negative value if error occurred
 */
static int ParseSerialPort (const char * portConfigFile, const char * relativePath,
        portConfig * pPort, serialConfig * pSerial, unsigned int portIndex);

int ParsePort(char * portConfigFile, portConfig * ports, serialConfig * serials,
        int index, const char * relativePath)
{
    int status = 0;
    strncpy(portFileError, portConfigFile, 65);

    switch(ports[index].portType)
    {
        case SPW_USB_PORT: case SPW_USB_MK2_PORT: case SPW_PCI_PORT:
            if((status = ParseSpWPort(portConfigFile, relativePath,
                    &ports[index], index)) != 0)
            {
                return status;
            }
            break;

        case SPW_TC_PORT: case SPW_ERROR_PORT:
            if((status = ParseSpWAuxPort(portConfigFile, relativePath,
                    ports, index)) != 0)
            {
                return status;
            }
            break;

        case UART_PORT:
            if((status = ParseSerialPort(portConfigFile, relativePath,
                    &ports[index], &serials[index], index)) != 0)
            {
                return status;
            }
            break;

        case TCP_SOCKET_CLI_PORT: case TCP_SOCKET_SRV_PORT:
            if((status = ParseTCPSocketPort(portConfigFile, relativePath,
                    &ports[index], index)) != 0)
            {
                return status;
            }
            break;

        case UDP_SOCKET_PORT:
            if((status = ParseUDPSocketPort(portConfigFile, relativePath,
                    &ports[index], index)) != 0)
            {
                return status;
            }
            break;

        case DUMMY_PORT:
            break;

        default:
            return UNKNOWN_PORT;
    }
    return 0;
}

void DisplayParsePortError (int status, portConfig * pPort, char * msg,
        unsigned int maxMsgSize)
{
    unsigned int len = 0;
    switch (pPort->portType)
    {
        case SPW_USB_PORT:
            len = snprintf(msg, maxMsgSize, "Error while parsing SpW "
                    "port file \"%s\": ", portFileError); 
            break;

        case SPW_USB_MK2_PORT:
            len = snprintf(msg, maxMsgSize, "Error while parsing SpW MK2 "
                    "port file \"%s\": ", portFileError); 
            break;

        case SPW_PCI_PORT:
            len = snprintf(msg, maxMsgSize, "Error while parsing SpW PCI2 "
                    "port file \"%s\": ", portFileError); 
            break;

        case UART_PORT:
            len = snprintf(msg, maxMsgSize, "Error while parsing serial "
                    "port file \"%s\": ", portFileError);
            break;

        case TCP_SOCKET_SRV_PORT: case TCP_SOCKET_CLI_PORT:
            len = snprintf(msg, maxMsgSize, "Error while parsing TCP socket "
                    "port file \"%s\": ", portFileError);
            break;

        case UDP_SOCKET_PORT:
            len = snprintf(msg, maxMsgSize, "Error while parsing UDP socket "
                    "port file \"%s\": ", portFileError);
            break;

        default:
            len = snprintf(msg, maxMsgSize, "Error while parsing unknown "
                    "port file \"%s\": ", portFileError);
            break;
    }

    switch (status)
    {
        case PORT_FILE_NOT_FOUND:
            snprintf(msg, maxMsgSize-len, "Port file \"%s\" not found",
                    portFileError);
            break;
            
        case PORT_FILE_PARSING_ERROR:
            snprintf(msg, maxMsgSize-len, "Port file \"%s\" parsing error",
                    portFileError);
            break;

        case ONE_PORT_PER_FILE:
            snprintf(&msg[len], maxMsgSize-len, "Only allowed 1 port per file");
            break;

        case UNKNOWN_PORT:
            snprintf(&msg[len], maxMsgSize-len, "Unknown port");
            break;

        case SPW_REF_NOT_FOUND:
            snprintf(&msg[len], maxMsgSize-len, "Selected SpW TC (id %d) "
                    "does not reference a valid SpW port", deviceError);
            break;
            
        case WRONG_PORT_SPW:
            snprintf(&msg[len], maxMsgSize-len, "Selected SpW (id %d) file"
                    " is not a valid SpW port file", deviceError);
            break;
            
        case WRONG_PORT_SPW_TC:
            snprintf(&msg[len], maxMsgSize-len, "Selected SpW TC (id %d) file"
                    " is not a valid SpW TC port file", deviceError);
            break;
            
        case WRONG_PORT_UART:
            snprintf(&msg[len], maxMsgSize-len, "Selected UART (id %d) file"
                    " is not a valid UART port file", deviceError);
            break;
            
        case WRONG_PORT_SOCKET:
            snprintf(&msg[len], maxMsgSize-len, "Selected socket (id %d) file"
                    " is not a valid socket port file", deviceError);
            break;
            
        case WRONG_SOCKET_ROLE:
            snprintf(&msg[len], maxMsgSize-len, "Selected socket (id %d) file"
                    " was not defined correctly as client or as server", deviceError);
            break;
            
        case WRONG_SYNC_PATTERN_LEN:
            snprintf(&msg[len], maxMsgSize-len, "Wrong \"syncPattern\" length "
                    "(%d) - bigger than \"sizeFieldOffset\"", deviceError);
            break;
            
        case BIG_SYNC_PATTERN_LEN:
            snprintf(&msg[len], maxMsgSize-len, "Wrong \"syncPattern\" length "
                    "(%d) - bigger than maximum %d", deviceError, MAX_PHY_HDR);
            break;
            
        case WRONG_PORT_SPW_TYPE:
            snprintf(&msg[len], maxMsgSize-len, "Wrong spw \"type\"");
            break;
            
        case WRONG_PARITY_VALUE:
            snprintf(&msg[len], maxMsgSize-len, "Wrong flow control. Only valid"
                    " \"NO\", \"ODD\", \"EVEN\", \"MARK\" and \"SPACE\" "
                    "(UPPERCASE only)");
            break;
            
        case WRONG_FLOW_CONTROL:
#if(defined _WIN32 || __CYGWIN__)
            snprintf(&msg[len], maxMsgSize-len, "Wrong flow control. Only valid"
                    " \"hw\", \"rts_cts\", \"xon_xoff\" and \"dtr_dsr\"");
#elif(defined __linux__)
            snprintf(&msg[len], maxMsgSize-len, "Wrong flow control. Only valid"
                    " \"hw\", \"rts_cts\" and \"xon_xoff\"");
#endif
            break;
            
        default:
            XMLerrorInfo(status, &msg[len], maxMsgSize-len);
            break;
    }
    return;
}

static int ParseSpWPort (const char * portConfigFile, const char * relativePath,
        portConfig * pPort, unsigned int portIndex)
{
    int status;
    unsigned int numberOfChildren, numberOfReadingPorts, idx;
    char attrData[ATTR_SIZE];
    int transmitLink, writePort, readPort[MAX_READ_PORTS];
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL, child = NULL, greatchild = NULL;
    char fullPath[MAX_STR_LEN];
	
    /* look for file */
    if(fileExists(portConfigFile, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
    	strncpy(portFileError, portConfigFile, 65);
        return PORT_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return PORT_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        return PORT_FILE_PARSING_ERROR;
    }

    /* NOTE: "name" is not used */
    GetXMLNumChildren(root, &numberOfChildren);
    if(numberOfChildren != 1)
    {
        return ONE_PORT_PER_FILE;
    }

    /* get xml format file */ 
    if((status = GetXMLChildElementByIndex (root, 0, &child)) != 0)
    {
        return status;
    }
    /* check if port file is spwPort and */
    /* get element tag to distinguish between spw devices */
    if(!(strncmp((char*)child->name, "spwPort", 7)))
    {
        if((status = GetXMLAttributeValueByName(child, "type", attrData, ATTR_PORT_SIZE)) != 0)
        {
            return -1;
        }
    }
    else if(!(strncmp((char*)child->name, "portConfig", 10)))
    {
        if((status = GetXMLAttributeValueByIndex(child, 1, attrData, ATTR_PORT_SIZE)) != 0)
        {
            return -1;
        }
    }
    else
    {
        deviceError = portIndex;
        return WRONG_PORT_SPW;
    }
    
    if(strncmp(attrData, "usb", 3) == 0)
    {
        pPort->portType = SPW_USB_PORT;
        pPort->portPhyHeaderOffsetTC = 5;
        pPort->portPhyHeaderOffsetTM = 4;
    }
    else if(strncmp(attrData, "mk2", 3) == 0)
    {
        pPort->portType = SPW_USB_MK2_PORT;
        pPort->portPhyHeaderOffsetTC = 5;
        pPort->portPhyHeaderOffsetTM = 4;
    }
    else if(strncmp(attrData, "pci", 3) == 0)
    {
        pPort->portType = SPW_PCI_PORT;
        pPort->portPhyHeaderOffsetTC = 4;
        pPort->portPhyHeaderOffsetTM = 4;
    }
    else
    {
        return WRONG_PORT_SPW_TYPE;
    }

    /* get link */
    if((status = GetXMLAttributeValueByName(child, "link", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return -1;
    }
    transmitLink = (int)strtol (attrData, NULL, 0);

    /* get writingPort */
    if((status = GetXMLAttributeValueByName(child, "writingPort", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return -1;
    }
    writePort = (int)strtol (attrData, NULL, 0);

    /* get readingPorts */
    GetXMLNumChildren(child, &numberOfReadingPorts);

    for(idx=0; idx<numberOfReadingPorts; ++idx)
    {
        if((status = GetXMLChildElementByIndex (child, idx, &greatchild)) != 0)
        {
            return status;
        }
        if((status = GetXMLAttributeValueByName(greatchild, "port", attrData, ATTR_PORT_SIZE)) != 0)
        {
            return status;
        }
        readPort[idx] = (int)strtol (attrData, NULL, 0);
        greatchild = NULL;
    }

    pPort->config.spw.link = transmitLink;
    pPort->config.spw.protocolID = 0x02;	/* SpaceWire - CCSDS packet transfer protocol */
    pPort->config.spw.writingPort = writePort;
    pPort->config.spw.numberOfReadingPorts = numberOfReadingPorts;
    for(idx=0; idx<numberOfReadingPorts; ++idx)
        pPort->config.spw.readingPorts[idx] = readPort[idx];
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}

static int ParseSpWAuxPort (const char * portConfigFile, const char * relativePath,
        portConfig * ports, unsigned int portIndex)
{
    unsigned int numberOfChildren;
    char attrData[32];
    int status;
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL, child = NULL;
    char fullPath[MAX_STR_LEN];
	
    /* look for file */
    if(fileExists(portConfigFile, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
    	strncpy(portFileError, portConfigFile, 65);
        return PORT_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return PORT_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        return PORT_FILE_PARSING_ERROR;
    }
    /* NOTE: "name" is not used */

    GetXMLNumChildren(root, &numberOfChildren);
    if(numberOfChildren != 1)
    {
        return ONE_PORT_PER_FILE;
    }

    /* get xml format file */ 
    if((status = GetXMLChildElementByIndex (root, 0, &child)) != 0)
    {
        return status;
    }
    /* check if port file is spwTCPort */
    if((strncmp((char*)child->name, "spwTCPort", 9)) &&
        (strncmp((char*)child->name, "portConfig", 10)))
    {
        deviceError = portIndex;
        return WRONG_PORT_SPW_TC;
    }

    /* get spw port ref */
    if((status = GetXMLAttributeValueByName(child, "spwPortRef", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    ports[portIndex].config.spw.channel = atoi(attrData);

    switch(ports[ports[portIndex].config.spw.channel].portType)
    {
        case SPW_USB_MK2_PORT:
            ports[portIndex].config.spw.link = ports[ports[portIndex].config.spw.channel].config.spw.link;
            break;

        case SPW_USB_PORT: case SPW_PCI_PORT:
            break;

        default:
            deviceError = portIndex;
            return SPW_REF_NOT_FOUND;
    }
    ports[portIndex].portTypeAux = ports[ports[portIndex].config.spw.channel].portType;
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}

static int ParseSerialPort (const char * portConfigFile, const char * relativePath,
        portConfig * pPort, serialConfig * pSerial, unsigned int portIndex)
{
    unsigned int numberOfChildren, numAttributes;
    char attrData[32];
    int status = 0, portNumber = -1;
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL, child = NULL, greatchild = NULL;
    char fullPath[MAX_STR_LEN];
    
    pPort->config.uart.portNum = -1;
    memset(pPort->config.uart.portName, 0, MAX_DEV_NAME_LEN);
	
    /* look for file */
    if(fileExists(portConfigFile, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
    	strncpy(portFileError, portConfigFile, 65);
        return PORT_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return PORT_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        return PORT_FILE_PARSING_ERROR;
    }
    /* NOTE: "name" is not used */

    GetXMLNumChildren(root, &numberOfChildren);
    if(numberOfChildren != 1)
    {
        return ONE_PORT_PER_FILE;
    }

    /* get xml format file */ 
    if((status = GetXMLChildElementByIndex (root, 0, &child)) != 0)
    {
        return status;
    }
    /* check if port file is uartPort */
    if((strncmp((char*)child->name, "uartPort", 8)) &&
            (strncmp((char*)child->name, "portConfig", 10)))
    {
        deviceError = portIndex;
        return WRONG_PORT_UART;
    }
    
    /* get deviceNumber */ 
    if((status = GetXMLAttributeValueByName(child, "number", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    char * endptr;
    portNumber = (int)strtol (attrData, &endptr, 0);
    if (endptr == attrData) /* no conversion performed */
        snprintf(pPort->config.uart.portName, MAX_DEV_NAME_LEN, "%s", attrData);
    else
        pPort->config.uart.portNum = portNumber;

    /* get baudRate */ 
    if((status = GetXMLAttributeValueByName(child, "baudRate", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    pSerial->baudRate = strtol (attrData, NULL, 0);

    /* get parity */
    if((status = GetXMLAttributeValueByName(child, "parity", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    if(strncmp(attrData, "ODD", 3) == 0)
        pSerial->parity = 1;
    else if(strncmp(attrData, "EVEN", 4) == 0)
        pSerial->parity = 2;
    else if(strncmp(attrData, "MARK", 4) == 0)
        pSerial->parity = 3;
    else if(strncmp(attrData, "SPACE", 5) == 0)
        pSerial->parity = 4;
    else if(strncmp(attrData, "NO", 2) == 0)
        pSerial->parity = 0;
    else
        return WRONG_PARITY_VALUE;

    /* get dataBits */ 
    if((status = GetXMLAttributeValueByName(child, "dataBits", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    pSerial->dataBits = (int)strtol (attrData, NULL, 0);

    /* get stopBits */ 
    if((status = GetXMLAttributeValueByName(child, "stopBits", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    pSerial->stopBits = (int)strtol (attrData, NULL, 0);

    if((status = SearchXMLAttributeValueByName(child, "flowControl", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    if(attrData[0] != '\0')
    {
        if(strncmp(attrData, "hw", 2) == 0)
        {
            pSerial->control = HW;
        }
        else if(strncmp(attrData, "rts_cts", 7) == 0)
        {
            pSerial->control = RTS_CTS;
        }
        else if(strncmp(attrData, "xon_xoff", 8) == 0)
        {
            pSerial->control = XON_OFF;
        }
#if(defined _WIN32 || __CYGWIN__)
        else if(strncmp(attrData, "dtr_dsr", 7) == 0)
        {
            pSerial->control = DTR_DSR;
        }
#endif
        else
        {
            return WRONG_FLOW_CONTROL;
        }
    }
    else
    {
        pSerial->control = HW; //default no flow control
    }

    /* get port protocol handle */ 
    if((status = GetXMLChildElementByIndex (child, 0, &greatchild)) != 0)
    {
        return status;
    }
    
    if((status = GetXMLNumAttributes(child, &numAttributes)) != 0)
    {
        return status;
    }
    if(numAttributes == 7)
    {
        /* get portName */ 
        if((status = GetXMLAttributeValueByName(child, "portName", attrData,
                ATTR_PORT_SIZE)) != 0)
        {
            return status;
        }
        snprintf(pPort->config.uart.portName, MAX_DEV_NAME_LEN, "%s", attrData);
    }
    
    status = ParsePortProtocol(greatchild, attrData, &pPort->ptcl, 
            &pPort->portPhyHeaderOffsetTC, &pPort->portPhyHeaderOffsetTM);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    
    if(status != 0)
    {
        pPort->config.uart.portNum = -1;
        memset(pPort->config.uart.portName, 0, MAX_DEV_NAME_LEN);
    }
    return status;
}

static int ParseTCPSocketPort (const char * portConfigFile,
        const char * relativePath, portConfig * pPort, unsigned int portIndex)
{
    unsigned int numberOfChildren;
    char attrData[ATTR_PORT_SIZE];
    int status;
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL, child = NULL, greatchild = NULL;
    char fullPath[MAX_STR_LEN];
	
    /* look for file */
    if(fileExists(portConfigFile, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
    	strncpy(portFileError, portConfigFile, 65);
        return PORT_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return PORT_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        return PORT_FILE_PARSING_ERROR;
    }
    /* NOTE: "name" is not used */

    GetXMLNumChildren(root, &numberOfChildren);
    if(numberOfChildren != 1)
    {
        return ONE_PORT_PER_FILE;
    }

    /* get xml format file */ 
    if((status = GetXMLChildElementByIndex (root, 0, &child)) != 0)
    {
        return status;
    }
    /* check if port file is TCPsocketCliPort or TCPsocketSrvPort */
    if((strncmp((char*)child->name, "TCPsocketCliPort", 16)) &&
            (strncmp((char*)child->name, "TCPsocketSrvPort", 16)) &&
            (strncmp((char*)child->name, "portConfig", 10)))
    {
        deviceError = portIndex;
        return WRONG_PORT_SOCKET;
    }
    if(strncmp((char*)child->name, "portConfig", 10) == 0)
    {
        if((status = GetXMLAttributeValueByName(child, "type",
                attrData, ATTR_PORT_SIZE)) != 0)
            return status;
        
        if(((strncmp(attrData, "gss:GSSIfaceTCPSocketCliPort", 24)) &&
            (pPort->portType == TCP_SOCKET_CLI_PORT)) ||
            ((strncmp(attrData, "gss:GSSIfaceTCPSocketSrvPort", 24)) &&
            (pPort->portType == TCP_SOCKET_SRV_PORT)))
        {
            deviceError = portIndex;
            return WRONG_SOCKET_ROLE;
        }
    }
    else if(((strncmp((char*)child->name, "TCPsocketCliPort", 16)) &&
            (pPort->portType == TCP_SOCKET_CLI_PORT)) ||
            ((strncmp((char*)child->name, "TCPsocketSrvPort", 16)) &&
            (pPort->portType == TCP_SOCKET_SRV_PORT)))
    {
        deviceError = portIndex;
        return WRONG_SOCKET_ROLE;
    }
    
    memset(pPort->config.socket.localIp, 0, 16);
    memset(pPort->config.socket.remoteIp, 0, 16);
    /* server: get local address & port number */
    if(pPort->portType == TCP_SOCKET_SRV_PORT)
    {
        if((status = GetXMLAttributeValueByName(child, "localIp",
                attrData, ATTR_PORT_SIZE)) != 0)
        {
            return status;
        }
        snprintf(pPort->config.socket.localIp, 16, "%s", attrData);
        if((status = GetXMLAttributeValueByName(child, "localPort",
                attrData, ATTR_PORT_SIZE)) != 0)
        {
            return status;
        }
        pPort->config.socket.localPort = (int)strtol (attrData, NULL, 0);
    }
    /* client: get remote address & port number */
    else if(pPort->portType == TCP_SOCKET_CLI_PORT)
    {
        if((status = GetXMLAttributeValueByName(child, "remoteIp",
                attrData, ATTR_PORT_SIZE)) != 0)
        {
            return status;
        }
        snprintf(pPort->config.socket.remoteIp, 16, "%s", attrData);
        if((status = GetXMLAttributeValueByName(child, "remotePort",
                attrData, ATTR_PORT_SIZE)) != 0)
        {
            return status;
        }
        pPort->config.socket.remotePort = (int)strtol (attrData, NULL, 0);
    }
    
    /* get port protocol handle */
    if((status = GetXMLChildElementByIndex (child, 0, &greatchild)) != 0)
    {
        return status;
    }
    status = ParsePortProtocol(greatchild, attrData, &pPort->ptcl, 
            &pPort->portPhyHeaderOffsetTC, &pPort->portPhyHeaderOffsetTM);
			
    xmlFreeDoc(doc);
    xmlCleanupParser();
    
    pPort->config.socket.socketHdl = -1;
    return status;
}

static int ParseUDPSocketPort (const char * portConfigFile,
        const char * relativePath, portConfig * pPort, unsigned int portIndex)
{
    unsigned int numberOfChildren;
    char attrData[ATTR_PORT_SIZE];
    int status;
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL, child = NULL;
    char fullPath[MAX_STR_LEN];
	
    /* look for file */
    if(fileExists(portConfigFile, relativePath, fullPath, MAX_STR_LEN) == NULL)
    {
    	strncpy(portFileError, portConfigFile, 65);
        return PORT_FILE_NOT_FOUND;
    }
    /* Open Document */
    doc = xmlParseFile(fullPath);
    if (doc == NULL)
    {
        return PORT_FILE_PARSING_ERROR;
    }
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
        return PORT_FILE_PARSING_ERROR;
    }
    /* NOTE: "name" is not used */

    GetXMLNumChildren(root, &numberOfChildren);
    if(numberOfChildren != 1)
    {
        return ONE_PORT_PER_FILE;
    }

    /* get xml format file */ 
    if((status = GetXMLChildElementByIndex (root, 0, &child)) != 0)
    {
        return status;
    }
    /* check if port file is UDPsocketPort */
    if((strncmp((char*)child->name, "UDPsocketPort", 13)) &&
            (strncmp((char*)child->name, "portConfig", 10)))
    {
        deviceError = portIndex;
        return WRONG_PORT_SOCKET;
    }
	
    /* get local address & port number */
    memset(pPort->config.socket.localIp, 0, 16);
    if((status = GetXMLAttributeValueByName(child, "localIp",
            attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    snprintf(pPort->config.socket.localIp, 16, "%s", attrData);
    if((status = GetXMLAttributeValueByName(child, "localPort",
            attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    pPort->config.socket.localPort = (int)strtol (attrData, NULL, 0);
	
    /* get remote address & port number */ 
    memset(pPort->config.socket.remoteIp, 0, 16);
    if((status = GetXMLAttributeValueByName(child, "remoteIp",
            attrData, ATTR_PORT_SIZE)) != 0)
    {
            return status;
    }
    snprintf(pPort->config.socket.remoteIp, 16, "%s", attrData);
    if((status = GetXMLAttributeValueByName(child, "remotePort",
            attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    pPort->config.socket.remotePort = (int)strtol (attrData, NULL, 0);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    
    pPort->config.socket.socketHdl = -1;
    return status;
}

static int ParsePortProtocol(xmlNodePtr ptclRoot, char attrData[32],
        protocolConfig * pPtcl, unsigned int * pPortPhyHeaderOffsetTM,
        unsigned int * pPortPhyHeaderOffsetTC)
{
    int status = 0;
    unsigned int numberOfChildren;
    
    /* get reference unit (bytes / bits) */
    if((status = GetXMLAttributeValueByName(ptclRoot, "unit", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    if(strncmp(attrData, "bytes", 5) == 0)
    {
        pPtcl->refUnit = 8;
    }
    else if(strncmp(attrData, "bits", 4) == 0)
    {
        pPtcl->refUnit = 1;
    }

    /* get power (no / 2 / 2_with_0) */
    if((status = GetXMLAttributeValueByName(ptclRoot, "power", attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    if(strncmp(attrData, "no", 2) == 0)
    {
        pPtcl->refPower = PTCL_POWER_NONE;
    }
    else if(strncmp(attrData, "2_with_0", 8) == 0)
    {
        pPtcl->refPower = PTCL_POWER_BASE_2_WITH_0;
    }
    else if(strncmp(attrData, "2", 1) == 0)
    {
        pPtcl->refPower = PTCL_POWER_BASE_2;
    }

    /* get constant size */
    if((status = GetFieldInBits(ptclRoot, "constSize",
            &pPtcl->constSizeInBits, attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    /* get offset of size field */
    if((status = GetFieldInBits(ptclRoot, "sizeFieldOffset",
            (int *)&pPtcl->sizeFieldOffsetInBits, attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    /* get length of size field */
    if((status = GetFieldInBits(ptclRoot, "sizeFieldTrim",
            (int *)&pPtcl->sizeFieldTrimInBits, attrData, ATTR_PORT_SIZE)) != 0)
    {
        return status;
    }
    
    memset(pPtcl->syncPattern, 0, 8);
    /* get sync pattern if exists */
    if((status = GetXMLNumChildren(ptclRoot, &numberOfChildren)) != 0)
    {
        return status;
    }
    if(numberOfChildren == 4)
    {
        unsigned int idx = 0;
        int hex;
        char * realData = attrData;
        if((status = GetXMLChildAttributeLengthByName(ptclRoot, "syncPattern",
                "hexValue", &pPtcl->syncPatternLength)) != 0)
        {
            return status;
        }
        if((status = GetXMLChildAttributeValueByName(ptclRoot, "syncPattern",
                "hexValue", attrData, 16)) != 0)
        {
            return status;
        }
        /* check if hexValue begins with "0x" */
        if((attrData[0] == '0') && (attrData[1] == 'x'))
        {
            realData = &attrData[2];
            pPtcl->syncPatternLength -= 2;
        }
        /* get real byte length and check limits */
        pPtcl->syncPatternLength/=2; //convert ascii to hex
        if(pPtcl->syncPatternLength > (pPtcl->sizeFieldOffsetInBits/8))
        {
            deviceError = pPtcl->syncPatternLength;
            pPtcl->syncPatternLength = 0;
            return WRONG_SYNC_PATTERN_LEN;
        }
        if(pPtcl->syncPatternLength > MAX_PHY_HDR)
        {
            deviceError = pPtcl->syncPatternLength;
            pPtcl->syncPatternLength = 0;
            return BIG_SYNC_PATTERN_LEN;
        }
        
        /* get data */
        for(idx=0; idx<pPtcl->syncPatternLength; ++idx)
        {
            sscanf (&realData[2*idx], "%02X", (int *)&hex);
            pPtcl->syncPattern[idx] = (unsigned char)(hex & 0xFF);
        }
        *pPortPhyHeaderOffsetTM =
                pPtcl->syncPatternLength+pPtcl->sizeFieldTrimInBits/8;
        if(pPtcl->sizeFieldTrimInBits % 8)
          *pPortPhyHeaderOffsetTM = *pPortPhyHeaderOffsetTM+1;  
        *pPortPhyHeaderOffsetTC = *pPortPhyHeaderOffsetTM;
    }
    return 0;
}
