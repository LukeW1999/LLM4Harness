#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_CAPACITY 1024

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t capacity;
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    /* Result must be a known error code */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Frame condition: allocator stored correctly */
    assert(buf.allocator == alloc);

    /* Length invariant: new buffer length is always zero */
    assert(buf.len == 0);

    /* Capacity must match the requested capacity */
    assert(buf.capacity == capacity);

    /* Buffer pointer condition based on capacity and result */
    if (capacity == 0) {
        /* Zero capacity always yields a NULL buffer */
        assert(buf.buffer == NULL);
    } else {
        if (result == AWS_OP_SUCCESS) {
            /* Successful allocation yields a non‑NULL buffer */
            assert(buf.buffer != NULL);
        } else {
            /* Allocation failure yields a NULL buffer */
            assert(buf.buffer == NULL);
        }
    }

    /* Validity predicate: the resulting buffer must be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
