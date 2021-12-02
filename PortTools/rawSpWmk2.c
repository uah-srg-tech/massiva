/**
 * \file	rawSpWmk2.c
 * \brief	
 *	
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 * 
 * \internal
 * Created:		09/05/2013 at 17:24:55
 * Company:		Space Research Group, Universidad de Alcal�.
 * Copyright:	Copyright (c) 2013, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include "configSpWusbmk2.h"                            /* NOT_MK2_DEV */
#include <stdio.h>					/* sprintf */
#include "../definitions.h"				/* MAX_PACKET_SIZE */

#ifndef	NOT_MK2_DEV
#include "cfg_api_mk2.h"				/* CFG_MK2_XX */
#include "cfg_api_router.h"				/* CFG_ROUTER_XX */	
#include "star-api.h"					/* STAR_XX */
//#define NEW

enum{
    COULD_NOT_CREATE_TX_PACKET = -1,
    COULD_NOT_CREATE_TX_OP = -2,
    COULD_NOT_SUBMIT_TX_OP = -3,
    COULD_NOT_WAIT_ON_TX_OP = -4,
    COULD_NOT_CREATE_TX_TICK_ITEM = -5,
    COULD_NOT_CREATE_TX_TICK_OP = -6,
    COULD_NOT_SUBMIT_TX_TICK_OP = -7,
    COULD_NOT_WAIT_ON_TX_TICK_OP = -8,
    COULD_NOT_CHANGE_TC_PERIOD = -9,
    COULD_NOT_ENABLE_TC_MASTER = -10,
    COULD_NOT_DISABLE_TC_MASTER = -11,
    COULD_NOT_MODIFY_EXTERNAL_SEL = -12,
    COULD_NOT_OPEN_TC_CHANNEL = -13,
    COULD_NOT_CLOSE_TC_CHANNEL = -14,
#ifdef NEW
    COULD_NOT_CREATE_RX_OP = -15,
    COULD_NOT_SUBMIT_RX_OP = -16,
    COULD_NOT_WAIT_ON_RX_OP = -17,
    WRONG_RX_PACKET = -18,
    RX_PACKET_TOO_BIG = -19
#else
    RX_TRANSFER_ERROR = -15,
    RX_PACKET_WRONG_EOP = -16,
#endif /* NEW */
};

static int timecodeValue = 0;
static int eopType = STAR_EOP_TYPE_EOP;
static STAR_EOP_TYPE EopType;
static STAR_TRANSFER_STATUS rxStatus;					//enum

static char waitOnError[5][23]={
    "Not yet started\0",
    "Transfer has begun\0",
    "Transfer has completed\0",
    "Transfer was canceled\0",
    "Error while processing\0"
};

#ifndef NEW
static char EOPTypes[4][22]={
    "Error determining EOP\0",
    "EOP (End of Packet)\0",
    "EEP (Error EOP)\0",
    "No EOP present\0"
};
#endif /* NEW */
#endif /* NOT_MK2_DEV */

int ReadRawSpWmk2(unsigned char * receivedPacket, unsigned int * pLength,
	unsigned int * pChannel)
{
    unsigned int status = 0;
#ifndef	NOT_MK2_DEV
#ifdef NEW
    unsigned int rxPacketLength;
    unsigned char * pRxBuffer; 
    STAR_STREAM_ITEM *pRxStreamItem = NULL;			//structure
    STAR_TRANSFER_OPERATION *pRxTransferOp = NULL;		//structure

    /* Create receive operation to receive 1 packet */
    pRxTransferOp = STAR_createRxOperation(1, STAR_RECEIVE_PACKETS);
    if (!pRxTransferOp)
    {
        return COULD_NOT_CREATE_RX_OP;
    }
    /* Submit the receive operation */
    if (!STAR_submitTransferOperation(*pChannel, pRxTransferOp))
    {
        return COULD_NOT_SUBMIT_RX_OP;
    }
    /* Wait on the receive operation completing (blocking) */
    rxStatus = STAR_waitOnTransferOperationCompletion(pRxTransferOp, -1);
    if(rxStatus == STAR_TRANSFER_STATUS_CANCELLED) //port closed
    {
        status = 0;
    }
    else if(rxStatus != STAR_TRANSFER_STATUS_COMPLETE)
    {
        return COULD_NOT_WAIT_ON_RX_OP;
    }

    /* Get the packet */
    pRxStreamItem = STAR_getTransferItem(pRxTransferOp, 0);
    if ((!pRxStreamItem) ||
            (pRxStreamItem->itemType != STAR_STREAM_ITEM_TYPE_SPACEWIRE_PACKET)
            || (!pRxStreamItem->item))
    {
        STAR_disposeTransferOperation(pRxTransferOp);
        return WRONG_RX_PACKET;
    }
    else
    {
        pRxBuffer =
                STAR_getPacketData((STAR_SPACEWIRE_PACKET *)pRxStreamItem->item,
                &rxPacketLength);
        if (!pRxBuffer)
        {
            STAR_destroyPacketData((unsigned char *)pRxBuffer);
            STAR_disposeTransferOperation(pRxTransferOp);
            return WRONG_RX_PACKET;
        }
        if(rxPacketLength > *pLength)
        {
            STAR_destroyPacketData((unsigned char *)pRxBuffer);
            STAR_disposeTransferOperation(pRxTransferOp);
            return RX_PACKET_TOO_BIG;
        }
        memcpy(receivedPacket, pRxBuffer, rxPacketLength);
        STAR_destroyPacketData((unsigned char *)pRxBuffer);
    }
    status = *pLength;

    /* Destroy the rx transfer operation */
    if (pRxTransferOp)
    {
        STAR_disposeTransferOperation(pRxTransferOp);
    }
#else
    rxStatus = STAR_receivePacket(*pChannel, &receivedPacket[0], pLength,
            &EopType, -1);//-1 is timeout. must be done this way
    status = *pLength;
    if(rxStatus == STAR_TRANSFER_STATUS_CANCELLED) //port closed
    {
        status = 0;
    }
    else if(rxStatus != STAR_TRANSFER_STATUS_COMPLETE)
    {
        status = RX_TRANSFER_ERROR;
    }
    else if(EopType != STAR_EOP_TYPE_EOP)
    {
        status = RX_PACKET_WRONG_EOP;
    }
#endif /* NEW */
#else
    UNUSED(receivedPacket);
    UNUSED(pLength);
    UNUSED(pChannel);
#endif /* NOT_MK2_DEV */
    return status;
}

int WriteRawSpWmk2(const unsigned char * sendPacket, int length,
	unsigned int * pChannel)
{
    int status = 0;
#ifdef NOT_MK2_DEV
    UNUSED(sendPacket);
    UNUSED(length);
    UNUSED(pChannel);
#else
    STAR_TRANSFER_STATUS txStatus;				//enum
    STAR_STREAM_ITEM *pTxStreamItem = NULL;			//structure
    STAR_TRANSFER_OPERATION *pTxTransferOp = NULL;		//structure
	
    /* Create the packet to be transmitted */
    pTxStreamItem = STAR_createPacket(NULL, (U8 *)&sendPacket[0], length,
    eopType);
    if (!pTxStreamItem)
    {
        return COULD_NOT_CREATE_TX_PACKET;
    }
	
    /* Create the transmit transfer operation for the packet */
    pTxTransferOp = STAR_createTxOperation(&pTxStreamItem, 1);
    if (!pTxTransferOp)
    {
        STAR_destroyStreamItem(pTxStreamItem);
        return COULD_NOT_CREATE_TX_OP;
    }
	
    /* Submit the transmit operation */
    if (!STAR_submitTransferOperation(*pChannel, pTxTransferOp))
    {
        STAR_disposeTransferOperation(pTxTransferOp);
        STAR_destroyStreamItem(pTxStreamItem);
        return COULD_NOT_SUBMIT_TX_OP;
    }
	
    /* Wait on the transmit operation completing */
    txStatus = STAR_waitOnTransferOperationCompletion(pTxTransferOp,
            (int)GetSpWMk2Timeout());
    if(txStatus != STAR_TRANSFER_STATUS_COMPLETE)
    {
        STAR_disposeTransferOperation(pTxTransferOp);
        STAR_destroyStreamItem(pTxStreamItem);
        return COULD_NOT_WAIT_ON_TX_OP;
    }
    /* get length */
    status = (int)STAR_getPacketLength((STAR_SPACEWIRE_PACKET *)pTxStreamItem->item);
    /* Dispose of the transmit operation */
    STAR_disposeTransferOperation(pTxTransferOp);
    /* Destroy the packet transmitted */
    STAR_destroyStreamItem(pTxStreamItem);
#endif /* NOT_MK2_DEV */
    return status;
}

void RawSpWmk2RWError(int error, char * msg, int msgSize,
	unsigned int * pDeviceId)
{
#ifdef NOT_MK2_DEV
    UNUSED(error);
    UNUSED(msg);
    UNUSED(msgSize);
    UNUSED(pDeviceId);
#else
    switch (error)
    {
        case COULD_NOT_CREATE_TX_PACKET:
            snprintf(msg, msgSize, "Unable to create packet to be transmitted");
            break;

        case COULD_NOT_CREATE_TX_OP:
            snprintf(msg, msgSize, "Unable to create transmitting operation");
            break;

        case COULD_NOT_SUBMIT_TX_OP:
            snprintf(msg, msgSize, "Error occurred while submitting "
                    "transmission");
            break;

        case COULD_NOT_WAIT_ON_TX_OP:
            snprintf(msg, msgSize, "Error occurred during transmission");
            break;

        case COULD_NOT_CREATE_TX_TICK_ITEM:
            snprintf(msg, msgSize, "Unable to create timecode to be "
                    "transmitted");
            break;

        case COULD_NOT_CREATE_TX_TICK_OP:
            snprintf(msg, msgSize, "Unable to create the timecode transmitting "
                    "operation");
            break;

        case COULD_NOT_SUBMIT_TX_TICK_OP:
            snprintf(msg, msgSize, "Error occurred while submitting timecode "
                    "transmitting");
            break;

        case COULD_NOT_WAIT_ON_TX_TICK_OP:
            snprintf(msg, msgSize, "Error occurred during timecode "
                    "transmission");
            break;

        case COULD_NOT_CHANGE_TC_PERIOD:
            snprintf(msg, msgSize, "Couldn't set time-code master frequency");
            break;

        case COULD_NOT_ENABLE_TC_MASTER:
            snprintf(msg, msgSize, "Couldn't enable time-code master");
            break;

        case COULD_NOT_DISABLE_TC_MASTER:
            snprintf(msg, msgSize, "Couldn't disable time-code master");
            break;

        case COULD_NOT_MODIFY_EXTERNAL_SEL:
            snprintf(msg, msgSize, "Couldn't perform modify external timecode "
                    "selection");
            break;

        case COULD_NOT_OPEN_TC_CHANNEL:
            snprintf(msg, msgSize, "Unable to open timecode channel");
            break;						 

        case COULD_NOT_CLOSE_TC_CHANNEL:
            snprintf(msg, msgSize, "Unable to close timecode channel");
            break;

#ifdef NEW
        case COULD_NOT_CREATE_RX_OP:
            snprintf(msg, msgSize, "Unable to create the receiving operation");
            break;

        case COULD_NOT_SUBMIT_RX_OP:
            snprintf(msg, msgSize, "Error occurred while submitting reception");
            break;
			
        case COULD_NOT_WAIT_ON_RX_OP:
            snprintf(msg, msgSize, "Error occurred while waiting on reception: "
                    "%s", waitOnError[rxStatus]);
            break;
			
        case WRONG_RX_PACKET:
            snprintf(msg, msgSize, "Received an unexpected traffic type, "
                    "or empty traffic item in item");
            break;

        case RX_PACKET_TOO_BIG:
            snprintf(msg, msgSize, "Received packet is too big");
            break;
#else	
        case RX_TRANSFER_ERROR:
            snprintf(msg, msgSize, "Transfer error: %s", waitOnError[rxStatus]);
            break;
            
        case RX_PACKET_WRONG_EOP:
            snprintf(msg, msgSize, "End of Packet received wrong EOP: %s",
                    EOPTypes[EopType]);
            break;
#endif /* NEW */

        default:
            snprintf(msg, msgSize, "Unknown SpW Mk2 Rx/Tx state (%d)!", error);
            break;
    }
    /* we don't know which port caused the error, so we call clear all */
    CFG_ROUTER_clearPortErrors(*pDeviceId, 0);
    CFG_ROUTER_clearPortErrors(*pDeviceId, 1);
    CFG_ROUTER_clearPortErrors(*pDeviceId, 2);
#endif /* NOT_MK2_DEV */
	return;
}

int PeriodicalTickInsSpWmk2 (unsigned int * pDeviceId, int enable,
	unsigned int hertz)
{
#ifdef NOT_MK2_DEV
    UNUSED(pDeviceId);
    UNUSED(enable);
    UNUSED(hertz);
#else
    if(enable)
    {
        if (CFG_MK2_setTimeCodePeriod(*pDeviceId, (hertz / 1000000)) == 0)
            return COULD_NOT_CHANGE_TC_PERIOD;
        if ((CFG_MK2_enableTimeCodeMaster(*pDeviceId)) == 0)
            return COULD_NOT_ENABLE_TC_MASTER;
    }
    else
    {
        if ((CFG_MK2_disableTimeCodeMaster(*pDeviceId)) == 0)
            return COULD_NOT_DISABLE_TC_MASTER;
    }
#endif /* NOT_MK2_DEV */
	return 0;
}

int SingleTickInSpWmk2 (unsigned int * pDeviceId, unsigned char value,
	unsigned char external)
{
#ifdef NOT_MK2_DEV
    UNUSED(pDeviceId);
    UNUSED(value);
    UNUSED(external);
#else
    STAR_TRANSFER_STATUS txTickStatus;				//enum
    STAR_STREAM_ITEM *pTxTickItem = NULL;			//structure
    STAR_TRANSFER_OPERATION *pTxTickOp = NULL;		//structure
    STAR_CHANNEL_ID tickChannel;
    unsigned char sendValue;

    if(external)
    {
#if 0
        if (!CFG_MK2_enableExternalTimeCodeSelection(*pDeviceId))
        {
            return COULD_NOT_MODIFY_EXTERNAL_SEL;
        }
#endif
        sendValue = value;
    }
    else
    {
        sendValue = timecodeValue;
    }

    pTxTickItem = STAR_createTimeCode(sendValue);
    if (!pTxTickItem)
    {
        return COULD_NOT_CREATE_TX_TICK_ITEM;
    }
	
    pTxTickOp = STAR_createTxOperation(&pTxTickItem, 1);
    if (!pTxTickOp)
    {
        STAR_destroyStreamItem(pTxTickItem);
        return COULD_NOT_CREATE_TX_TICK_OP;
    }
	
    /* Open Timecode channel */
    tickChannel = STAR_openChannelToLocalDevice(*pDeviceId,
            STAR_CHANNEL_DIRECTION_OUT, 0, 0);
    if (!tickChannel)
    {
        return COULD_NOT_OPEN_TC_CHANNEL;
    }
	
    /* Submit the transmit operation */
    if (!STAR_submitTransferOperation(tickChannel, pTxTickOp))
    {
        STAR_disposeTransferOperation(pTxTickOp);
        STAR_destroyStreamItem(pTxTickItem);
        return COULD_NOT_SUBMIT_TX_TICK_OP;
    }

    /* Wait on the transmit operation completing */
    txTickStatus = STAR_waitOnTransferOperationCompletion(pTxTickOp, -1);
    if(txTickStatus != STAR_TRANSFER_STATUS_COMPLETE)
    {
        STAR_disposeTransferOperation(pTxTickOp);
        STAR_destroyStreamItem(pTxTickItem);
        return COULD_NOT_WAIT_ON_TX_TICK_OP;
    }

    /* Dispose of the transmit operation */
    STAR_disposeTransferOperation(pTxTickOp);
    /* Destroy the packet transmitted */
    STAR_destroyStreamItem(pTxTickItem);

    if(external)
    {
#if 0
        if (!CFG_MK2_disableExternalTimeCodeSelection(*pDeviceId))
        {
            return COULD_NOT_MODIFY_EXTERNAL_SEL;
        }
#endif
    }
    else
    {
        timecodeValue++;
        if(timecodeValue == 64)
            timecodeValue = 0;
    }

    if(STAR_closeChannel(tickChannel) == 0)
        return COULD_NOT_CLOSE_TC_CHANNEL;
	
#endif /* NOT_MK2_DEV */
    return 0;
}

void EnableDisableEEPSpWmk2(char enableDisable)
{
#ifdef NOT_MK2_DEV
    UNUSED(enableDisable);
#else
    if(enableDisable) //enable EEP
    {
        eopType = STAR_EOP_TYPE_EEP;
    }
    else //if(!enableDisable) //disable EEP
    {
        eopType = STAR_EOP_TYPE_EOP;
    }
#endif /* NOT_MK2_DEV */
    return;
}

int FlushSpWmk2 (unsigned char * rxPacket, unsigned int * pLength,
	unsigned int * pChannel)
{
    int counter = 0;
#ifdef NOT_MK2_DEV
    UNUSED(rxPacket);
    UNUSED(pLength);
    UNUSED(pChannel);
#else
    STAR_TRANSFER_STATUS status = STAR_TRANSFER_STATUS_NOT_STARTED;
    do {
        status = STAR_receivePacket(*pChannel, rxPacket, pLength, &EopType, 1000);
        counter++;
    }while(status == STAR_TRANSFER_STATUS_COMPLETE);
    counter--;//to remove last packet read
#endif /* NOT_MK2_DEV */
    return counter;
}

int InjectErrorSpWMk2(unsigned int * pDeviceId, unsigned char port,
	unsigned char error)
{
    int status = 0;
#ifdef NOT_MK2_DEV
    UNUSED(pDeviceId);
    UNUSED(port);
    UNUSED(error);
#else
	/* if not valid error defined, inject SPW DISCONNECT error */
    if((error < 1) || (error > 7))
        error = SPW_ERROR_DISCONNECT;
    status = CFG_MK2_injectError(*pDeviceId, port, error);
#endif /* NOT_MK2_DEV */
    return status;
}