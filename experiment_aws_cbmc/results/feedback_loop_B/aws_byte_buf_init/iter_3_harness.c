#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 2. Initialize the buffer to a known state */
    buf.buffer = NULL;
    buf.len = 0;
    buf.capacity = 0;
    buf.allocator = NULL;

    /* 3. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old = buf;

    /* 4. Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.buffer != NULL || capacity == 0);
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
    } else {
        assert(buf.buffer == old.buffer);
        assert(buf.len == old.len);
        assert(buf.capacity == old.capacity);
        assert(buf.allocator == old.allocator);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(buf.allocator == allocator);
    assert(buf.capacity == capacity);

    /* 7. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
