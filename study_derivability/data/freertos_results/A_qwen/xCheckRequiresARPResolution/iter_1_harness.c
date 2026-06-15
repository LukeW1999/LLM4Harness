/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_Routing.h"
/* CBMC includes. */
#include "cbmc.h"

/* Global variables. */
BaseType_t xIsIPv6;

/* Abstraction of xIsIPInARPCache. */
BaseType_t xIsIPInARPCache( uint32_t ulAddressToLookup )
{
    BaseType_t xReturn;
    xReturn = nondet_BaseType_t();
    return xReturn;
}

void xCheckRequiresARPResolution_harness()
{
    NetworkInterface_t * pxNetworkInterface;
    uint32_t ulIPAddress;
    BaseType_t xResult;

    /* Allocate nondeterministic inputs. */
    pxNetworkInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    ulIPAddress = nondet_uint32_t();

    /* Constrain inputs. */
    __CPROVER_assume( pxNetworkInterface != NULL );
    __CPROVER_assume( pxNetworkInterface->pxIPv4Settings != NULL );

    /* Call the function under test. */
    xResult = xCheckRequiresARPResolution( pxNetworkInterface, ulIPAddress );

    /* Assert postconditions. */
    if( xIsIPv6 == pdFALSE )
    {
        if( xIsIPInARPCache( ulIPAddress ) == pdTRUE )
        {
            assert( xResult == pdFALSE );
        }
        else
        {
            assert( xResult == pdTRUE );
        }
    }
    else
    {
        assert( xResult == pdFALSE );
    }

    /* Free allocated memory. */
    free( pxNetworkInterface );
}