#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_CAPACITY 256

void aws_byte_buf_from_empty_array_harness(void) {
    /* nondeterministic capacity */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    /* nondeterministic bytes pointer */
    uint8_t *bytes = NULL;
    if (capacity > 0) {
        bytes = malloc(capacity);
        __CPROVER_assume(bytes != NULL);
        /* ensure the allocated region is writable */
        __CPROVER_assume(__CPROVER_is_fresh(bytes, capacity));
    }

    /* PRE‑CALL SNAPSHOT */
    const void *orig_bytes = bytes;
    size_t orig_capacity = capacity;

    /* call the function under verification */
    struct aws_byte_buf result = aws_byte_buf_from_empty_array(bytes, capacity);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* result fields must reflect the input array */
    assert(result.buffer == orig_bytes);
    assert(result.len == 0);
    assert(result.capacity == orig_capacity);
    assert(result.allocator == NULL);

    /* input parameters must be unchanged */
    assert(bytes == orig_bytes);
    assert(capacity == orig_capacity);
}
