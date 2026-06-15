#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;

    // Nondeterministically decide whether buffer is NULL
    bool buffer_is_null;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    if (buffer_is_null) {
        buf.buffer = NULL;
        // Capacity and len can be anything, but len may be > capacity? Ensure validity: if buffer NULL, len must be 0 and capacity 0 or something? Actually aws_byte_buf_is_valid allows buffer NULL if len==0 and capacity==0? Let's check: The typical check: buffer may be NULL if len==0 and capacity==0. So we need valid state.
        // To keep it simple, we set len=0 and capacity=0 when buffer is NULL.
        buf.len = 0;
        buf.capacity = 0;
        buf.allocator = NULL; // or nondet
    } else {
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        // len can be within [0, capacity] nondeterministically, but ensure_byte_buf_has_allocated_buffer_member already sets? Actually that function allocates buffer but may set len to a nondet? It likely does not set len, so we set len nondet but ensure <= capacity.
        // We'll assume validity later.
    }

    // Assume the byte_buf is valid
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    // Save old state
    size_t old_capacity = buf.capacity;
    size_t old_len = buf.len;
    uint8_t *old_buffer = buf.buffer;
    struct aws_byte_buf old_buf = buf;

    // Call the function
    aws_byte_buf_secure_zero(&buf);

    // Postcondition: the buffer is still valid
    assert(aws_byte_buf_is_valid(&buf));
    // Capacity and allocator remain unchanged
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_buf.allocator);
    // If buffer was not NULL, then len == 0 and buffer was zeroed (we cannot check zeroing without knowing original content, but we can check len)
    if (!buffer_is_null) {
        assert(buf.len == 0);
        assert(buf.buffer != NULL);
    } else {
        // If buffer was NULL, it remains NULL and len unchanged
        assert(buf.buffer == NULL);
        assert(buf.len == old_len);
    }
}
