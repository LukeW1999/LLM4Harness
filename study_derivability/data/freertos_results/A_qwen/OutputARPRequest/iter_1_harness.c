#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "NetworkInterface.h"

void OutputARPRequest_harness()
{
    NetworkBufferDescriptor_t * pxNetworkBuffer;
    ARPHeader_t * pxARPHeader;
    uint8_t ucSenderIP[ ipIP_ADDRESS_LENGTH_BYTES ];
    uint8_t ucTargetIP[ ipIP_ADDRESS_LENGTH_BYTES ];
    uint8_t ucSenderMAC[ ipMAC_ADDRESS_LENGTH_BYTES ];

    // Allocate non-deterministic inputs
    pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) malloc( sizeof( NetworkBufferDescriptor_t ) );
    pxARPHeader = ( ARPHeader_t * ) malloc( sizeof( ARPHeader_t ) );
    for( size_t i = 0; i < ipIP_ADDRESS_LENGTH_BYTES; i++ )
    {
        ucSenderIP[ i ] = nondet_uint8();
        ucTargetIP[ i ] = nondet_uint8();
    }
    for( size_t i = 0; i < ipMAC_ADDRESS_LENGTH_BYTES; i++ )
    {
        ucSenderMAC[ i ] = nondet_uint8();
    }

    // Constrain inputs
    __CPROVER_assume( pxNetworkBuffer != NULL );
    __CPROVER_assume( pxARPHeader != NULL );
    __CPROVER_assume( pxNetworkBuffer->pucEthernetBuffer != NULL );
    __CPROVER_assume( pxNetworkBuffer->xDataLength >= sizeof( ARPPacket_t ) );

    // Initialize network buffer
    pxNetworkBuffer->pucEthernetBuffer = ( uint8_t * ) pxARPHeader;

    // Call the function under test
    eARPGetCacheEntry( ucTargetIP, &ucSenderMAC[ 0 ] );
    vARPGenerateRequest( pxNetworkBuffer, ucSenderIP, ucTargetIP );

    // Assert postconditions
    assert( pxARPHeader->usOperation == ipARP_REQUEST );
    assert( memcmp( pxARPHeader->ucSenderProtocolAddress, ucSenderIP, ipIP_ADDRESS_LENGTH_BYTES ) == 0 );
    assert( memcmp( pxARPHeader->ucTargetProtocolAddress, ucTargetIP, ipIP_ADDRESS_LENGTH_BYTES ) == 0 );
    assert( memcmp( pxARPHeader->ucSenderHardwareAddress, ucSenderMAC, ipMAC_ADDRESS_LENGTH_BYTES ) == 0 );

    // Free allocated memory
    free( pxNetworkBuffer );
    free( pxARPHeader );
}