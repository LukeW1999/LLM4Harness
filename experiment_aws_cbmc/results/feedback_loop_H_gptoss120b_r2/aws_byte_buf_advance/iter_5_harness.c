#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    /* Bound len to avoid overflow */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Buffer must be bounded and have an allocated backing store */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    buffer.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* Output must be a valid, empty byte buffer before the call */
    output.buffer = NULL;
    output.capacity = 0;
    output.len = 0;
    output.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_buffer = buffer;

    /* 3. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        /* Success: buffer.len decreased by len */
        assert(buffer.len == old_buffer.len - len);
        /* Unchanged fields of buffer */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        /* Buffer pointer advanced by len */
        assert(buffer.buffer == old_buffer.buffer + len);

        /* Output fields after success */
        assert(output.buffer == old_buffer.buffer);
        assert(output.capacity == len);
        assert(output.len == 0);
        assert(output.allocator == old_buffer.allocator);
    } else {
        /* Failure: buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* Output zeroed on failure */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);
    }

    /* 5. Invariant: both buffers remain valid */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
