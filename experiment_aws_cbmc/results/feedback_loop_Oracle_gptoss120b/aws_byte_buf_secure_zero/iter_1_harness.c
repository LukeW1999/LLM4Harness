/* CBMC harness for aws_byte_buf_secure_zero */
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_CAPACITY 256U
#define GUARD_SIZE   8U

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;
    size_t capacity;
    size_t len;

    /* nondeterministic capacity and length */
    capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);
    len = nondet_size_t();
    __CPROVER_assume(len <= capacity);

    /* allocate raw memory with guards if capacity > 0 */
    uint8_t *raw = NULL;
    uint8_t *guard_before = NULL;
    uint8_t *guard_after  = NULL;
    uint8_t guard_before_orig[GUARD_SIZE];
    uint8_t guard_after_orig[GUARD_SIZE];

    if (capacity > 0) {
        raw = (uint8_t *)malloc(capacity + 2 * GUARD_SIZE);
        __CPROVER_assume(raw != NULL);

        guard_before = raw;
        guard_after  = raw + GUARD_SIZE + capacity;

        /* initialise guard bytes with nondet values */
        for (size_t i = 0; i < GUARD_SIZE; ++i) {
            guard_before[i] = nondet_uint8_t();
            guard_after[i]  = nondet_uint8_t();
        }

        /* keep copies of the original guard values */
        for (size_t i = 0; i < GUARD_SIZE; ++i) {
            guard_before_orig[i] = guard_before[i];
            guard_after_orig[i]  = guard_after[i];
        }

        /* initialise the buffer region with nondet data */
        for (size_t i = 0; i < capacity; ++i) {
            raw[GUARD_SIZE + i] = nondet_uint8_t();
        }
    }

    /* set up the aws_byte_buf structure */
    buf.buffer    = (capacity > 0) ? raw + GUARD_SIZE : NULL;
    buf.capacity  = capacity;
    buf.len       = len;
    buf.allocator = NULL;               /* allocator is not required for validity */

    /* pre‑condition: the buffer must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* call the function under test */
    aws_byte_buf_secure_zero(&buf);

    /* post‑conditions */
    assert(aws_byte_buf_is_valid(&buf));   /* still a valid buffer */
    assert(buf.len == 0);                  /* length reset to zero */

    if (capacity > 0) {
        /* all bytes up to capacity must be zero */
        for (size_t i = 0; i < capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }

        /* guard bytes must be unchanged */
        for (size_t i = 0; i < GUARD_SIZE; ++i) {
            assert(guard_before[i] == guard_before_orig[i]);
            assert(guard_after[i]  == guard_after_orig[i]);
        }
    } else {
        /* when capacity is zero the buffer pointer must be NULL */
        assert(buf.buffer == NULL);
    }

    /* clean up */
    free(raw);
    return 0;
}
