#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare and bound source buffer */
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 2. Choose length to advance */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= buffer.len);
    /* src buffer must be non‑NULL when advancing a non‑zero length */
    __CPROVER_assume(len == 0 || buffer.buffer != NULL);

    /* 3. Save old state of source buffer */
    struct aws_byte_buf old_buffer = buffer;

    /* 4. Prepare destination buffer (must be empty) */
    struct aws_byte_buf output;
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;

    /* 5. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 6. The operation must succeed under the assumed pre‑conditions */
    assert(result);

    /* 7. Post‑conditions for the successful case */
    assert(buffer.len == old_buffer.len - len);
    assert(buffer.capacity == old_buffer.capacity);               /* capacity unchanged */
    assert(buffer.buffer == old_buffer.buffer + len);
    assert(buffer.allocator == old_buffer.allocator);

    assert(output.len == len);
    assert(output.capacity == len);
    assert(output.buffer == old_buffer.buffer);
    assert(output.allocator == old_buffer.allocator);

    /* 8. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
