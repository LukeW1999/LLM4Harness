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

void DNSgetHostByName_a_harness()
{
    const char *pcHostName = (const char *)malloc(nondet_uint());
    uint32_t *pulIPAddress = (uint32_t *)malloc(sizeof(uint32_t));
    TickType_t uxTicksToWait = nondet_uint();

    if (pcHostName == NULL || pulIPAddress == NULL)
    {
        return;
    }

    size_t hostname_length = nondet_uint();
    __CPROVER_assume(hostname_length > 0 && hostname_length <= 255);
    __CPROVER_assume(__CPROVER_forall {size_t i; (i < hostname_length) ==> (pcHostName[i] != '\0')});
    pcHostName[hostname_length - 1] = '\0';

    BaseType_t result = DNSgetHostByName_a(pcHostName, pulIPAddress, uxTicksToWait);

    if (result == pdPASS)
    {
        assert(*pulIPAddress != 0); // Assuming a valid IP address should not be 0
    }
    else
    {
        assert(*pulIPAddress == 0); // If DNS resolution fails, IP address should be set to 0
    }

    free((void *)pcHostName);
    free((void *)pulIPAddress);
}