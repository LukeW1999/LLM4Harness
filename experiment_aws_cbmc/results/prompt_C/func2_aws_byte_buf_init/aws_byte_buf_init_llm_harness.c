#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <aws/common/byte_buf.h>
#include <aws/common/malloc.h>

void ensure_aws_byte_buf_has_allocated_data_member(struct aws_byte_buf *buf, size_t capacity) {
    if (capacity > 0) {
        buf->buffer = malloc(capacity);
        __CPROVER_assume(buf->buffer != NULL);
    } else {
        buf->buffer = NULL;
    }
}

void aws_byte_buf_init_harness() {
    /* 1. Declare data structures */
    struct aws_byte_buf buf;
    struct aws_allocator allocator;
    size_t capacity;

    /* 2. Bound and allocate structures */
    __CPROVER_assume(capacity <= 1024); // Example bound for capacity
    ensure_aws_byte_buf_has_allocated_data_member(&buf, capacity);

    /* 3. Assume preconditions */
    __CPROVER_assume(&buf != NULL);
    __CPROVER_assume(&allocator != NULL);

    /* 4. Save old state if needed */
    struct aws_byte_buf old_buf = buf;

    /* 5. Call function under verification */
    int result = aws_byte_buf_init(&buf, &allocator, capacity);

    /* 6. Assert postconditions (both success and failure branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == &allocator);
        if (capacity > 0) {
            assert(buf.buffer != NULL);
        } else {
            assert(buf.buffer == NULL);
        }
    } else if (result == AWS_OP_ERR) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
        assert(buf.buffer == NULL);
    } else {
        assert(0); // Should not reach here
    }

    /* 7. Assert invariants (validity predicates) */
    assert(aws_byte_buf_is_valid(&buf));
}
