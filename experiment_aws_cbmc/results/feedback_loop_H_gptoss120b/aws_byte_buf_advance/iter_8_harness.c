#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;

    /* 2. Set up a valid allocator */
    buffer.allocator = aws_default_allocator();

    /* 3. Nondeterministically choose capacity and length within bounds */
    buffer.capacity = nondet_size_t();
    __CPROVER_assume(buffer.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buffer.capacity > 0);

    buffer.len = nondet_size_t();
    __CPROVER_assume(buffer.len <= buffer.capacity);

    /* 4. Allocate the underlying buffer memory */
    ensure_byte_buf_has_allocated_buffer_member(&buffer);

    /* 5. Ensure the main buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 6. Choose a nondeterministic advance length that fits */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= buffer.capacity - buffer.len);

    /* 7. Save old state for later comparison */
    struct aws_byte_buf old_buffer = buffer;

    /* 8. Initialise output to a valid zeroed state */
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* 9. Call the function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 10. Assert post‑conditions */
    if (result) {
        /* Success: buffer advanced */
        assert(buffer.len == old_buffer.len + len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* Success: output initialized */
        assert(output.len == len);
        assert(output.capacity == len);
        assert(output.buffer == old_buffer.buffer + old_buffer.len);
        assert(output.allocator == old_buffer.allocator);
    } else {
        /* Failure: buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* Failure: output zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);
    }

    /* 11. Invariants must always hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
