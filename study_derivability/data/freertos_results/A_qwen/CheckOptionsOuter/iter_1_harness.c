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
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkInterface.h"

void CheckOptionsOuter_harness()
{
    struct xTCP_HEADER xTCPHeader;
    uint8_t ucOptionBuffer[60];
    uint8_t ucLength = nondet_uint8();
    uint8_t ucResult;

    /* Initialize the TCP header with nondeterministic values */
    xTCPHeader.ucTCPOffset = (nondet_uint8() & 0xF0) | 0x50; /* Ensure valid offset */
    xTCPHeader.usURG = nondet_uint16();
    xTCPHeader.usACK = nondet_uint16();
    xTCPHeader.usPSH = nondet_uint16();
    xTCPHeader.usRST = nondet_uint16();
    xTCPHeader.usSYN = nondet_uint16();
    xTCPHeader.usFIN = nondet_uint16();
    xTCPHeader.usWindow = nondet_uint16();
    xTCPHeader.usChecksum = nondet_uint16();
    xTCPHeader.usUrgent = nondet_uint16();

    /* Constrain the length to be within a valid range */
    __CPROVER_assume(ucLength <= sizeof(ucOptionBuffer));

    /* Initialize the option buffer with nondeterministic values */
    for (uint8_t i = 0; i < ucLength; i++) {
        ucOptionBuffer[i] = nondet_uint8();
    }

    /* Call the function under test */
    ucResult = CheckOptionsOuter(&xTCPHeader, ucOptionBuffer, ucLength);

    /* Assert postconditions */
    assert(ucResult == pdTRUE || ucResult == pdFALSE);
}