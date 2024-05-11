/**
 * \file	configSpWusb.c
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		22/03/2012 at 16:06:42
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include "configSpWusb.h"				/* NOT_USB_DEV ? */

#ifndef NOT_USB_DEV
#if !defined (_WIN32) && defined (__CYGWIN__)
#define _WIN32	//for cygwin
#endif

#include "spw_usb_api.h"
#include "spw_config_library.h"
#include <stdio.h>						/* sprintf */

//#define PRINT_VERSION

enum {
    COULD_NOT_OPEN_DEVICE = -11,
    COULD_NOT_SET_AS_INTERFACE = -12,
    COULD_NOT_SET_BASE_TX_RATE = -13,
    COULD_NOT_SET_LINK_SPEED = -14,
    COULD_NOT_REGISTER_PORT = -15,
    COULD_NOT_UNREGISTER_PORT = -16,
    COULD_NOT_SET_TICK_ENABLE = -17
};

static int txRxLink;
static int binLink[3] = {2, 4, 8};

/**
 * \brief Function to get the number of USB-SpW brick devices
 */
int ListSpWusbDevices(void)
{
    U8 deviceCount = 0;
    //deviceCount = USBSpaceWire_CountDevices();
    /* It doesn't work */

    star_device_handle hDevice;
    if(USBSpaceWire_Open(&hDevice, 0))
    {
        deviceCount = 1;
        USBSpaceWire_Close(hDevice);
    }
    return deviceCount;
}

/**
 * \brief Function to open and configure a USB brick SpW device
 * \brief and then listen on a port
 * \param phDevice		A pointer to a handle of the device
 * \param portNumber	The SpW port to listen at
 * \return Returns the status (0 if correct, < 0 if error)
 */
int OpenConfigureSpWusbDevice(void ** phDevice, int link)
{
    int status = 0;
    // Open the SpW device
    if(!USBSpaceWire_Open(phDevice, 0))
        return COULD_NOT_OPEN_DEVICE;
    
    if(!CFGSpaceWire_IsRMAPEnabled())
        CFGSpaceWire_EnableRMAP(1);
    if(CFGSpaceWire_GetRMAPDestinationKey() != 0x20)
        CFGSpaceWire_SetRMAPDestinationKey(0x20);
    /* don't forget to clear stack for a right re-configuration*/
    CFGSpaceWire_StackClear();
    CFGSpaceWire_AddrStackPush(0);
    CFGSpaceWire_AddrStackPush(254);
    CFGSpaceWire_RetAddrStackPush(254);

    if(CFGSpaceWire_SetBrickBaseTransmitRate(*phDevice, CFG_BRK_CLK_100_MHZ,
            CFG_BRK_DVDR_1, 0xff) != CFG_TRANSFER_SUCCESS) //100 MBits/s
        return COULD_NOT_SET_BASE_TX_RATE;
    
    if(CFGSpaceWire_SetLinkSpeed(*phDevice, link, 9) != CFG_TRANSFER_SUCCESS)
        return COULD_NOT_SET_LINK_SPEED;			   //10 MBits/s

    if(CFGSpaceWire_SetAsInterface(*phDevice, link, 0) != CFG_TRANSFER_SUCCESS)
        return COULD_NOT_SET_AS_INTERFACE;

    if(CFGSpaceWire_SetTickEnableStatus(*phDevice, binLink[link-1])
            != CFG_TRANSFER_SUCCESS)
        return COULD_NOT_SET_TICK_ENABLE;

    if(!USBSpaceWire_RegisterReceiveOnAllPorts(*phDevice))
        return COULD_NOT_REGISTER_PORT;

#ifdef PRINT_LINK_SPEED
    U32 clk, dvdr, enable, base_clk, base_dvdr, statusControl, operatingSpeed;

    if(CFGSpaceWire_GetBrickBaseTransmitRate(*phDevice, &clk, &dvdr, &enable) ==
            CFG_TRANSFER_SUCCESS)
    {
        switch(clk)
        {
            case CFG_BRK_CLK_100_MHZ: base_clk = 100; break;
            case CFG_BRK_CLK_120_MHZ: base_clk = 120; break;
            case CFG_BRK_CLK_140_MHZ: base_clk = 140; break;
            case CFG_BRK_CLK_160_MHZ: base_clk = 160; break;
            case CFG_BRK_CLK_180_MHZ: base_clk = 180; break;
            case CFG_BRK_CLK_200_MHZ: base_clk = 200; break;
        }

        switch(dvdr)
        {
            case CFG_BRK_DVDR_1: base_dvdr = 1; break;
            case CFG_BRK_DVDR_2: base_dvdr = 2; break;
            case CFG_BRK_DVDR_4: base_dvdr = 4; break;
        }

        if (CFGSpaceWire_GetLinkStatusControl(*phDevice, 1, &statusControl) ==
                CFG_TRANSFER_SUCCESS) 
        {
            CFGSpaceWire_LSOperatingSpeed(statusControl, &operatingSpeed);
            printf("Link Speed = %d\n", base_clk/base_dvdr/(operatingSpeed+1));
            }
    }
#endif /* PRINT_LINK_SPEED */

    txRxLink = link;
#ifdef PRINT_VERSION
    SPACEWIRE_FIRMWARE_VERSION version;
    unsigned int versionInt = 0;
    if (USBSpaceWire_GetFirmwareVersionExtended(*phDevice, &version)) 
    { 
        versionInt = (version.major << 24) | (version.minor << 16) |
                (version.edit << 8) | version.patch;
    }
#endif /* PRINT_VERSION */
    return status;
}

/**
 * \brief Function which parses an SpW configuration error error and copies an
 * \brief error message into a string. 
 * \param error		The SpW configuration status error to be parsed
 * \param[out] msg	The string to copy the error message in
 * \param msgSize	Maximum size of the string to copy the error message in
 */
void DisplaySpWusbError(int error, char * msg, int msgSize)
{
    switch (error)
    {
        case CFG_TRANSMIT_PACKET_FAILURE:
            snprintf(msg, msgSize, "A write to the device failed");
            break;
			 
        case CFG_REPLY_PACKET_FAILURE:
            snprintf(msg, msgSize, "A response from the device failed");
            break;
			 
        case CFG_REPLY_PACKET_TOO_BIG:
            snprintf(msg, msgSize, "The response packet from the device "
                    "was larger than expected");
            break;
			 
        case CFG_REPLY_PACKET_TOO_SMALL:
            snprintf(msg, msgSize, "The response packet from the device "
                    "was not as large as expected");
            break;
			 
        case CFG_REPLY_PACKET_NAK:
            snprintf(msg, msgSize, "A negative acknowledgement was sent by"
                    " the device");
            break;
			 
        case CFG_REPLY_PACKET_CHECKSUM_ERROR:
            snprintf(msg, msgSize, "The response from the device was a "
                    "packet with an incorrect checksum value");
            break;
			
        case COULD_NOT_SET_BASE_TX_RATE:
            snprintf(msg, msgSize, "Couldn't set transmit rate");
            break;
			
        case COULD_NOT_SET_LINK_SPEED:
            snprintf(msg, msgSize, "Couldn't set link speed");
            break;
			
        case COULD_NOT_SET_AS_INTERFACE:
            snprintf(msg, msgSize, "Couldn't set brick as interface");
            break;
			
        case COULD_NOT_REGISTER_PORT:
            snprintf(msg, msgSize, "Couldn't register the port");
            break;
			
        case COULD_NOT_UNREGISTER_PORT:
            snprintf(msg, msgSize, "Couldn't unregister the port");
            break;
			
        case COULD_NOT_SET_TICK_ENABLE:
            snprintf(msg, msgSize, "Couldn't set tick enable");
            break;
			
        case COULD_NOT_OPEN_DEVICE:
            snprintf(msg, msgSize, "Couldn't open the device");
            break;
			
        default:
            snprintf(msg, msgSize, "Unknown Configuration SPW-USB error %d",
                    error);
            break;
    }
}

/**
 * \brief Function to stop listening at a port in a USB brick SpW device
 * \brief and close the device 
 * \param hDevice		A handle of the device
 * \param portNumber	The SpW port to listen at
 * \return Returns the status (0 if correct, < 0 if error)
 */
int UnconfigCloseSpWusbDevice(void * hDevice)
{
    // Indicates that the handle no longer wishes to receive packets
    // with a first byte of PORT_NUM
    if(!USBSpaceWire_UnregisterReceiveOnAllPorts(hDevice))
        return COULD_NOT_UNREGISTER_PORT;

    // Close the device
    USBSpaceWire_Close(hDevice);
    return 0;
}

double GetSpWusbTimeout (void * hDevice)
{
    return USBSpaceWire_GetTimeout(hDevice);
}

void SetSpWusbTimeout (void * hDevice, double sTimeout)
{
    USBSpaceWire_SetTimeout(hDevice, sTimeout);
    return;
}
#endif /* NOT_USB_DEV */