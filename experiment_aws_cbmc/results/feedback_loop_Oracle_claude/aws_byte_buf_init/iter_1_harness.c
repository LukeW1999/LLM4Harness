#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_harness(void) {
    /* Allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Unconstrained capacity */
    size_t capacity;

    /* Output buffer - must be non-NULL (precondition) */
    struct aws_byte_buf buf;

    /* Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* Postcondition 1: Return value correctness */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* Postcondition 1a: On success, validity predicate holds */
        assert(aws_byte_buf_is_valid(&buf));

        /* Postcondition 2: Length invariants on success */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.len <= buf.capacity);

        /* Postcondition 2a: Buffer pointer invariant */
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }

        /* Postcondition 3: Frame condition - allocator is set correctly */
        assert(buf.allocator == allocator);

    } else {
        /* Postcondition 1b: On failure, capacity must have been non-zero
           (zero capacity never fails) */
        assert(capacity != 0);

        /* Postcondition 2b: On failure, buf is zeroed out */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }
}
