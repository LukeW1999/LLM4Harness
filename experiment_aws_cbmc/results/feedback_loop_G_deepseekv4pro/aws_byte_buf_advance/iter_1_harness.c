#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

void aws_byte_buf_advance_harness() {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    /* Bound the buffer state space */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* Bound len to a reasonable range to limit CBMC exploration */
    __CPROVER_assume(len < MAX_BUFFER_SIZE);

    struct aws_byte_buf old_buffer = buffer;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        /* Success path assertions */
        assert(buffer.len == old_buffer.len + len);
        assert(output.capacity == len);
        assert(output.len == 0);
        assert(output.allocator == NULL);
        /* output.buffer points to the correct offset inside or NULL if original was NULL */
        assert(output.buffer == (old_buffer.buffer ? old_buffer.buffer + old_buffer.len : NULL));

        /* buffer fields that must not change */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);
    } else {
        /* Failure path: buffer must be unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        /* output must be zeroed */
        assert(output.buffer == NULL);
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    /* Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
