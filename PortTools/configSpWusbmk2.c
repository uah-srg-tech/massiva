/**
 * \file	configSpWusbmk2.c
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		10/07/2013 at 17:35:37
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include "configSpWusbmk2.h"			/* NOT_MK2_DEV */
#ifndef NOT_MK2_DEV

#include <stdio.h>				/* sprintf */
#include "cfg_api_mk2.h"			/* CFG_MK2_XX */
#include "cfg_api_brick_mk2.h"			/* CFG_BRICK_MK2_XX */
#include "cfg_api_router.h"			/* CFG_ROUTER_XX */	

enum {
    NO_DEVICES_PRESENT = -11,
    CANT_OPEN_CHANNEL = -12,
    CANT_CLOSE_CHANNEL = -13,
    CANT_SET_TC_DISTRIBUTION_PORTS = -14,
};

static unsigned int spwMk2TimeoutInMs;

/**
 * \brief Function to get the number of USB-SpW MK2 brick devices
 */
int ListSpWMk2Devices(void)
{
    U32 deviceCount = 0;
    STAR_getDeviceList(&deviceCount);
    return deviceCount;
}

/**
 * \brief Function to open a channel in a USB-SpW Mk2 brick
 * \param pChannelId		A pointer to a channel handle for the device
 * \param transmitLink		The link to send packet (link 1)
 * \return Returns the status (0 if correct, < 0 if error)
 */
int OpenConfigureSpWMk2Device(unsigned int * pChannelId,
	unsigned int * pDeviceId)
{
    //STAR_CHANNEL_ID is typedef of U32
    //STAR_SPACEWIRE_ADDRESS is struct of U16 and U8*
    U32 deviceCount = 0;
    STAR_DEVICE_ID * devices;

    /* Select first and only device (if it is present) */
    devices = STAR_getDeviceList(&deviceCount);
    if(deviceCount == 0)
    {
        return NO_DEVICES_PRESENT;
    }
    *pDeviceId = devices[0];
    STAR_destroyDeviceList(devices);

    /* clear all possible port errors */
    CFG_ROUTER_clearPortErrors(*pDeviceId, 0);
    CFG_ROUTER_clearPortErrors(*pDeviceId, 1);
    CFG_ROUTER_clearPortErrors(*pDeviceId, 2);

    /* Disable Periodical Time Codes */
    CFG_MK2_disableTimeCodeMaster(*pDeviceId);

    /* Enable interface mode */
    CFG_MK2_enableInterfaceMode(*pDeviceId);
    CFG_BRICK_MK2_enableInterfaceModeOnPort(*pDeviceId, 1);
    CFG_BRICK_MK2_enableInterfaceModeOnPort(*pDeviceId, 2);

    /* Set link speed for link 1 to 100Mbps (200/20)*/
    CFG_BRICK_MK2_setLinkClockFrequency(*pDeviceId, 1,
            STAR_CFG_BRICK_MK2_LINK_FREQ_200);
    CFG_MK2_setLinkRateDivider(*pDeviceId, 1, 20);

    /* Set link speed for link 2 to 100Mbps (200/20)*/
    CFG_BRICK_MK2_setLinkClockFrequency(*pDeviceId, 2,
            STAR_CFG_BRICK_MK2_LINK_FREQ_200);
    CFG_MK2_setLinkRateDivider(*pDeviceId, 2, 20);

    /* Open the first and only channel */
    *pChannelId = STAR_openChannelToLocalDevice(*pDeviceId,
            STAR_CHANNEL_DIRECTION_INOUT, 1, TRUE);
    //"1" is channel number
    // When a device is in interface mode, a channel corresponds to a port
    //"TRUE" means incoming data is queued in driver, avoiding packet drops
    if (!(*pChannelId))
    {
       return CANT_OPEN_CHANNEL;
    }

#ifndef __linux__
    /* Specify which output ports time-codes are forwarded on.  */
    if (!CFG_ROUTER_setTimeCodeDistributionPorts(*pDeviceId, 0x00000fff))
    {
        return CANT_SET_TC_DISTRIBUTION_PORTS;
    }
#endif
    return 0;
}

/**
 * \brief Function which parses an USB-SpW Mk2 brick configuration error and
 * \brief copies an error message into a string. 
 * \param error		The SpW configuration status error to be parsed
 * \param[out] msg	The string to copy the error message in
 * \param msgSize	Maximum size of the string to copy the error message in
 */
void DisplaySpWMk2Error(int error, char * msg, int msgSize)
{
    switch (error)
    {
        case NO_DEVICES_PRESENT:
            snprintf(msg, msgSize, "No SPW Mk2 devices");
            break;

        case CANT_OPEN_CHANNEL:
            snprintf(msg, msgSize, "Unable to open channel");
            break;

        case CANT_CLOSE_CHANNEL:
            snprintf(msg, msgSize, "Unable to close channel");
            break;

        case CANT_SET_TC_DISTRIBUTION_PORTS:
            snprintf(msg, msgSize, "Unable to specify which output ports "
                    "timecodes are forwarded on");
            break;

        default:
            snprintf(msg, msgSize, "Unknown configuration SPW-USB Mk2 Error %d",
                    error);
            break;
    }
}

/**
 * \brief Function to close a channel in a USB-SpW Mk2 brick
 * \param pChannelId		A pointer to a channel handle for the device
 * \return Returns the status (0 if correct, < 0 if error)
 */
int UnconfigCloseSpWMk2Device(unsigned int * pChannelId)
{
    if(STAR_closeChannel(*pChannelId) == 0)
        return CANT_CLOSE_CHANNEL;
    return 0;
}

double GetSpWMk2Timeout (void)
{
    return (double)spwMk2TimeoutInMs / 1000;
}

void SetSpWMk2Timeout (double sTimeout)
{
    spwMk2TimeoutInMs = (unsigned int)sTimeout;
}
#endif /* NOT_MK2_DEV */