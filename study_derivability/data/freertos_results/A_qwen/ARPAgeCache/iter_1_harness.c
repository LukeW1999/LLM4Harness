#include "FreeRTOS.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"

void ARPAgeCache_harness()
{
    /* Nondeterministic inputs */
    TickType_t xTimeNow = nondet_TickType_t();
    ARP_CACHE_T * pxARP_Cache = (ARP_CACHE_T *)malloc(sizeof(ARP_CACHE_T));
    __CPROVER_assume(pxARP_Cache != NULL);
    pxARP_Cache->uxCacheEntries = nondet_uint16_t();
    __CPROVER_assume(pxARP_Cache->uxCacheEntries <= ipARP_CACHE_ENTRIES);
    for (UBaseType_t i = 0; i < pxARP_Cache->uxCacheEntries; i++) {
        pxARP_Cache->xARPCache[i].ulIPAddress = nondet_uint32_t();
        pxARP_Cache->xARPCache[i].ucMACAddress[0] = nondet_uint8_t();
        pxARP_Cache->xARPCache[i].ucMACAddress[1] = nondet_uint8_t();
        pxARP_Cache->xARPCache[i].ucMACAddress[2] = nondet_uint8_t();
        pxARP_Cache->xARPCache[i].ucMACAddress[3] = nondet_uint8_t();
        pxARP_Cache->xARPCache[i].ucMACAddress[4] = nondet_uint8_t();
        pxARP_Cache->xARPCache[i].ucMACAddress[5] = nondet_uint8_t();
        pxARP_Cache->xARPCache[i].ucAge = nondet_uint8_t();
        pxARP_Cache->xARPCache[i].ucValid = nondet_uint8_t();
    }

    /* Call the function under test */
    ARPAgeCache(pxARP_Cache, xTimeNow);

    /* Postconditions */
    for (UBaseType_t i = 0; i < pxARP_Cache->uxCacheEntries; i++) {
        if (pxARP_Cache->xARPCache[i].ucAge > ipARP_MAX_AGE) {
            assert(pxARP_Cache->xARPCache[i].ucValid == pdFALSE);
        }
    }
}