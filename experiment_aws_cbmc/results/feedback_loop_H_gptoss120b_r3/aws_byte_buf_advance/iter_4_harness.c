#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    /* 2. Bound the buffers */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));

    /* 3. Allocate memory for the input buffer and set a valid allocator */
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    buffer.allocator = aws_default_allocator();

    /* 4. Strengthen invariants for the source buffer */
    __CPROVER_assume(buffer.capacity > 0);
    __CPROVER_assume(buffer.buffer != NULL);
    __CPROVER_assume(buffer.len <= buffer.capacity);
    __CPROVER_assume(buffer.allocator != NULL);

    /* 5. Initialise the output buffer (it will be overwritten on success) */
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;

    /* 6. Assume the buffers are valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* 7. Nondeterministic length, bounded and not larger than the source buffer */
    len = nondet_size_t();
    __CPROVER_assume(len > 0);
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len <= buffer.len);
    __CPROVER_assume(len <= buffer.capacity);

    /* 8. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 9. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 10. Post‑condition checks */
    if (result) {
        /* Success: input buffer advanced */
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer + len);

        /* Success: output buffer describes the advanced slice */
        assert(output.buffer == old_buffer.buffer);
        assert(output.len == len);
        assert(output.capacity == len);
        assert(output.allocator == old_buffer.allocator);
    } else {
        /* Failure: input buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        /* Failure: output buffer unchanged */
        assert(output.buffer == old_output.buffer);
        assert(output.len == old_output.len);
        assert(output.capacity == old_output.capacity);
        assert(output.allocator == old_output.allocator);
    }

    /* 11. Invariant holds after the call */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
