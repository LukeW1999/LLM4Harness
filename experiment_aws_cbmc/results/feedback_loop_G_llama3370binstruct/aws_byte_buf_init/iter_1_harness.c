#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 4. Assert postconditions for BOTH success and failure paths */
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
        assert(buf.len == old.len);
        assert(buf.capacity == old.capacity);
        assert(buf.allocator == old.allocator);
        assert(buf.buffer == old.buffer);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(buf.allocator == allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
