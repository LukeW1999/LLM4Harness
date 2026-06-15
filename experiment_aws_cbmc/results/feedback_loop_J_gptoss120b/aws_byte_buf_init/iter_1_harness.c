#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old = buf;

    /* 3. Prepare arguments */
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 4. Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 5. Post‑conditions for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* Successful initialization */
        assert(buf.capacity == capacity);
        assert(buf.len == 0);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        /* Allocation failed: struct is zeroed */
        assert(result == AWS_OP_ERR);
        assert(buf.capacity == 0);
        assert(buf.len == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    /* 6. Validity invariant must always hold */
    assert(aws_byte_buf_is_valid(&buf));
}
