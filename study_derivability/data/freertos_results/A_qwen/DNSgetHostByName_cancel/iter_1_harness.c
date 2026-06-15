#include "FreeRTOS.h"
#include "queue.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP_Private.h"
#include "cbmc.h"

#define MAX_HOSTNAME_LEN 256

void DNSgetHostByName_cancel_harness()
{
    char pcHostName[MAX_HOSTNAME_LEN];
    uint32_t ulIPAddress;
    TickType_t uxTicksToWait;

    /* Nondeterministic inputs */
    for (size_t i = 0; i < MAX_HOSTNAME_LEN; i++) {
        pcHostName[i] = nondet_char();
    }
    ulIPAddress = nondet_uint32_t();
    uxTicksToWait = nondet_TickType_t();

    /* Constraints */
    __CPROVER_assume(strlen(pcHostName) < MAX_HOSTNAME_LEN);
    __CPROVER_assume(uxTicksToWait <= portMAX_DELAY);

    /* Initialize DNS */
    vDNSInitialise();

    /* Call the function under test */
    DNSgetHostByName_cancel(pcHostName, &ulIPAddress, uxTicksToWait);

    /* Postconditions */
    /* Assuming that after cancellation, the IP address should not be modified or should be set to an invalid value */
    assert(ulIPAddress == 0 || ulIPAddress == 0xFFFFFFFF);
}