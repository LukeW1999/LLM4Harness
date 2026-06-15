/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_IPv6_Utils.h"
/* CBMC includes. */
#include "cbmc.h"

void prvChecksumIPv6Checks_harness()
{
    NetworkBufferDescriptor_t *pxNetworkBuffer;
    size_t uxBufferLength;
    uint16_t usExpectedChecksum;
    uint16_t usCalculatedChecksum;

    /* Allocate non-deterministic inputs */
    pxNetworkBuffer = (NetworkBufferDescriptor_t *)malloc(sizeof(NetworkBufferDescriptor_t));
    __CPROVER_assume(pxNetworkBuffer != NULL);

    pxNetworkBuffer->pucEthernetBuffer = (uint8_t *)malloc(nondet_uint());
    __CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer != NULL);

    uxBufferLength = nondet_size_t();
    __CPROVER_assume(uxBufferLength > 0 && uxBufferLength <= ipTOTAL_ETHERNET_FRAME_SIZE);

    usExpectedChecksum = nondet_uint16();

    /* Call the function under test */
    usCalculatedChecksum = prvChecksumIPv6Checks(pxNetworkBuffer, uxBufferLength, usExpectedChecksum);

    /* Assert postconditions */
    assert(usCalculatedChecksum == usExpectedChecksum || usCalculatedChecksum == ~usExpectedChecksum);
}