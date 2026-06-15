/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkBufferManagement.h"

void DNSgetHostByName_harness()
{
    const char *pcHostName;
    uint32_t ulIPAddress;
    TickType_t xBlockTimeTicks;

    /* Allocate nondeterministic inputs */
    pcHostName = (const char *)malloc(nondet_uint() * sizeof(char));
    ulIPAddress = nondet_uint();
    xBlockTimeTicks = nondet_uint();

    /* Constrain inputs */
    __CPROVER_assume(pcHostName != NULL);
    __CPROVER_assume(strlen(pcHostName) <= 255); /* Maximum length of a domain name */
    __CPROVER_assume(xBlockTimeTicks <= portMAX_DELAY);

    /* Call the function under test */
    ulIPAddress = FreeRTOS_gethostbyname(pcHostName);

    /* Assert postconditions */
    assert((ulIPAddress == 0) || (ulIPAddress != 0)); /* IP address should be either valid or zero */
}