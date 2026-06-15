/*  
 * Contract for aws_byte_buf_from_array  
 * Preconditions:  
 *   - len is any size_t value (including 0).  
 *   - If len > 0, bytes points to a writable memory region of at least len bytes.  
 *   - If len == 0, bytes may be NULL.  
 * Postconditions (validity):  
 *   - The returned aws_byte_buf satisfies aws_byte_buf_is_valid(&buf).  
 *   - buf.allocator == NULL.  
 *   - buf.len == len and buf.capacity == len.  
 *   - If len == 0 then buf.buffer == NULL, else buf.buffer == (uint8_t *)bytes.  
 * Postconditions (frame):  
 *   - The memory region pointed to by bytes is not modified by the function.  
 *   - No other memory locations are modified.  
 */

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to generate a nondeterministic size_t */
static size_t nondet_size_t(void);
static uint8_t nondet_uint8_t(void);

void aws_byte_buf_from_array_harness(void) {
    /* Nondeterministic length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 1024); /* bound to keep verification tractable */

    /* Allocate a buffer for the input array if needed */
    uint8_t *bytes = NULL;
    uint8_t *bytes_copy = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
        bytes_copy = malloc(len);
        __CPROVER_assume(bytes_copy != NULL);
        /* Fill with nondeterministic data */
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        /* Preserve a copy to check for modifications */
        memcpy(bytes_copy, bytes, len);
    }

    /* Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array((const void *)bytes, len);

    /* Verify postconditions */
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    if (len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == bytes);
    }
    assert(aws_byte_buf_is_valid(&buf));

    /* Verify that the input memory was not modified */
    if (len > 0) {
        for (size_t i = 0; i < len; ++i) {
            assert(bytes[i] == bytes_copy[i]);
        }
    }

    /* Clean up */
    free(bytes);
    free(bytes_copy);
    return 0;
}

/* Definitions of nondeterministic helpers for CBMC */
size_t nondet_size_t(void) {
    size_t x;
    return x;
}
uint8_t nondet_uint8_t(void) {
    uint8_t x;
    return x;
}
