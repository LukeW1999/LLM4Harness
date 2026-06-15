#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* Ensure buffer has enough capacity for its length */
    __CPROVER_assume(buffer.capacity >= buffer.len);
    __CPROVER_assume(buffer.capacity >= len);

    /* len must be no larger than the current length of the buffer */
    __CPROVER_assume(len <= buffer.len);
    /* if len is non‑zero, the buffer must have a non‑NULL underlying array */
    __CPROVER_assume(!(len > 0 && buffer.buffer == NULL));

    /* Use a valid allocator for the source buffer */
    buffer.allocator = aws_default_allocator();

    /* output must be a valid buffer with enough capacity for the slice */
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;
    ensure_byte_buf_has_allocated_buffer_member(&output);
    __CPROVER_assume(output.capacity >= len);
    __CPROVER_assume(!(len > 0 && output.buffer == NULL));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* The function should succeed under the above assumptions */
    __CPROVER_assert(result, "aws_byte_buf_advance must succeed");

    /* success: buffer advanced forward, output describes the taken slice */
    assert(buffer.buffer == old_buffer.buffer + len);
    assert(buffer.len == old_buffer.len - len);
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);

    assert(output.buffer == old_buffer.buffer);
    assert(output.capacity == len);
    assert(output.len == len);
    assert(output.allocator == NULL);

    /* invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
