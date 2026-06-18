/* CBMC harness for aws_byte_buf_from_empty_array */

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

/* nondeterministic helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_buf_from_empty_array_harness(void) {
    /* limit the size to keep the verification tractable */
    const size_t MAX_CAPACITY = 256;

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    uint8_t *bytes = NULL;
    uint8_t *bytes_copy = NULL;
    uint8_t canary = 0xAA; /* sentinel value placed just after the buffer */

    if (capacity > 0) {
        /* allocate space for the buffer plus a canary byte */
        bytes = malloc(capacity + 1);
        __CPROVER_assume(bytes != NULL);

        /* allocate a copy to check that the function does not modify the input */
        bytes_copy = malloc(capacity);
        __CPROVER_assume(bytes_copy != NULL);

        /* initialise the buffer with nondeterministic data */
        for (size_t i = 0; i < capacity; ++i) {
            bytes[i] = nondet_uint8_t();
            bytes_copy[i] = bytes[i];
        }

        /* set the canary byte */
        bytes[capacity] = canary;
    } else {
        /* when capacity is zero the pointer may be NULL */
        bytes = NULL;
        bytes_copy = NULL;
    }

    /* call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(bytes, capacity);

    /* ----- post‑condition checks ----- */

    /* 1. The returned buffer must satisfy its validity predicate */
    assert(aws_byte_buf_is_valid(&buf));

    /* 2. Length must be zero and capacity must be unchanged */
    assert(buf.len == 0);
    assert(buf.capacity == capacity);

    /* 3. Buffer pointer must match the supplied array (or be NULL when capacity is zero) */
    if (capacity > 0) {
        assert(buf.buffer == bytes);
    } else {
        assert(buf.buffer == NULL);
    }

    /* 4. The function must not modify the contents of the input array */
    if (capacity > 0) {
        for (size_t i = 0; i < capacity; ++i) {
            assert(bytes[i] == bytes_copy[i]);
        }
    }

    /* 5. The canary byte placed after the buffer must remain unchanged */
    if (capacity > 0) {
        assert(bytes[capacity] == canary);
    }

    /* clean up */
    free(bytes);
    free(bytes_copy);
    return 0;
}
