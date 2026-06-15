#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

#include "aws/common/byte_buf.h"
#include "aws/common/allocator.h"
#include "aws/common/memory.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_harness() {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Allocate a non‑null allocator */
    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    __CPROVER_assume(allocator != NULL);

    /* 3. Non‑deterministic capacity, bounded */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 4. Save old state */
    struct aws_byte_buf old = buf;

    /* 5. Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 6. Result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* 7. Post‑conditions for the success path */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        /* 8. Post‑conditions for the failure path (zeroed struct) */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    /* 9. Invariant: the buffer must be valid after the call */
    assert(aws_byte_buf_is_valid(&buf));
}
