#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_CAPACITY 1024

void aws_byte_buf_from_empty_array_harness(void) {
    /* Destination buffer */
    struct aws_byte_buf buf;

    /* Non‑deterministic capacity */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    /* Non‑deterministic allocator (may be NULL to force failure) */
    struct aws_allocator *alloc;
    __CPROVER_assume(alloc != NULL || alloc == NULL);

    /* Call the function under test */
    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    /* Post‑condition checks */
    if (alloc == NULL) {
        /* Allocation failed because allocator is NULL */
        assert(result != AWS_OP_SUCCESS);
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    } else {
        /* Allocator is non‑NULL, function should succeed */
        assert(result == AWS_OP_SUCCESS);
        assert(buf.allocator == alloc);

        if (capacity == 0) {
            /* Zero capacity → empty buffer */
            assert(buf.buffer == NULL);
            assert(buf.len == 0);
            assert(buf.capacity == 0);
        } else {
            /* Non‑zero capacity → buffer allocated */
            assert(buf.buffer != NULL);
            assert(buf.len == 0);
            assert(buf.capacity == capacity);
        }
    }

    /* Validity invariant must always hold */
    assert(aws_byte_buf_is_valid(&buf));
}
