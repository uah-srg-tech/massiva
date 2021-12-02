/**
 * \file	rawSpWpci.c
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		30/07/2013 at 16:09:38
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include "configSpWpci.h"				/* NOT_PCI_DEV ? */
#include <stdio.h>					/* sprintf */
#include "../definitions.h"				/* UNUSED */

#ifndef	NOT_PCI_DEV
#include "SpaceWirePCI2.h"
static int timecodeValue = 0;
enum {
    WAIT = TRUE,
    DONT_WAIT = FALSE
};
#endif /* NOT_PCI_DEV */

enum {
	PCI_LINK_NOT_STARTED = -101
};

int ReadRawSpWpci(unsigned char * receivedPacket, int * pLength, void * hDevice,
	unsigned char transmitLink)
{
    int status = 0;
#ifdef NOT_PCI_DEV
    UNUSED(receivedPacket);
    UNUSED(pLength);
    UNUSED(hDevice);
    UNUSED(transmitLink);
#else
    while(status != 1)
    {
        status = PciSpaceWire_IsValidHandle(hDevice);
        if (status == 0)
        {
            break;
        }
        status = PciSpaceWire_ReceiveDataPending(hDevice, transmitLink-1);
    }
    U32 length = *pLength;
    PCI_SPACEWIRE_TRANSFER_STATUS receiveResult;

    //adjust to 4-byte words
    length = 4*(length/4)+(length%4);

    while(PciSpaceWire_WaitLinkStarted(hDevice, transmitLink-1) == 0)
    {
        if(pciClosed)
            return 0;
    }

    while(PciSpaceWire_ReceiveDataPending(hDevice, transmitLink-1) == 0)
    {
        if(pciClosed)
            return 0;
    }

    if(pciClosed)
        return 0;

    receiveResult = PciSpaceWire_ReadPacket(hDevice, transmitLink-1,
            receivedPacket, length, 1, DONT_WAIT);
    do {
        receiveResult = PciSpaceWire_GetReadStatus(hDevice, transmitLink-1);
    } while (receiveResult == TRANSFER_STARTED);

    if (receiveResult != TRANSFER_SUCCESS)
    {
        return -receiveResult;
    }

    *pLength = PciSpaceWire_GetLastReadSize(hDevice, transmitLink-1);
    status = *pLength;
#endif /* NOT_PCI_DEV */
    return status;
}

int WriteRawSpWpci(const unsigned char * sendPacket, int length, void * hDevice,
	unsigned char transmitLink)
{
    int status = 0;
#ifdef NOT_PCI_DEV
    UNUSED(sendPacket);
    UNUSED(length);
    UNUSED(hDevice);
    UNUSED(transmitLink);
#else
    PCI_SPACEWIRE_TRANSFER_STATUS sendResult;

    while(PciSpaceWire_WaitLinkStarted(hDevice, transmitLink-1) == 0);

    sendResult = PciSpaceWire_WriteLink(hDevice, transmitLink-1,
            (unsigned char *)sendPacket, length, length, WAIT);
    if ((sendResult != TRANSFER_STARTED) && (sendResult != TRANSFER_SUCCESS))
    {
        status = -sendResult;
    }
    status = length;
#endif /* NOT_PCI_DEV */
    return status;
}

void RawSpWpciRWError(int error, char * msg, int msgSize, void * hDevice,
	unsigned char transmitLink)
{
#ifdef NOT_PCI_DEV
    UNUSED(error);
    UNUSED(msg);
    UNUSED(msgSize);
    UNUSED(hDevice);
    UNUSED(transmitLink);
#else
    int realError = error;
    if(realError < 100)
        realError = -error;
    switch (realError)
    {
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
		
	case TRANSFER_ERROR_RECEIVED_EEP:
            snprintf(msg, msgSize, "Transfer Error: Received EEP");
            break;
		
	case TRANSFER_ERROR_BAD_TRANSFER_FUNCTION:
            snprintf(msg, msgSize, "Transfer Error: Bad transfer function");
            break;
		
	case TRANSFER_ERROR_NO_EOP:
            snprintf(msg, msgSize, "Transfer Error: No EOP");
            break;
		
	case TRANSFER_ERROR_NO_RECEIVE_BUFFERS:
            snprintf(msg, msgSize, "Transfer Error: No receive buffers");
            break;
		
	case TRANSFER_ERROR_NO_CONFIG_BUFFERS:
            snprintf(msg, msgSize, "Transfer Error: No config buffers");
            break;
		
	case TRANSFER_ERROR_NO_TRANSMIT_BUFFERS:
            snprintf(msg, msgSize, "Transfer Error: No transmit buffers");
            break;
		
	case TRANSFER_ERROR_NULL_BUFFER_IDENTIFIER:
            snprintf(msg, msgSize, "Transfer Error: NULL buffer identifier");
            break;
		
	case TRANSFER_ERROR_LINK_CONNECTION_ERROR:
            snprintf(msg, msgSize, "Transfer Error: Link connection error");
            break;
		
	case TRANSFER_ERROR_ESCAPE_SEQUENCE:
            snprintf(msg, msgSize, "Transfer Error: Escape Sequence");
            break;
		
	case TRANSFER_ERROR_CREDIT:
            snprintf(msg, msgSize, "Transfer Error: Credit");
            break;
		
	case PCI_LINK_NOT_STARTED:
            snprintf(msg, msgSize, "PCI Link not started");
            break;
		
	default:
            snprintf(msg, msgSize, "Unknown Result state (%d)!", error);
            break;
    }
    if(error != PCI_LINK_NOT_STARTED)
        PciSpaceWire_ResetLink(hDevice, transmitLink-1);
#endif /* NOT_PCI_DEV */
    return;
}

int PeriodicalTickInsSpWpci (void * hDevice, int enable, unsigned int hertz)
{
    /* TODO */
    UNUSED(hDevice);
    UNUSED(enable);
    UNUSED(hertz);
    return 0;
}

int SingleTickInSpWpci (void * hDevice, unsigned char value,
	unsigned char external)
{
#ifdef NOT_PCI_DEV
    UNUSED(hDevice);
    UNUSED(value);
    UNUSED(external);
#else
    unsigned char sendValue;

    if(external)
    {
        sendValue = value;
    }
    else
    {
        sendValue = timecodeValue;
    }

    PciSpaceWire_SendTimeCode(hDevice, sendValue);

    if(!external)
    {
        timecodeValue++;
        if(timecodeValue == 64)
            timecodeValue = 0;
    }
#endif /* NOT_PCI_DEV */
    return 0;
}

void EnableDisableEEPSpWpci(void * hDevice, char enableDisable,
	unsigned char transmitLink)
{
#ifdef NOT_PCI_DEV
    UNUSED(hDevice);
    UNUSED(enableDisable);
    UNUSED(transmitLink);
#else
    PciSpaceWire_EnableError(hDevice, FALSE, enableDisable, transmitLink-1); 
#endif /* NOT_PCI_DEV */
    return;
}

int FlushSpWpci (unsigned char * rxPacket, unsigned int * pLength,
	void * hDevice, unsigned char transmitLink)
{
    int counter = 0;
#ifdef NOT_PCI_DEV
    UNUSED(rxPacket);
    UNUSED(pLength);
    UNUSED(hDevice);
    UNUSED(transmitLink);
#else
    int status = 0;
    do {
        status = PciSpaceWire_ReceiveDataPending(hDevice, transmitLink-1);
        if(status)
        {
            ReadRawSpWPci(rxPacket, (int *)pLength, hDevice, transmitLink);
            counter++;
        }
    } while(status);
#endif /* NOT_PCI_DEV */
    return counter;
}