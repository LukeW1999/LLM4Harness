/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"

/* Mock for eARPGetCacheEntry if needed */
eARPLookupResult_t eARPGetCacheEntry( uint32_t * pulIPAddress,
                                      MACAddress_t * const pxMACAddress,
                                      struct xNetworkEndPoint ** ppxEndPoint )
{
    eARPLookupResult_t eResult;
    return eResult;
}

void harness()
{
    /* Allocate a network buffer descriptor */
    NetworkBufferDescriptor_t * pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Allocate Ethernet buffer large enough for IP packet */
    size_t xDataLength;
    __CPROVER_assume( xDataLength >= sizeof( IPPacket_t ) );
    __CPROVER_assume( xDataLength <= ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER );

    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) malloc( xDataLength );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = xDataLength;

    /* Setup network endpoints */
    pxNetworkEndPoints = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxNetworkEndPoints != NULL );

    pxNetworkEndPoints->pxNetworkInterface = ( NetworkInterface_t * ) malloc( sizeof( NetworkInterface_t ) );
    __CPROVER_assume( pxNetworkEndPoints->pxNetworkInterface != NULL );

    if( nondet_bool() )
    {
        pxNetworkEndPoints->pxNext = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
        __CPROVER_assume( pxNetworkEndPoints->pxNext != NULL );
        pxNetworkEndPoints->pxNext->pxNext = NULL;
        pxNetworkEndPoints->pxNext->pxNetworkInterface = pxNetworkEndPoints->pxNetworkInterface;
    }
    else
    {
        pxNetworkEndPoints->pxNext = NULL;
    }

    /* Associate an endpoint with the buffer */
    pxNetworkBuffer->pxEndPoint = pxNetworkEndPoints;

    /* Call the function under test */
    BaseType_t xResult = xCheckRequiresARPResolution( pxNetworkBuffer );

    /* The result should be pdTRUE or pdFALSE */
    __CPROVER_assert( ( xResult == pdTRUE ) || ( xResult == pdFALSE ),
                      "xCheckRequiresARPResolution must return pdTRUE or pdFALSE." );
}