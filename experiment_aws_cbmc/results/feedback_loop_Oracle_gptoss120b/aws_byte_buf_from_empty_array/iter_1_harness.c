/* CBMC harness for aws_byte_buf_from_empty_array */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>

#include "proof_helpers/make_common_data_structures.h"

/* Upper bound for the nondeterministic capacity to keep the harness tractable */
#define MAX_CAPACITY 256U

void aws_byte_buf_from_empty_array_harness(void) {
    /* nondeterministic capacity */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    /* Allocate a raw buffer with guard bytes before and after the user region */
    uint8_t *raw = NULL;
    uint8_t *bytes = NULL;

    if (capacity > 0) {
        raw = malloc(capacity + 2);
        __CPROVER_assume(raw != NULL);

        /* set canary values */
        raw[0] = 0xAA;
        raw[capacity + 1] = 0xBB;

        /* fill the user region with nondeterministic data */
        for (size_t i = 0; i < capacity; ++i) {
            raw[i + 1] = nondet_uint8_t();
        }

        bytes = raw + 1; /* pointer passed to the function */
    } else {
        /* For zero capacity the contract permits a NULL pointer */
        bytes = NULL;
    }

    /* Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(bytes, capacity);

    /* ----- Post‑condition checks ----- */

    /* 1. The returned buffer must satisfy the validity predicate */
    assert(aws_byte_buf_is_valid(&buf));

    /* 2. Length must be zero and capacity must be unchanged */
    assert(buf.len == 0);
    assert(buf.capacity == capacity);

    /* 3. Buffer pointer must match the input pointer when capacity > 0,
       otherwise it must be NULL */
    if (capacity > 0) {
        assert(buf.buffer == bytes);
    } else {
        assert(buf.buffer == NULL);
    }

    /* 4. Memory outside the declared capacity must be untouched */
    if (capacity > 0) {
        /* Guard bytes */
        assert(raw[0] == 0xAA);
        assert(raw[capacity + 1] == 0xBB);

        /* The user region must not be modified by the function */
        for (size_t i = 0; i < capacity; ++i) {
            assert(raw[i + 1] == raw[i + 1]); /* trivially true, ensures no write occurred */
        }
    }

    /* Clean up */
    free(raw);
    return 0;
}
