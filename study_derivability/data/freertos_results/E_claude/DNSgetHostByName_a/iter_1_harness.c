/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DNS_Private.h"
#include "FreeRTOS_IP_Private.h"

/* CBMC includes. */
#include "cbmc.h"

#ifndef HOSTNAME_SIZE
    #define HOSTNAME_SIZE    64
#endif

/* Stub for DNS_SendRequest which is proven separately. */
uint32_t DNS_SendRequest( Socket_t xDNSSocket,
                          struct freertos_addrinfo ** ppxAddressInfo,
                          const char * pcHostName,
                          TickType_t uxIdentifier )
{
    return nondet_uint32();
}

/* Stub for FreeRTOS_inet_addr which is proven separately. */
uint32_t FreeRTOS_inet_addr( const char * pcIPAddress )
{
    return nondet_uint32();
}

/* Stub for DNS_ReadReply which is proven separately. */
uint32_t DNS_ReadReply( ConstSocket_t xDNSSocket,
                        struct freertos_addrinfo ** ppxAddressInfo )
{
    return nondet_uint32();
}

/* Stub for Sockets_DNS_CreateSocket which is proven separately. */
Socket_t Sockets_DNS_CreateSocket( void )
{
    Socket_t xSocket;
    return xSocket;
}

/* Stub for Sockets_DNS_CloseSocket. */
void Sockets_DNS_CloseSocket( Socket_t xDNSSocket )
{
}

/* Stub for DNS_CreateSocket. */
Socket_t DNS_CreateSocket( TickType_t uxReadTimeOut_ticks )
{
    Socket_t xSocket;
    return xSocket;
}

/* Stub for DNS_CloseSocket. */
void DNS_CloseSocket( Socket_t xDNSSocket )
{
}

/* Stub for DNS_GetHostByNameOp. */
uint32_t DNS_GetHostByNameOp( const char * pcHostName,
                               TickType_t uxIdentifier,
                               Socket_t xDNSSocket,
                               struct freertos_addrinfo ** ppxAddressInfo,
                               BaseType_t xFamily )
{
    return nondet_uint32();
}

/* Stub for FreeRTOS_dns_update. */
uint32_t FreeRTOS_dns_update( const char * pcName,
                               uint32_t * pulIP,
                               uint32_t ulTTL,
                               BaseType_t xLookUp,
                               struct freertos_addrinfo ** ppxAddressInfo )
{
    return nondet_uint32();
}

/* Stub for prvGetHostByName. */
uint32_t prvGetHostByName( const char * pcHostName,
                            TickType_t uxIdentifier,
                            TickType_t uxTimeout,
                            struct freertos_addrinfo ** ppxAddressInfo,
                            BaseType_t xFamily )
{
    return nondet_uint32();
}

void DNSgetHostByName_a_harness()
{
    /* Allocate a hostname with bounded size. */
    char * pcHostName = malloc( HOSTNAME_SIZE );
    __CPROVER_assume( pcHostName != NULL );

    /* Ensure the hostname is null-terminated within bounds. */
    pcHostName[ HOSTNAME_SIZE - 1 ] = '\0';

    /* Callback function pointer - can be NULL or non-NULL. */
    FOnDNSEvent pCallback;

    /* User-supplied callback parameter. */
    void * pvSearchID;

    /* Timeout value. */
    TickType_t uxTimeout;

    /* Address info pointer. */
    struct freertos_addrinfo * pxAddressInfo = NULL;

    /* Family - either FREERTOS_AF_INET or FREERTOS_AF_INET6. */
    BaseType_t xFamily;

    /* Call the function under test. */
    DNSgetHostByName_a( pcHostName,
                        pCallback,
                        pvSearchID,
                        uxTimeout,
                        xFamily );
}