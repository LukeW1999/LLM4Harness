#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_advance_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output = {0}; /* initialize to zero */

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 2. Ensure buffer has a non‑NULL underlying array */
    __CPROVER_assume(buffer.buffer != NULL);

    /* 3. Nondeterministic length, bounded and within buffer */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len <= buffer.len); /* precondition for success */

    /* 4. Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* 5. Call function under test */
    int result = aws_byte_buf_advance(&buffer, &output, len);

    /* 6. Assert postconditions (success expected) */
    assert(result == 0);
    assert(buffer.len == old_buffer.len - len);
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);
    assert(buffer.buffer == old_buffer.buffer + len);

    assert(output.buffer == old_buffer.buffer);
    assert(output.len == len);
    assert(output.capacity == len);
    assert(output.allocator == NULL);

    /* 7. Validity invariants */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
