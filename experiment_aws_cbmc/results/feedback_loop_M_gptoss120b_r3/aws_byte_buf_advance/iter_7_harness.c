#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_advance_harness() {
    /* 1. Declare data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output = {0};

    /* 2. Impose bounds and validity */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 3. Ensure underlying array is non‑NULL and allocator is NULL (required by advance) */
    __CPROVER_assume(buffer.buffer != NULL);
    buffer.allocator = NULL;

    /* 4. Relate capacity and length */
    __CPROVER_assume(buffer.capacity >= buffer.len);
    __CPROVER_assume(buffer.capacity <= MAX_BUFFER_SIZE);

    /* 5. Choose a nondeterministic length that satisfies the precondition */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= buffer.len);
    __CPROVER_assume(len <= buffer.capacity);

    /* 6. Save old state for later checks */
    struct aws_byte_buf old_buffer = buffer;

    /* 7. Call the function under test */
    int result = aws_byte_buf_advance(&buffer, &output, len);

    /* 8. Expected successful outcome */
    assert(result == 0);
    assert(buffer.len == old_buffer.len - len);
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);
    assert(buffer.buffer == old_buffer.buffer + len);

    assert(output.buffer == old_buffer.buffer);
    assert(output.len == len);
    assert(output.capacity == len);
    assert(output.allocator == NULL);

    /* 9. Preserve invariants */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
