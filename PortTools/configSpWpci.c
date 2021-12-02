/**
 * \file	configSpWpci.c
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
#ifndef NOT_PCI_DEV

#include <stdio.h>						/* sprintf */
#include "SpaceWirePCI2.h"

unsigned char pciClosed = 0;

enum {
    COULD_NOT_OPEN_DEVICE = -11,
    COULD_NOT_START_LINK = -12,
    COULD_NOT_STOP_LINK = -13
};

/**
 * \brief Function to get the number of SpW PCI2 board
 */
int ListSpWpciDevices(void)
{
    U32 numBoards = 0;
    //numBoards = PciSpaceWire_CountBoards();
    /* It doesn't work */

    void * hDevice;
    if(PciSpaceWire_Open(&hDevice, 0))
    {
        numBoards = 1;
        PciSpaceWire_Close(hDevice);
    }
    return numBoards;
}

/**
 * \brief Function to open and configure a SpW PCI2 board
 * \brief and then start a link 
 * \param phDevice		A pointer to a handle of the device
 * \param transmitLink	The SpW link to start (only for or PCI board)
 * \return Returns the status (0 if correct, < 0 if error)
 */
int OpenConfigureSpWpciDevice (void ** phDevice, int transmitLink)
{
    //Open the SpWdevice
    if(PciSpaceWire_Open(phDevice, 0) == 0)
        return COULD_NOT_OPEN_DEVICE;

    PciSpaceWire_Base_Speed(*phDevice, BOARD_SPEED_80);

    //Start the link
    PciSpaceWire_ResetLink(*phDevice, transmitLink-1); 
    if(PciSpaceWire_StartLinkAuto(*phDevice, transmitLink-1, LINK_SPEED_8TH,
            FALSE, FALSE, FALSE) == 0)
        return COULD_NOT_START_LINK;

    return 0;
}

/**
 * \brief Function which parses an SpW configuration error error and copies an
 * \brief error message into a string. 
 * \param error		The SpW configuration status error to be parsed
 * \param[out] msg	The string to copy the error message in
 */
void DisplaySpWpciError(int error, char * msg, int msgSize)
{
    switch (error)
    {
        case COULD_NOT_OPEN_DEVICE:
            snprintf(msg, msgSize, "Couldn't open the device");
            break;

        case COULD_NOT_START_LINK:
            snprintf(msg, msgSize, "Couldn't start PCI link");
            break;

        case COULD_NOT_STOP_LINK:
            snprintf(msg, msgSize, "Couldn't stop PCI link");
            break;

        default:
            snprintf(msg, msgSize, "Unknown Configuration SPW PCI2 error %d",
                    error);	
            break;
    }
}

/**
 * \brief Function to stop a link and close the SpW PCI2 board
 * \param hDevice		A handle of the device
 * \param transmitLink	The SpW link to stop
 * \return Returns the status (0 if correct, < 0 if error)
 */
int UnconfigCloseSpWpciDevice(void * hDevice, int transmitLink)
{
    //Stop the link (crashes)
    /*if(PciSpaceWire_StopLink(hDevice, transmitLink-1) == 0)
            return COULD_NOT_STOP_LINK;*/
    // Close the device
    pciClosed = 1;
    PciSpaceWire_Close(hDevice);
    return 0;
}

int ForceCloseSpWpciDevice(void * hDevice)
{
    // Close the device
    PciSpaceWire_Close(hDevice);
    return 0;
}

double GetSpWpciTimeout (void * hDevice)
{
    return PciSpaceWire_GetTimeOut(hDevice);
}

void SetSpWpciTimeout (void * hDevice, double sTimeout)
{
    PciSpaceWire_SetTimeOut(hDevice, sTimeout);
    return;
}
#endif /* NOT_PCI_DEV */