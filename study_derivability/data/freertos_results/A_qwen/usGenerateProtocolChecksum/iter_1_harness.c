#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <cbmc_proof/nondet.h>

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

uint16_t usGenerateProtocolChecksum( const uint8_t * ucBuffer, size_t uxLength, BaseType_t xOutgoingPacket );

void usGenerateProtocolChecksum_harness()
{
    size_t uxLength = nondet_size_t();
    __CPROVER_assume( uxLength > 0 && uxLength <= 1500 ); // Assuming a reasonable packet size

    uint8_t *ucBuffer = malloc( uxLength );
    __CPROVER_assume( ucBuffer != NULL );

    for( size_t i = 0; i < uxLength; i++ )
    {
        ucBuffer[ i ] = nondet_uint8_t();
    }

    BaseType_t xOutgoingPacket = nondet_BaseType_t();

    uint16_t result = usGenerateProtocolChecksum( ucBuffer, uxLength, xOutgoingPacket );

    // Postcondition: The result should be a valid 16-bit checksum
    assert( result <= 0xFFFF );

    free( ucBuffer );
}