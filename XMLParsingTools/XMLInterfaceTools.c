/**
 * \file	XMLInterfaceTools.c
 * \brief	functions for parsing XML export settings (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		18/09/2013 at 15:32:15
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <string.h>
#include "XMLTools.h"			/* GetXMLValueByTag, ... */
#include "XMLPortTools.h"		/* ConfigurePortXML */
					/* formatField, boolVar, portConfig */

enum {
    WRONG_INTERFACE_ID = -101,
    WRONG_INTERFACE_TYPE = -102,
    WRONG_INTERFACE_IO_TYPE = -103,
    MUST_BE_IN_IO_TYPE = -104
};

static int portError;

int ParseInterface(xmlNodePtr element, portConfig * ports,
        serialConfig * serials, char attrData[ATTR_SIZE], int index,
        const char * relativePath)
{
    int status = 0;
    portError = 0;

    /* check if Interface Number is the same as in tag */
    if((status = GetXMLAttributeValueByName (element, "id", attrData, ATTR_SIZE)) != 0)
    {
        portError = index;
        return status;
    }
    if((status = GetXMLAttributeValueByName (element, "name", ports[index].name,
            MAX_PORT_NAME_LEN)) != 0)
    {
        portError = index;
        return status;
    }
    if(index != atoi(attrData))
    {
        portError = index;
        return WRONG_INTERFACE_ID;
    }
    /* TODO: use "ioType", as currently ports which aren't i/o are configured,
     * but can't send or receive because of GSS checkings */

    /* get xml port type */
    ports[index].portType = NO_PORT;
    ports[index].portTypeAux = NO_PORT;
    ports[index].portPhyHeaderOffsetTM = 0;
    ports[index].portPhyHeaderOffsetTC = 0;
    if((status = GetXMLAttributeValueByName (element, "ifType", attrData, ATTR_SIZE)) != 0)
    {
        portError = index;
        return status;
    }
    if(strncmp(attrData, "SpW_TC", 6) == 0)
        ports[index].portType = SPW_TC_PORT;
    else if(strncmp(attrData, "SpW_Error", 9) == 0)
        ports[index].portType = SPW_ERROR_PORT;
    else if(strncmp(attrData, "SpW", 3) == 0)
        ports[index].portType = SPW_USB_PORT;
    else if(strncmp(attrData, "uart", 4) == 0)
        ports[index].portType = UART_PORT;
    else if(strncmp(attrData, "socketCli", 9) == 0)
        ports[index].portType = SOCKET_CLI_PORT;
    else if(strncmp(attrData, "socketSrv", 9) == 0)
        ports[index].portType = SOCKET_SRV_PORT;
    else if(strncmp(attrData, "dummy", 5) == 0)
        ports[index].portType = DUMMY_PORT;
    else
    {
        portError = index;
        return WRONG_INTERFACE_TYPE;
    }

    /* get xml port type */
    if((status = GetXMLAttributeValueByName (element, "ioType", attrData, ATTR_SIZE)) != 0)
    {
        portError = index;
        return status;
    }
    if(strncmp(attrData, "in_out", 6) == 0)
        ports[index].ioType = INOUT_PORT;
    else if(strncmp(attrData, "in", 2) == 0)
        ports[index].ioType = IN_PORT;
    else if(strncmp(attrData, "out", 3) == 0)
        ports[index].ioType = OUT_PORT;
    else
    {
        ports[index].portType = NO_PORT;
        portError = index;
        return WRONG_INTERFACE_IO_TYPE;
    }
    /* if dummy port, don't parse it */
    if((ports[index].ioType != IN_PORT) &
            ((ports[index].portType == DUMMY_PORT) ||
            (ports[index].portType == SPW_TC_PORT) ||
            (ports[index].portType == SPW_ERROR_PORT)))
    {
        ports[index].portType = NO_PORT;
        portError = index;
        return MUST_BE_IN_IO_TYPE;
    }
    
    /* if dummy port, don't parse it */
    if(ports[index].portType == DUMMY_PORT)
    {
        return 0;
    }
	
    /* get xml port config file */
    if((status = GetXMLValueFromAttributeOrHRefTag(element, "ifConfig",
            attrData, ATTR_SIZE)) != 0)
    {
        ports[index].portType = NO_PORT;
        portError = index;
        return status;
    }
    /* parse port config file */
    if((status = ParsePort(attrData, ports, serials, index, relativePath)) != 0)
    {
        ports[index].portType = NO_PORT;
        portError = -1;
        return status;
    }
    return 0;
}

void DisplayParseInterfaceError (int status, portConfig * pPort, char * msg,
	unsigned int maxMsgSize)
{
    if(portError == -1)
    {
        DisplayParsePortError (status, pPort, msg, maxMsgSize);
    }
    else
    {
        unsigned int len = snprintf(msg, 31, "Error in configuration files: ");
        switch (status)
        { 		  
            case WRONG_INTERFACE_ID:
                snprintf(&msg[len], maxMsgSize-len, "Wrong Interface %d id",
                        portError);
                break;  

            case WRONG_INTERFACE_TYPE:
                snprintf(&msg[len], maxMsgSize-len, "Wrong Interface %d type. "
                        "Only valid \"SpW\", \"uart\", \"dummy\""
                        "\"socketSrvPort\" or \"socketSrvPort\"", portError);
                break;

            case WRONG_INTERFACE_IO_TYPE:
                snprintf(&msg[len], maxMsgSize-len, "Wrong interface %d IO type. "
                        "Only \"in\", \"out\" or \"in_out\"", portError);
                break;

            case MUST_BE_IN_IO_TYPE:
                snprintf(&msg[len], maxMsgSize-len, "Wrong Interface %d type. "
                        "For ifType=\"dummy\", ifType=\"SpW_TC\" and \"ifType="
                        "\"SpW_Error\", only ioType=\"in\" is allowed",
                        portError);
                break;

            default:
                XMLerrorInfo(status, &msg[len], maxMsgSize-len);
                break;
        }
    }
    return;
}