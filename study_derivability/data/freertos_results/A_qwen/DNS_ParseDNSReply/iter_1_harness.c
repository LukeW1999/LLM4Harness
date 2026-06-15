/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DNS_Parser.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

void DNS_ParseDNSReply_harness()
{
    uint8_t *pucUDPPayload;
    size_t uxBufferLength;
    DNSMessage_t xDNSStructure;
    size_t uxSourceAddressLength;

    /* Allocate nondeterministic inputs */
    pucUDPPayload = (uint8_t *)malloc(nondet_uint());
    uxBufferLength = nondet_uint();
    uxSourceAddressLength = nondet_uint();

    /* Constrain inputs */
    __CPROVER_assume(pucUDPPayload != NULL);
    __CPROVER_assume(uxBufferLength > 0 && uxBufferLength <= 1500); /* Typical MTU size */
    __CPROVER_assume(uxSourceAddressLength == sizeof( IPv4_Address_t ) || uxSourceAddressLength == sizeof( IPv6_Address_t ));

    /* Initialize the DNSMessage_t structure */
    xDNSStructure.pucPayloadBuffer = pucUDPPayload;
    xDNSStructure.usPayloadSize = uxBufferLength;
    xDNSStructure.ulIPAddress = 0;
    xDNSStructure.pcName = (char *)malloc(nondet_uint());
    xDNSStructure.usNameLength = nondet_ushort();
    xDNSStructure.usType = nondet_ushort();
    xDNSStructure.usClass = nondet_ushort();
    xDNSStructure.ulTTL = nondet_ulong();
    xDNSStructure.usAnswerCount = nondet_ushort();
    xDNSStructure.usAuthorityRRs = nondet_ushort();
    xDNSStructure.usAdditionalRRs = nondet_ushort();
    xDNSStructure.usFlags = nondet_ushort();

    /* Call the function under test */
    bool xResult = DNS_ParseDNSReply(&xDNSStructure, uxSourceAddressLength);

    /* Assert postconditions */
    assert(xResult == pdPASS || xResult == pdFAIL);
    if (xResult == pdPASS) {
        assert(xDNSStructure.ulIPAddress != 0);
        assert(xDNSStructure.pcName != NULL);
        assert(xDNSStructure.usNameLength > 0);
        assert(xDNSStructure.usType == dnsTYPE_A || xDNSStructure.usType == dnsTYPE_AAAA);
        assert(xDNSStructure.usClass == dnsCLASS_IN);
        assert(xDNSStructure.ulTTL > 0);
    }
    free(pucUDPPayload);
    free(xDNSStructure.pcName);
}