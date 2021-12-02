/**
 * \file	configSpW.c
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		17/07/2013 at 11:44:32
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdio.h>				/* snprintf */
#include "configSpWusbmk2.h"    /* SpWMk2, NOT_MK2_DEV */
#include "configSpWusb.h"		/* SpW, NOT_USB_DEV */
#include "configSpWpci.h"		/* SpWpci, NOT_PCI_DEV */
#include "../definitions.h"		/* portConfig */

enum {
    NO_DEVICES = -666,
    DEV_NOT_CONFIGURED = -667
};

/**
 * \brief Function to close a SpW device (or UART_PORT debug)
 * \param phDevice			Pointer to spw struct
 * \param readPortNumber	Port to configure as reading pPort (SpW-USB brick)
 * \return Returns the status (0 if correct, < 0 if error)
 */
int SelectOpenSpWDevice(portConfig * pPort)
{
    int status = 0;

    switch(pPort->portType)
    {
        case SPW_USB_MK2_PORT:
#ifdef NOT_MK2_DEV
            status = DEV_NOT_CONFIGURED;
#else
            status = OpenConfigureSpWMk2Device(&pPort->config.spw.channel,
                    &pPort->config.spw.deviceId);
#endif /* NOT_MK2_DEV */
            break;

        case SPW_USB_PORT:
#ifdef NOT_USB_DEV
            status = DEV_NOT_CONFIGURED;
#else
            status = OpenConfigureSpWusbDevice(&pPort->config.spw.ptr,
                    pPort->config.spw.link);
#endif /* NOT_USB_DEV */
            break;

        case SPW_PCI_PORT:
#ifdef NOT_PCI_DEV
            status = DEV_NOT_CONFIGURED;
#else
            status = OpenConfigureSpWpciDevice(&pPort->config.spw.ptr,
                    pPort->config.spw.link);
#endif /* NOT_PCI_DEV */
            break;
            
        default:
            break;
    }
    if(status == 0)
    {
        unsigned int msgLen = 0;
        //create device info string
        if(pPort->portType == SPW_USB_PORT)
        {
            msgLen = snprintf(pPort->deviceInfo, MAX_DEV_INFO_LEN,
                    "USB link %d, ", pPort->config.spw.link);
        }
        else if(pPort->portType == SPW_USB_MK2_PORT)
        {
            msgLen = snprintf(pPort->deviceInfo, MAX_DEV_INFO_LEN,
                    "USB MK2 link %d, ", pPort->config.spw.link);
        }
        else if(pPort->portType == SPW_PCI_PORT)
        {
            msgLen = snprintf(pPort->deviceInfo, MAX_DEV_INFO_LEN,
                    "PCI, ");
        }
        msgLen += snprintf(&pPort->deviceInfo[msgLen],
                MAX_DEV_INFO_LEN-msgLen, "W: 0x%02x, R: 0x%02x",
                pPort->config.spw.writingPort,
                pPort->config.spw.readingPorts[0]);
        unsigned int readIdx = 1;
        for(readIdx = 1; readIdx<pPort->config.spw.numberOfReadingPorts; ++readIdx)
        {
            if((msgLen+7) >= MAX_DEV_INFO_LEN)
            {
                if((msgLen+4) < MAX_DEV_INFO_LEN)
                {
                    msgLen += snprintf(&pPort->deviceInfo[msgLen],
                            MAX_DEV_INFO_LEN-msgLen, "...");
                }
                break;
            }
            else
            {
                msgLen += snprintf(&pPort->deviceInfo[msgLen],
                        MAX_DEV_INFO_LEN-msgLen, ", 0x%02x",
                        pPort->config.spw.readingPorts[readIdx]);
            }
        }
    }
    return status;
}

/**
 * \brief Function to close a SpW device (or UART_PORT debug)
 * \param phDevice		Pointer to spw struct
 * \param status		The SpW configuration status error to be parsed
 * \param[out] msg		The string to copy the error message in
 * \param msgSize		Maximum size of the string to copy the error message in
 */
void SelectDisplaySpWError(portConfig * pPort, int status, char * msg,
	int msgSize)
{
    if(status == NO_DEVICES)
    {
        snprintf(msg, msgSize, "No compatible devices detected");
        return;
    }
    else if(status == DEV_NOT_CONFIGURED)
    {
        snprintf(msg, msgSize, "No compatible devices drivers have been linked");
        return;
    }

    switch(pPort->portType)
    {
        case SPW_USB_MK2_PORT:
#ifdef NOT_MK2_DEV
            snprintf(msg, msgSize, "No SpW USB MK2 drivers have been linked");
#else
            DisplaySpWMk2Error(status, msg, msgSize);
#endif /* NOT_MK2_DEV */
            break;

        case SPW_USB_PORT:
#ifdef NOT_USB_DEV
            snprintf(msg, msgSize, "No SpW USB drivers have been linked");
#else
            DisplaySpWusbError(status, msg, msgSize);
#endif /* NOT_USB_DEV */
            break;

        case SPW_PCI_PORT:
#ifdef NOT_PCI_DEV
            snprintf(msg, msgSize, "No SpW PCI drivers have been linked");
#else
            DisplaySpWpciError(status, msg, msgSize);
            ForceCloseSpWpciDevice(pPort->config.spw.ptr);
#endif /* NOT_PCI_DEV */
            break;
            
        default:
            break;
    }
    return;
}

/**
 * \brief Function to close a SpW device (or UART_PORT debug)
 * \param phDevice			Pointer to spw struct
 * \param readPortNumber	Port configured as reading pPort (SpW-USB brick)
 * \return Returns the status (0 if correct, < 0 if error)
 */
int SelectCloseSpWDevice(portConfig * pPort)
{
    int status = 0; 
    switch(pPort->portType)
    {
        case SPW_USB_MK2_PORT:
#ifdef NOT_MK2_DEV
            status = DEV_NOT_CONFIGURED;
#else
            status = UnconfigCloseSpWMk2Device(&pPort->config.spw.channel);
#endif /* NOT_MK2_DEV */
            break;

        case SPW_USB_PORT:
#ifdef NOT_USB_DEV
            status = DEV_NOT_CONFIGURED;
#else
            status = UnconfigCloseSpWusbDevice(pPort->config.spw.ptr);
#endif /* NOT_USB_DEV */
            break;

        case SPW_PCI_PORT:
#ifdef NOT_PCI_DEV
            status = DEV_NOT_CONFIGURED;
#else
            status = UnconfigCloseSpWpciDevice(pPort->config.spw.ptr, pPort->config.spw.link);
#endif /* NOT_PCI_DEV */
            break;
            
        default:
            break;
    }
    return status;
}

/**
 * \brief Function to choose the SpW device which will be used
 * \brief Looks for SpW PCI-2 boards, and SpW-USB and SpW-USB MK2 bricks 
 * \return Returns 0x01 if USB, 0x02 if MK2, 0x04 if PCI or a combination
 */
unsigned char GetNumberOfSpWDevices()
{
    unsigned char devices = 0;

#ifndef NOT_USB_DEV
    if(ListSpWusbDevices() == 1)
    {
        devices |= 0x01;
    }
#endif /* NOT_USB_DEV */

#ifndef NOT_MK2_DEV
    if(ListSpWMk2Devices() == 1)
    {
        devices |= 0x02;
    }
#endif /* NOT_MK2_DEV */

#ifndef NOT_PCI_DEV
    if(ListSpWpciDevices() == 1)
    {
        devices |= 0x04;
    }
#endif /* NOT_PCI_DEV */

    return devices;
}

double GetSpWTimeout(portConfig * pPort)
{
    double msTimeout = 0;
    switch(pPort->portType)
    {
        case SPW_USB_MK2_PORT:
#ifndef NOT_MK2_DEV
            msTimeout = GetSpWMk2Timeout();
#endif /* NOT_MK2_DEV */
            break;

        case SPW_USB_PORT:
#ifndef NOT_USB_DEV
            msTimeout = GetSpWusbTimeout(pPort->config.spw.ptr);
#endif /* NOT_USB_DEV */
            break;

        case SPW_PCI_PORT:
#ifndef NOT_PCI_DEV
            msTimeout = GetSpWpciTimeout(pPort->config.spw.ptr);
#endif /* NOT_PCI_DEV */
            break;

        default:
            break;
    }
    return msTimeout;
}

void SetSpWTimeout(portConfig * pPort, double msTimeout)
{
#if defined NOT_MK2_DEV && defined NOT_USB_DEV && defined NOT_PCI_DEV
    UNUSED(msTimeout);
#endif
    switch(pPort->portType)
    {
        case SPW_USB_MK2_PORT:
#ifndef NOT_MK2_DEV
            SetSpWMk2Timeout(msTimeout/1000);//only for TX
#endif /* NOT_USB_DEV */
            break;

        case SPW_USB_PORT:
#ifndef NOT_USB_DEV
            SetSpWusbTimeout(pPort->config.spw.ptr, msTimeout/1000);
#endif /* NOT_USB_DEV */
            break;

        case SPW_PCI_PORT:
#ifndef NOT_PCI_DEV
            SetSpWpciTimeout(pPort->config.spw.ptr, msTimeout/1000);
#endif /* NOT_PCI_DEV */
            break;

        default:
            break;
    }
    return;
}