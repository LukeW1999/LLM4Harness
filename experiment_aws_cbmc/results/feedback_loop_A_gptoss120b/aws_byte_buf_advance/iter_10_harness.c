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

    /* 2. Preconditions for the function */
    __CPROVER_assume(buffer.buffer != NULL);
    __CPROVER_assume(len <= buffer.len);
    __CPROVER_assume(output.capacity >= len);   /* output must be able to hold the slice */

    /* 3. Save old state of source buffer */
    struct aws_byte_buf old_buffer = buffer;

    /* 4. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 5. The operation must succeed under the assumed pre‑conditions */
    assert(result);

    /* 6. Post‑conditions for the successful case */
    if (len > 0) {
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.buffer == old_buffer.buffer + len);
    } else {
        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);
    }
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);

    /* output now refers to the advanced slice */
    assert(output.len == len);
    assert(output.buffer == old_buffer.buffer);
    assert(output.capacity == len);
    assert(output.allocator == old_buffer.allocator);

    /* 7. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
