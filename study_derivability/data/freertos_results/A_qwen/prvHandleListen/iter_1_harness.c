#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <cbmc_proof/nondet.h>
#include <cbmc_proof/proof_allocators.h>
#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "IPTraceMacroDefaults.h"

void prvHandleListen_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)pvPortMalloc(sizeof(NetworkBufferDescriptor_t));
    __CPROVER_assume(pxNetworkBuffer != NULL);

    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)pvPortMalloc(ipTOTAL_ETHERNET_FRAME_SIZE);
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);

    pxNetworkBuffer->xDataLength = nondet_uint16_t();
    __CPROVER_assume(pxNetworkBuffer->xDataLength <= ipTOTAL_ETHERNET_FRAME_SIZE);

    pxNetworkBuffer->pxEndPoint = (NetworkEndPoint_t *)pvPortMalloc(sizeof(NetworkEndPoint_t));
    __CPROVER_assume(pxNetworkBuffer->pxEndPoint != NULL);

    pxNetworkBuffer->pxEndPoint->ipv4_settings.ulIPAddress = nondet_uint32_t();
    pxNetworkBuffer->pxEndPoint->ipv4_settings.ulNetMask = nondet_uint32_t();
    pxNetworkBuffer->pxEndPoint->ipv4_settings.ulGatewayAddress = nondet_uint32_t();
    pxNetworkBuffer->pxEndPoint->ipv4_settings.ulDNSServerAddresses[0] = nondet_uint32_t();
    pxNetworkBuffer->pxEndPoint->ipv4_settings.ulDNSServerAddresses[1] = nondet_uint32_t();

    pxNetworkBuffer->pxEndPoint->usPort = nondet_uint16_t();
    pxNetworkBuffer->pxEndPoint->ucNetMask = nondet_uint8_t();
    pxNetworkBuffer->pxEndPoint->ucVersion = nondet_uint8_t();
    pxNetworkBuffer->pxEndPoint->ucMACAddress[0] = nondet_uint8_t();
    pxNetworkBuffer->pxEndPoint->ucMACAddress[1] = nondet_uint8_t();
    pxNetworkBuffer->pxEndPoint->ucMACAddress[2] = nondet_uint8_t();
    pxNetworkBuffer->pxEndPoint->ucMACAddress[3] = nondet_uint8_t();
    pxNetworkBuffer->pxEndPoint->ucMACAddress[4] = nondet_uint8_t();
    pxNetworkBuffer->pxEndPoint->ucMACAddress[5] = nondet_uint8_t();

    FreeRTOS_Socket_t *pxSocket;
    pxSocket = (FreeRTOS_Socket_t *)pvPortMalloc(sizeof(FreeRTOS_Socket_t));
    __CPROVER_assume(pxSocket != NULL);

    pxSocket->u.xTCP.eTCPState = eTCP_LISTEN;
    pxSocket->u.xTCP.usLocalPort = nondet_uint16_t();
    pxSocket->u.xTCP.usRemotePort = nondet_uint16_t();
    pxSocket->u.xTCP.ulRemoteIP = nondet_uint32_t();
    pxSocket->u.xTCP.bits.bPassAcceptFunction = nondet_bool();
    pxSocket->u.xTCP.bits.bPassAcceptFunctionCalled = nondet_bool();
    pxSocket->u.xTCP.bits.bConnPrepared = nondet_bool();
    pxSocket->u.xTCP.bits.bFinSent = nondet_bool();
    pxSocket->u.xTCP.bits.bFinReceived = nondet_bool();
    pxSocket->u.xTCP.bits.bFinAccepted = nondet_bool();
    pxSocket->u.xTCP.bits.bCloseRequested = nondet_bool();
    pxSocket->u.xTCP.bits.bUserShutdown = nondet_bool();
    pxSocket->u.xTCP.bits.bWonElection = nondet_bool();
    pxSocket->u.xTCP.bits.bSendKeepAlive = nondet_bool();
    pxSocket->u.xTCP.bits.bWaitForARP = nondet_bool();
    pxSocket->u.xTCP.bits.bWinAcked = nondet_bool();
    pxSocket->u.xTCP.bits.bCloseAfterSend = nondet_bool();
    pxSocket->u.xTCP.bits.bRxStopped = nondet_bool();
    pxSocket->u.xTCP.bits.bAcceptData = nondet_bool();
    pxSocket->u.xTCP.bits.bConnJustMade = nondet_bool();
    pxSocket->u.xTCP.bits.bRepliedToSYN = nondet_bool();
    pxSocket->u.xTCP.bits.bOwnSocket = nondet_bool();
    pxSocket->u.xTCP.bits.bMallocError = nondet_bool();
    pxSocket->u.xTCP.bits.bPassQueued = nondet_bool();
    pxSocket->u.xTCP.bits.bReuseSocket = nondet_bool();
    pxSocket->u.xTCP.bits.bIsIPv6 = nondet_bool();
    pxSocket->u.xTCP.bits.bIsBound = nondet_bool();
    pxSocket->u.xTCP.bits.bWasBound = nondet_bool();
    pxSocket->u.xTCP.bits.bInQueue = nondet_bool();
    pxSocket->u.xTCP.bits.bPassAcceptFunctionCalled = nondet_bool();
    pxSocket->u.xTCP.bits.bPassAcceptFunction = nondet_bool();
    pxSocket->u.xTCP.bits.bWinAccepted = nondet_bool();
    pxSocket->u.xTCP.bits.bSYNSent = nondet_bool();
    pxSocket->u.xTCP.bits.bSYNReceived = nondet_bool();
    pxSocket->u.xTCP.bits.bFINReceived = nondet_bool();
    pxSocket->u.xTCP.bits.bFINSent = nondet_bool();
    pxSocket->u.xTCP.bits.bFINAccepted = nondet_bool();
    pxSocket->u.xTCP.bits.bCloseRequested = nondet_bool();
    pxSocket->u.xTCP.bits.bUserShutdown = nondet_bool();
    pxSocket->u.xTCP.bits.bWonElection = nondet_bool();
    pxSocket->u.xTCP.bits.bSendKeepAlive = nondet_bool();
    pxSocket->u.xTCP.bits.bWaitForARP = nondet_bool();
    pxSocket->u.xTCP.bits.bWinAcked = nondet_bool();
    pxSocket->u.xTCP.bits.bCloseAfterSend = nondet_bool();
    pxSocket->u.xTCP.bits.bRxStopped = nondet_bool();
    pxSocket->u.xTCP.bits.bAcceptData = nondet_bool();
    pxSocket->u.xTCP.bits.bConnJustMade = nondet_bool();
    pxSocket->u.xTCP.bits.bRepliedToSYN = nondet_bool();
    pxSocket->u.xTCP.bits.bOwnSocket = nondet_bool();
    pxSocket->u.xTCP.bits.bMallocError = nondet_bool();
    pxSocket->u.xTCP.bits.bPassQueued = nondet_bool();
    pxSocket->u.xTCP.bits.bReuseSocket = nondet_bool();
    pxSocket->u.xTCP.bits.bIsIPv6 = nondet_bool();
    pxSocket->u.xTCP.bits.bIsBound = nondet_bool();
    pxSocket->u.xTCP.bits.bWasBound = nondet_bool();
    pxSocket->u.xTCP.bits.bInQueue = nondet_bool();

    pxSocket->pxEndPoint = pxNetworkBuffer->pxEndPoint;

    pxNetworkBuffer->pvPerPacketInfo = (void *)pxSocket;

    prvHandleListen(pxNetworkBuffer);

    // Postconditions
    assert(pxSocket->u.xTCP.eTCPState == eTCP_LISTEN || pxSocket->u.xTCP.eTCPState == eTCP_ESTABLISHED);
    assert(pxNetworkBuffer->eReleaseBuffer == eReleaseBuffer);
}