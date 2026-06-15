/* CBMC harness for aws_byte_buf_reset */

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_reset_harness(void) {
    /* Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Create a byte buffer with nondeterministic capacity (bounded) */
    size_t max_capacity = 64U;                     /* bound for CBMC */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= max_capacity);

    struct aws_byte_buf buf;
    if (aws_byte_buf_init(&buf, alloc, capacity) != AWS_OP_SUCCESS) {
        /* Allocation failed – nothing to test */
        return 0;
    }

    /* Set a nondeterministic length that respects the capacity */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    /* Fill the allocated region with nondeterministic data */
    if (capacity > 0) {
        uint8_t *tmp = (uint8_t *)aws_mem_acquire(alloc, capacity);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < capacity; ++i) {
            tmp[i] = nondet_uint8_t();
        }
        memcpy(buf.buffer, tmp, capacity);
        aws_mem_release(alloc, tmp);
    }

    /* Preserve a copy of the original contents for later comparison */
    uint8_t *original_contents = NULL;
    if (capacity > 0) {
        original_contents = (uint8_t *)malloc(capacity);
        __CPROVER_assume(original_contents != NULL);
        memcpy(original_contents, buf.buffer, capacity);
    }

    /* Nondeterministic flag indicating whether to zero the contents */
    bool zero_contents = nondet_bool();

    /* Call the function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* -------------------------------------------------------------------- */
    /* Post‑condition checks                                                */
    /* -------------------------------------------------------------------- */

    /* The buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* Length must be reset to zero, capacity and allocator unchanged */
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == alloc);

    if (capacity > 0) {
        if (zero_contents) {
            /* All bytes up to capacity must be zeroed */
            for (size_t i = 0; i < capacity; ++i) {
                assert(buf.buffer[i] == 0);
            }
        } else {
            /* Contents must be unchanged */
            for (size_t i = 0; i < capacity; ++i) {
                assert(buf.buffer[i] == original_contents[i]);
            }
        }
    } else {
        /* When capacity is zero the buffer pointer must be NULL */
        assert(buf.buffer == NULL);
    }

    /* -------------------------------------------------------------------- */
    /* Clean‑up                                                             */
    /* -------------------------------------------------------------------- */
    if (original_contents != NULL) {
        free(original_contents);
    }
    aws_byte_buf_clean_up(&buf);
    return 0;
}
