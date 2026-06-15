/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"

/* Mock implementation of pxGetNetworkBufferWithDescriptor */
NetworkBufferDescriptor_t * pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes,
                                                              TickType_t xBlockTimeTicks )
{
    NetworkBufferDescriptor_t * pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );

    __CPROVER_assume( pxNetworkBuffer != NULL );

    pxNetworkBuffer->pucEthernetBuffer = malloc( xRequestedSizeBytes );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );

    pxNetworkBuffer->xDataLength = xRequestedSizeBytes;
    return pxNetworkBuffer;
}

/* Stub for NetworkInterfaceOutputFunction */
BaseType_t NetworkInterfaceOutputFunction_Stub( struct xNetworkInterface * pxDescriptor,
                                                NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                                BaseType_t xReleaseAfterSend )
{
    __CPROVER_assert( pxDescriptor != NULL, "The network interface cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer != NULL, "The network buffer descriptor cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer->pucEthernetBuffer != NULL, "The Ethernet buffer cannot be NULL." );
    return 0;
}

/* Harness function for ARPProcessPacket */
void ARPProcessPacket_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    size_t xRequestedSizeBytes = 64; // Example size for an ARP packet

    /* Allocate and initialize the network buffer */
    pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( xRequestedSizeBytes, 0 );
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Initialize the Ethernet buffer with some dummy data */
    for( size_t i = 0; i < xRequestedSizeBytes; i++ )
    {
        pxNetworkBuffer->pucEthernetBuffer[i] = ( uint8_t ) nondet_uint8();
    }

    /* Assume valid network endpoints and interfaces */
    pxNetworkEndPoints = ( NetworkEndPoint_t * ) malloc( sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxNetworkEndPoints != NULL );

    /* Interface init. */
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

    pxNetworkEndPoints->pxNetworkInterface->pfOutput = NetworkInterfaceOutputFunction_Stub;

    /* Call the function under test */
    ARPProcessPacket( pxNetworkBuffer );
}