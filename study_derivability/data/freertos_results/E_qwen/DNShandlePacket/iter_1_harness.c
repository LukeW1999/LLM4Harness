void DNShandlePacket_harness()
{
    NetworkBufferDescriptor_t xNetworkBuffer;
    uint32_t ulIPAddress;

    NetworkEndPoint_t * pxNetworkEndPoint_Temp = ( NetworkEndPoint_t * ) safeMalloc( sizeof( NetworkEndPoint_t ) );

    BaseType_t xDataSize;

    __CPROVER_assume( ( xDataSize > 0 ) && ( xDataSize < ( ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER ) ) );

    xNetworkBuffer.pucEthernetBuffer = safeMalloc( xDataSize );
    xNetworkBuffer.xDataLength = xDataSize;
    __CPROVER_assume( xNetworkBuffer.pucEthernetBuffer != NULL );

    if( nondet_bool() )
    {
        __CPROVER_assume( pxNetworkEndPoint_Temp != NULL );
        xNetworkBuffer.pxEndPoint = pxNetworkEndPoint_Temp;
    }
    else
    {
        xNetworkBuffer.pxEndPoint = NULL;
    }

    DNShandlePacket( &xNetworkBuffer, ulIPAddress );
}