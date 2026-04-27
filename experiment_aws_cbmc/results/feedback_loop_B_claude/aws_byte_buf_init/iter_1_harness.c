#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "aws/common/byte_buf.h"
#include "aws/common/common.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_harness() {
    /* 1. Declare inputs */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    size_t capacity = nondet_size_t();

    /* 2. Preconditions: buf and allocator must be non-NULL (AWS_PRECONDITION) */
    __CPROVER_assume(allocator != NULL);

    /* Bound capacity to keep state space manageable */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 3. Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: len == 0, capacity == capacity, allocator == allocator */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        /* If capacity == 0, buffer must be NULL; otherwise buffer must be non-NULL */
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
        /* Validity invariant must hold */
        assert(aws_byte_buf_is_valid(&buf));
    } else {
        /* On failure (AWS_OP_ERR): AWS_ZERO_STRUCT was called, so all fields are zero/NULL */
        assert(result == AWS_OP_ERR);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
        /* capacity must have been non-zero for failure to occur */
        assert(capacity != 0);
        /* Validity invariant must still hold (zeroed struct is valid) */
        assert(aws_byte_buf_is_valid(&buf));
    }
}
