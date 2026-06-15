/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

void DNSgetHostByName_cancel_harness()
{
    const char * pcHostName;
    TickType_t xTimeout;
    struct freertos_addrinfo * pxAddressInfo;

    pcHostName = __CPROVER_string("example.com");
    __CPROVER_assume(pcHostName != NULL);

    xTimeout = nondet_TickType_t();

    pxAddressInfo = (struct freertos_addrinfo *)malloc(sizeof(struct freertos_addrinfo));
    __CPROVER_assume(pxAddressInfo != NULL);

    DNS_gethostbyname_a(pcHostName, pxAddressInfo, xTimeout);

    DNSgetHostByName_cancel(pxAddressInfo);
}