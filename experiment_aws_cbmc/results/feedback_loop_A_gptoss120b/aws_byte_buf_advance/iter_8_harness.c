#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&output);
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* Ensure source buffer pointer is not NULL */
    __CPROVER_assume(buffer.buffer != NULL);

    /* Constrain len so that the advance operation can succeed */
    __CPROVER_assume(len <= buffer.len);
    __CPROVER_assume(output.capacity >= len);

    /* 2. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 3. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 4. The operation must succeed under the assumed pre‑conditions */
    assert(result);

    /* 5. Post‑conditions for the successful case */
    if (len > 0) {
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.buffer == old_buffer.buffer + len);
    } else {
        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);
    }
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);

    /* output now contains a copy of the advanced slice */
    assert(output.len == len);
    if (old_output.buffer == NULL) {
        assert(output.buffer == old_buffer.buffer);
    } else {
        assert(output.buffer == old_output.buffer);
    }
    assert(output.capacity == old_output.capacity);
    assert(output.allocator == old_output.allocator);

    /* 6. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
