/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IPv6.h"
#include "FreeRTOS_IP_Private.h"
/* CBMC includes. */
#include "cbmc.h"

eFrameProcessingResult_t prvAllowIPPacketIPv6( const IPHeader_IPv6_t * const pxIPv6Header,
                                               const NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                               UBaseType_t uxHeaderLength );

/* Create an endpoint and return, real endpoint doesn't matter in this test. */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnIP_IPv6( const IPv6_Address_t * pxIPAddress )
{
    __CPROVER_assert(pxIPAddress != NULL, "pxIPAddress should not be NULL");
    NetworkEndPoint_t *pxEndPoint = (NetworkEndPoint_t *)malloc(sizeof(NetworkEndPoint_t));
    __CPROVER_assume(pxEndPoint != NULL);
    return pxEndPoint;
}

void prvAllowIPPacketIPv6_harness()
{
    IPHeader_IPv6_t *pxIPv6Header = (IPHeader_IPv6_t *)malloc(sizeof(IPHeader_IPv6_t));
    NetworkBufferDescriptor_t *pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    UBaseType_t uxHeaderLength;

    __CPROVER_assume(pxIPv6Header != NULL);
    __CPROVER_assume(pxNetworkBuffer != NULL);

    /* Nondeterministic inputs */
    pxIPv6Header->ucVersionTrafficClassFlowLabel[0] = nondet_uint8_t();
    pxIPv6Header->ucVersionTrafficClassFlowLabel[1] = nondet_uint8_t();
    pxIPv6Header->ucVersionTrafficClassFlowLabel[2] = nondet_uint8_t();
    pxIPv6Header->ucVersionTrafficClassFlowLabel[3] = nondet_uint8_t();
    pxIPv6Header->usPayloadLength = nondet_uint16_t();
    pxIPv6Header->ucNextHeader = nondet_uint8_t();
    pxIPv6Header->ucHopLimit = nondet_uint8_t();
    for (size_t i = 0; i < ipSIZE_OF_IPv6_ADDRESS; i++) {
        pxIPv6Header->xSourceAddress.ucBytes[i] = nondet_uint8_t();
        pxIPv6Header->xDestinationAddress.ucBytes[i] = nondet_uint8_t();
    }

    uxHeaderLength = nondet_uber_uint();

    /* Constraints */
    __CPROVER_assume((pxIPv6Header->ucVersionTrafficClassFlowLabel[0] & 0xF0) == 0x60); // IPv6 version check
    __CPROVER_assume(uxHeaderLength >= ipSIZE_OF_IPv6_HEADER);

    eFrameProcessingResult_t result = prvAllowIPPacketIPv6(pxIPv6Header, pxNetworkBuffer, uxHeaderLength);

    /* Postconditions */
    switch (result) {
        case eReleaseBuffer:
        case eProcessBuffer:
        case eReturnEthernetFrame:
            break;
        default:
            __CPROVER_assert(0, "Unexpected result from prvAllowIPPacketIPv6");
    }
}