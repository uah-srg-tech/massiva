/**
 * \file	rawSpWusb.c
 * \brief	functions for write and read data in SpaceWire port (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		05/03/2012 at 15:36:42
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include "configSpWusb.h"				/* NOT_USB_DEV ? */
#include <stdio.h>					/* sprintf */
#include "../definitions.h"				/* MAX_PACKET_SIZE */

#ifndef	NOT_USB_DEV
#if !defined (_WIN32) && defined (__CYGWIN__)
#define _WIN32	//for spw_usb_api.h definitions
#endif

#include "spw_usb_api.h"
#include "spw_config_library.h"
#endif /* NOT_USB_DEV */

enum {
    DONT_WAIT = 0,
    WAIT = 1
};

enum {
    COULD_NOT_SET_TICK_FREQ = -101,
    COULD_NOT_ENABLE_AUTO_TICK = -102,
    COULD_NOT_DISABLE_AUTO_TICK = -103,
    COULD_NOT_PERFORM_TICK = -104,
    COULD_NOT_ENABLE_EXTERNAL_TC = -105,
    COULD_NOT_DISABLE_EXTERNAL_TC = -106,
    SPW_NO_EOP = -107,
    SPW_EEP = -108,
};

int ReadRawSpWusb(unsigned char * receivedPacket, int * pLength, void * hDevice)
{
    int status = 0;
#ifdef NOT_USB_DEV
    UNUSED(receivedPacket);
    UNUSED(pLength);
    UNUSED(hDevice);
#else
    USB_SPACEWIRE_STATUS receiveResult;	//Result of receive transfer
    USB_SPACEWIRE_ID receiveIdentifier;	//Identifier of the read
    USB_SPACEWIRE_PACKET_PROPERTIES properties;
    
    if(USBSpaceWire_WaitOnReadPacketAvailable(hDevice, -1) == 0)
        return 0;
	
    //Receive data, wait until transfer has been completed or timeout
    receiveResult = USBSpaceWire_ReadPackets(hDevice, receivedPacket,
            *pLength, 1, WAIT, &properties, &receiveIdentifier);
    USBSpaceWire_FreeRead(hDevice, receiveIdentifier);

    *pLength = USBSpaceWire_GetReadLength(&properties, 0);
    status = *pLength;
	
    if (receiveResult != TRANSFER_SUCCESS)
    {
        status = -receiveResult;
    }
    else if(properties.eop == SPACEWIRE_USB_NO_EOP)
    {
        status = SPW_NO_EOP;
    }
    else if(properties.eop == SPACEWIRE_USB_EEP)
    {
        status = SPW_EEP;
    }
#endif /* NOT_USB_DEV */
    return status;
}

int WriteRawSpWusb(const unsigned char * sendPacket, int length, void * hDevice)
{
    int status = 0;
#ifdef NOT_USB_DEV
    UNUSED(sendPacket);
    UNUSED(length);
    UNUSED(hDevice);
#else
    USB_SPACEWIRE_STATUS sendResult;	//The result of the send operation
    USB_SPACEWIRE_ID sendIdentifier;	//The identifier for the send

    //Send the packet and wait on it completing
    sendResult = USBSpaceWire_SendPacket(hDevice, (unsigned char *)sendPacket,
            length, WAIT, &sendIdentifier);

    if (sendResult != TRANSFER_SUCCESS)
    {
        status = -sendResult;
    }
    status = length;
#if 0
    //Send the packet and wait for completing
    if ((sendResult != TRANSFER_STARTED) && (sendResult != TRANSFER_SUCCESS))
    {
        USBSpaceWire_FreeSend(hDevice, sendIdentifier);
        return sendResult;
    }
	
    while(sendResult == TRANSFER_STARTED)
    {
        sendResult = USBSpaceWire_GetSendStatus(hDevice, sendIdentifier);
    }	
#endif
    //Free send
    USBSpaceWire_FreeSend(hDevice, sendIdentifier);
#endif /* NOT_USB_DEV */
    return status;
}

void RawSpWusbRWError(int error, char * msg, int msgSize)
{
    int realError = error;
    if(realError < 100)
        realError = -error;
    switch (realError)
    {
#ifdef NOT_USB_DEV
    UNUSED(error);
    UNUSED(msg);
    UNUSED(msgSize);
#else
	case TRANSFER_NOT_STARTED:
            snprintf(msg, msgSize, "Transfer Not Started");
            break;
		
	case TRANSFER_STARTED:
            snprintf(msg, msgSize, "Transfer Started (In progress)");
            break;
		
	case TRANSFER_SUCCESS:
            snprintf(msg, msgSize, "Transfer Success");
            break;
		
	case TRANSFER_ERROR_DISCONNECT:
            snprintf(msg, msgSize, "Transfer Error: Disconnect");
            break;
		
	case TRANSFER_ERROR_PARITY:
            snprintf(msg, msgSize, "Transfer Error: Parity");
            break;
		
	case TRANSFER_ERROR_UNKNOWN:
            snprintf(msg, msgSize, "Transfer Error: Unknown");
            break;
		
	case TRANSFER_ERROR_TIMEOUT:
            snprintf(msg, msgSize, "Transfer Error: Timeout");
            break;
		
	case TRANSFER_ERROR_LINK_NOT_STARTED:
            snprintf(msg, msgSize, "Transfer Error: Link not started");
            break;
		
	case TRANSFER_ERROR_USER_BUFFER_LOCK_FAILED:
            snprintf(msg, msgSize, "Transfer Error: Could not lock user "
                    "buffer for DMA transfer");
            break;
		
	case TRANSFER_ERROR_LINK_ALREADY_IN_USE:
            snprintf(msg, msgSize, "Transfer Error: Could not start "
                    "transfer as transfer already in progress");
            break;
		
	case TRANSFER_ERROR_RECEIVE_BUFFER_FULL:
            snprintf(msg, msgSize, "Transfer Error: Could not perform read "
                    "as buffer full");
            break;
		
	case TRANSFER_ERROR_OUT_OF_MEMORY:
            snprintf(msg, msgSize, "Transfer Error: Could not perform "
                    "transfer as out of memory");
            break;
		
	case TRANSFER_ERROR_NOT_FOUND:
            snprintf(msg, msgSize, "Transfer Error: Could not find the "
                    "specified operation");
            break;
		
	case TRANSFER_ERROR_USB_ERROR:
            snprintf(msg, msgSize, "Transfer Error: USB Error");
            break;
		
	case TRANSFER_ERROR_NOT_REGISTERED_TO_RECEIVE:
            snprintf(msg, msgSize, "Transfer Error: Not registered to receive");
            break;
#endif /* NOT_USB_DEV */
		
        case COULD_NOT_SET_TICK_FREQ:
            snprintf(msg, msgSize, "Couldn't set tick frequency");
            break;
		
        case COULD_NOT_ENABLE_AUTO_TICK:
            snprintf(msg, msgSize, "Couldn't enable auto tick");
            break;
		
        case COULD_NOT_DISABLE_AUTO_TICK:
            snprintf(msg, msgSize, "Couldn't disable auto tick");
            break;
		
        case COULD_NOT_PERFORM_TICK:
            snprintf(msg, msgSize, "Couldn't perform single tick");
            break;
		
        case COULD_NOT_ENABLE_EXTERNAL_TC:
            snprintf(msg, msgSize, "Couldn't enable external timecode "
                    "selection");
            break;
        case COULD_NOT_DISABLE_EXTERNAL_TC:
            snprintf(msg, msgSize, "Couldn't disable external timecode "
                    "selection");
            break;
		
        case SPW_NO_EOP:
            snprintf(msg, msgSize, "Packet received with no End of Packet");
            break;
		
        case SPW_EEP:
            snprintf(msg, msgSize, "Packet received with Error End of Packet");
            break;
		
	default:
            snprintf(msg, msgSize, "Unknown Result state (%d)!", realError);
            break;
    }
    return;
}

int PeriodicalTickInsSpWusb (void * hDevice, int enable, unsigned int hertz)
{
#ifdef NOT_USB_DEV
    UNUSED(hDevice);
    UNUSED(enable);
    UNUSED(hertz);
#else
    if(enable)
    {
        U32 rtr_clk_freq = USBSpaceWire_TC_GetClockFrequency(hDevice);
        if (!USBSpaceWire_TC_SetAutoTickInFrequency(hDevice, 
                rtr_clk_freq / hertz))
        {
            return COULD_NOT_SET_TICK_FREQ;
        }
    }

    if (!USBSpaceWire_TC_EnableAutoTickIn(hDevice, enable, 0))
    {
        if(enable)
            return COULD_NOT_ENABLE_AUTO_TICK;
        else
            return COULD_NOT_DISABLE_AUTO_TICK;
    }
#endif /* NOT_USB_DEV */
    return 0;
}

int SingleTickInSpWusb (void * hDevice, unsigned char value,
	unsigned char external)
{
#ifdef NOT_USB_DEV
    UNUSED(hDevice);
    UNUSED(value);
    UNUSED(external);
#else
    if(external)
    {
        if (!USBSpaceWire_TC_EnableExternalTimecodeSelection(hDevice, 1))
        {
            return COULD_NOT_ENABLE_EXTERNAL_TC;
        }
    }

    if (!USBSpaceWire_TC_PerformTickIn(hDevice, value))
    {
        return COULD_NOT_PERFORM_TICK;
    }

    if(external)
    {
        if (!USBSpaceWire_TC_EnableExternalTimecodeSelection(hDevice, 0))
        {
            return COULD_NOT_DISABLE_EXTERNAL_TC;
        }
    }
#endif /* NOT_USB_DEV */
    return 0;
}

void EnableDisableEEPSpWusb(void * hDevice, char enableDisable)
{
#ifdef NOT_USB_DEV
    UNUSED(hDevice);
    UNUSED(enableDisable);
#else
    USBSpaceWire_EnableSendEEPs(hDevice, enableDisable);
#endif /* NOT_USB_DEV */
    return;
}

int FlushSpWusb (unsigned char * rxPacket, unsigned int * pLength, void * hDevice)
{
#ifdef NOT_USB_DEV
    UNUSED(rxPacket);
    UNUSED(pLength);
    UNUSED(hDevice);
#else
    int status = 0, counter = 0;
    do {
        status = USBSpaceWire_WaitOnReadPacketAvailable(hDevice, 0);
        if(status)
        {
            ReadRawSpWusb(rxPacket, (int *)pLength, hDevice);
            counter++;
        }
    } while(status);
#endif /* NOT_USB_DEV */
    return 0;
}