/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_Routing.h"

/* CBMC includes. */
#include "cbmc.h"

/* The ARP cache is defined in FreeRTOS_ARP.c */
extern ARPCacheRow_t xARPCache[ ipconfigARP_CACHE_ENTRIES ];

/* This pointer is maintained by the IP-task. Defined in FreeRTOS_IP.c */
extern NetworkBufferDescriptor_t * pxARPWaitingNetworkBuffer;

NetworkEndPoint_t * pxNetworkEndPoint_Temp;

/* Stub out FreeRTOS_FindEndPointOnNetMask as it is not relevant to
 * the correctness of this proof */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress )
{
    return pxNetworkEndPoint_Temp;
}

/* Stub out FreeRTOS_FindEndPointOnNetMask_IPv6 as it is not relevant to
 * the correctness of this proof */
NetworkEndPoint_t * FreeRTOS_FindEndPointOnNetMask_IPv6( const IPv6_Address_t * pxIPv6Address )
{
    __CPROVER_assert( pxIPv6Address != NULL, "Precondition: pxIPv6Address != NULL" );
    return pxNetworkEndPoint_Temp;
}

/* Get rid of configASSERT in FreeRTOS_TCP_IP.c */
BaseType_t xIsCallingFromIPTask( void )
{
    return pdTRUE;
}

/* This is an output function and need not be tested with this proof. */
void FreeRTOS_OutputARPRequest_Multi( NetworkEndPoint_t * pxEndPoint,
                                      uint32_t ulIPAddress )
{
    /* Intentionally empty stub */
}

void harness()
{
    /* Non-deterministically initialize the ARP cache entries so that
     * CBMC explores all possible states of the cache. */
    for( BaseType_t x = 0; x < ipconfigARP_CACHE_ENTRIES; x++ )
    {
        /* ucAge can be any value, including 0 (entry invalid) or
         * ipconfigMAX_ARP_AGE (fresh entry). */
        /* All fields are left unconstrained (non-deterministic). */
    }

    /* Non-deterministically set pxNetworkEndPoint_Temp to NULL or non-NULL */
    if( nondet_bool() )
    {
        pxNetworkEndPoint_Temp = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

        if( pxNetworkEndPoint_Temp != NULL )
        {
            pxNetworkEndPoint_Temp->pxNext = NULL;
        }
    }
    else
    {
        pxNetworkEndPoint_Temp = NULL;
    }

    /* ARPAgeCache takes no arguments; it operates on the global xARPCache. */
    ARPAgeCache();
}