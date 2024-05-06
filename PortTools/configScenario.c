/**
 * \file	XMLPortTools.c
 * \brief	functions for parsing XML port files (definition)
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		05/03/2012 at 17:48:22
 * Company:		Space Research Group, Universidad de AlcalÃÂ¡.
 * Copyright:	Copyright (c) 2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#include <stdio.h>				/* sprintf */
#include <string.h>
#include "../definitions.h"			/* portConfig, MAX_MSG_SIZE */
#include "../PortTools/configSpW.h"		/* SelectOpenSpWDevice, ... */
#include "../PortTools/configSerial.h"		/* ConfigureSerial, ... */
#include "../PortTools/configTCPSocket.h"	/* ConfigureTCPSocket, ... */
#include "../PortTools/configUDPSocket.h"	/* ConfigureUDPSocket, ... */
#include "../PortTools/rawProtocol.h"		/* ptclMutex, ... */

static int portError;
static int TCPserverConfigured = 0;

enum {
    UNKNOWN_PORT = -501,
    NO_SPW_DEVICE = -502,
};

int ConfigPort(unsigned int portIndex, portConfig * ports,
        serialConfig * pSerialConf)
{
    int status = 0;
    unsigned int handle = 0;
    unsigned char devices;
    portConfig * pPort = &ports[portIndex];
    portError = portIndex;

    if(portIndex == 0)
        resetUartPortHandles();
    switch(pPort->portType)
    {
        case SPW_USB_PORT: case SPW_USB_MK2_PORT: case SPW_PCI_PORT:
            devices = GetNumberOfSpWDevices();

            if((pPort->portType == SPW_USB_PORT) && (!(devices &0x01)))
                return NO_SPW_DEVICE;
            else if((pPort->portType == SPW_USB_MK2_PORT) && (!(devices &0x02)))
                return NO_SPW_DEVICE;
            else if((pPort->portType == SPW_PCI_PORT) && (!(devices &0x04)))
                return NO_SPW_DEVICE;

            if ((status = SelectOpenSpWDevice(pPort)) < 0)
            {
                pPort->config.spw.link = 0;
                pPort->config.spw.protocolID = 0;
                pPort->config.spw.writingPort = 0;
                pPort->config.spw.numberOfReadingPorts = 0;
                unsigned int idx = 0;
                for(idx=0; idx<MAX_READ_PORTS; ++idx)
                    pPort->config.spw.readingPorts[idx] = 0;
                return status;
            }
            SetSpWTimeout(pPort, DEFAULT_TIMEOUT_MS);
            break;

        case UART_PORT:
            if ((status = ConfigureSerial(&pPort->config.uart,
                    pSerialConf->baudRate, pSerialConf->parity,
                    pSerialConf->dataBits, pSerialConf->stopBits,
                    pSerialConf->control, pPort->deviceInfo)) != 0)
                return status;
            pthread_mutex_init(&ptclMutex[portIndex], NULL);
            pPort->ptcl.portValid = 1;
            pPort->ptcl.portConnected = 1;
            break;

        case SPW_TC_PORT:
            handle = pPort->config.spw.channel;
            switch(pPort->portTypeAux)
            {
                case SPW_USB_MK2_PORT:
                    /* when spwRef is configured, handle.u32 and type.spw.deviceId must be copied */
                    pPort->config.spw.channel = ports[handle].config.spw.channel;
                    pPort->config.spw.deviceId = ports[handle].config.spw.deviceId;
                    break;

                case SPW_USB_PORT: case SPW_PCI_PORT:
                    /* when spwRef is configured, handle.ptr must be copied */
                    pPort->config.spw.ptr = ports[handle].config.spw.ptr;
                    break;
                    
                default:
                    break;
            }
            break;
            
        case TCP_SOCKET_SRV_PORT:
            if ((status = ConfigureTCPSocketServer(pPort->config.socket.localIp,
                    pPort->config.socket.localPort,
                    &pPort->config.socket.localAddr)) != 0)
                return status;
            pthread_mutex_init(&ptclMutex[portIndex], NULL);
            pPort->ptcl.portValid = 1;
            TCPserverConfigured = 1;
            break;
        
        case TCP_SOCKET_CLI_PORT:
            if ((status = ConfigureTCPSocketClient(pPort->config.socket.remoteIp,
                    pPort->config.socket.remotePort,
                    &pPort->config.socket.socketHdl)) != 0)
                return status;
            pthread_mutex_init(&ptclMutex[portIndex], NULL);
            pPort->ptcl.portValid = 1;
            pPort->ptcl.portConnected = 1;
            break;
            
        case UDP_SOCKET_PORT:
            if ((status = ConfigureUDPSocket(pPort->config.socket.localIp,
                    pPort->config.socket.localPort,
                    &pPort->config.socket.localAddr,
                    pPort->config.socket.remoteIp,
                    pPort->config.socket.remotePort,
                    &pPort->config.socket.remoteAddr,
                    &pPort->config.socket.socketHdl)) != 0)
                return status;
            break;
        
        case SPW_ERROR_PORT: case DUMMY_PORT:
            break;

        default:
            return UNKNOWN_PORT;
    }
    return 0;
}

void DisplayConfigPortError(int error, portConfig * pPort,
	char * msg, unsigned int msgSize)
{
    if(error == UNKNOWN_PORT)
    {
        snprintf(msg, msgSize, "Unknown port %d", portError);
        return;
    }

    unsigned int portInfoMsgSize = 0;
    char SpWport[4][4] = {"", "USB", "MK2", "PCI"};
    switch (pPort->portType)
    {
            case SPW_USB_PORT: case SPW_USB_MK2_PORT: case SPW_PCI_PORT:
                portInfoMsgSize = snprintf(msg, msgSize,
                        "Error while configuring port %d (SpW %s): ",
                        portError, SpWport[pPort->portType]);
                SelectDisplaySpWError(pPort, error, &msg[portInfoMsgSize],
                        msgSize-portInfoMsgSize);
                break;

            case UART_PORT:
                portInfoMsgSize = snprintf(msg, msgSize,
                        "Error while configuring port %d (%s): ",
                        portError, pPort->config.uart.portName);
                DisplaySerialError(error, &msg[portInfoMsgSize],
                        msgSize-portInfoMsgSize);
                break;

        case TCP_SOCKET_SRV_PORT: case TCP_SOCKET_CLI_PORT:
                portInfoMsgSize = snprintf(msg, msgSize,
                        "Error while configuring port %d (TCP socket): ",
                        portError);
                DisplayTCPSocketError(error, &msg[portInfoMsgSize],
                        msgSize-portInfoMsgSize);
                break;

        case UDP_SOCKET_PORT:
                portInfoMsgSize = snprintf(msg, msgSize,
                        "Error while configuring port %d (UDP socket): ",
                        portError);
                DisplayUDPSocketError(error, &msg[portInfoMsgSize],
                        msgSize-portInfoMsgSize);
                break;

            default:
                portInfoMsgSize = snprintf(msg, msgSize,
                        "Error while configuring port %d: ", portError);
                break;
    }

    switch(error)
    {
        case NO_SPW_DEVICE:
            snprintf(&msg[portInfoMsgSize], msgSize-portInfoMsgSize,
                    "Selected SpW device is not available");
            break;
    }
    return;
}

int UnconfigPort(unsigned int portIndex, portConfig * pPort)
{
    int status = 0;
    portError = portIndex;

    switch(pPort->portType)
    {
        case SPW_USB_PORT: case SPW_USB_MK2_PORT: case SPW_PCI_PORT:
            if((status = SelectCloseSpWDevice(pPort)) != 0)
                return status;
            pPort->config.spw.link = 0;
            pPort->config.spw.protocolID = 0; 
            pPort->config.spw.numberOfReadingPorts = 0;
            pPort->config.spw.writingPort = 0;
            unsigned int idx;
            for(idx=0; idx<MAX_READ_PORTS; ++idx)
                pPort->config.spw.readingPorts[idx] = 0;
            break;

        case SPW_TC_PORT: case SPW_ERROR_PORT: case NO_PORT: case DUMMY_PORT:
            break;

        case UART_PORT:
            pthread_mutex_lock(&ptclMutex[portIndex]);
            pPort->ptcl.portValid = 0;
            pPort->ptcl.portConnected = 0;
            status = CloseSerialPort(pPort->config.uart.portNum);
            pthread_mutex_unlock(&ptclMutex[portIndex]);
            break;
            
        case TCP_SOCKET_SRV_PORT: case TCP_SOCKET_CLI_PORT:
            pthread_mutex_lock(&ptclMutex[portIndex]);
            pPort->ptcl.portValid = 0;
            pPort->ptcl.portConnected = 0;
            status = CloseTCPSocket(&pPort->config.socket.socketHdl);
            pthread_mutex_unlock(&ptclMutex[portIndex]);
            break;
            
        case UDP_SOCKET_PORT: 
            pPort->ptcl.portValid = 0;
            status = CloseUDPSocket(&pPort->config.socket.socketHdl);
            break;

        default:
            return UNKNOWN_PORT;
    }
    return status;
}

int UnconfigServer()
{
    int status = 0;
    if(TCPserverConfigured)
        status = CloseTCPSocket(NULL);
    return status;
}

void DisplayUnconfigPortError(int status, portConfig * pPort, char * msg,
        unsigned int msgSize)
{
    unsigned int msgLen = 0;
    switch (pPort->portType)
    {
        case SPW_USB_PORT: case SPW_USB_MK2_PORT: case SPW_PCI_PORT:
            msgLen = snprintf(msg, msgSize,
                    "Error while closing SpW port %d: ", portError);
            SelectDisplaySpWError(pPort, status, &msg[msgLen],
                    msgSize-msgLen);
            break;

        case UART_PORT:
            msgLen = snprintf(msg, msgSize,
                    "Error while closing serial port %d: ", portError);
            DisplaySerialError(status, &msg[msgLen], msgSize-msgLen);
            break;

        case TCP_SOCKET_SRV_PORT: case TCP_SOCKET_CLI_PORT:
            msgLen = snprintf(msg, msgSize,
                    "Error while closing TCP socket port %d: ", portError);
            DisplayTCPSocketError(status, &msg[msgLen], msgSize-msgLen);
            break;


        case UDP_SOCKET_PORT:
            msgLen = snprintf(msg, msgSize,
                    "Error while closing UDP socket port %d: ", portError);
            DisplayTCPSocketError(status, &msg[msgLen], msgSize-msgLen);
            break;

        default:
            break;
    }
    return;
}