/*
 * FreeRTOS memory safety proofs with CBMC.
 * Copyright (C) 2022 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DNS_Globals.h"

/* CBMC proof utilities */
#include "cbmc.h"

#ifndef ipconfigUSE_NBNS
    #define ipconfigUSE_NBNS    1
#endif

/* Maximum buffer length for NBNS name */
#ifndef NBNS_NAME_MAX_LEN
    #define NBNS_NAME_MAX_LEN    ( 32U )
#endif

/* The minimum size for an NBNS payload buffer to be valid */
#ifndef DNS_BUFFER_MAX
    #define DNS_BUFFER_MAX    ( ipconfigNETWORK_MTU )
#endif

void DNS_TreatNBNS( uint8_t * pucPayload,
                    size_t uxBufferLength,
                    uint32_t ulIPAddress );

void DNS_TreatNBNS_harness( void )
{
    uint8_t * pucPayload;
    size_t uxBufferLength;
    uint32_t ulIPAddress;

    /* Create a nondeterministic buffer length, bounded to a reasonable size */
    size_t nondet_len;

    __CPROVER_assume( nondet_len > 0 );
    __CPROVER_assume( nondet_len <= DNS_BUFFER_MAX );

    uxBufferLength = nondet_len;

    /* Allocate payload buffer of the given length */
    pucPayload = ( uint8_t * ) malloc( uxBufferLength );
    __CPROVER_assume( pucPayload != NULL );

    /* ulIPAddress is fully nondeterministic */
    ulIPAddress = nondet_uint32_t();

    /* Call the function under test */
    DNS_TreatNBNS( pucPayload, uxBufferLength, ulIPAddress );

    /* No specific postconditions to assert for this function,
     * as it operates on network buffers and sends responses.
     * CBMC will verify memory safety automatically. */
}