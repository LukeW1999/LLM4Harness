#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_buf_advance_harness(void) {
    /* Set up buffer with nondeterministic but valid state */
    struct aws_byte_buf buffer;
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* Set up output buffer - must be valid before call */
    struct aws_byte_buf output;
    ensure_byte_buf_has_allocated_buffer_member(&output);
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* Nondeterministic len to advance */
    size_t len = nondet_size_t();

    /* Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* Call the function */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        /* SUCCESS PATH */
        /* buffer->len increased by len */
        assert(buffer.len == old_buffer.len + len);
        /* buffer->capacity unchanged */
        assert(buffer.capacity == old_buffer.capacity);
        /* buffer->buffer unchanged */
        assert(buffer.buffer == old_buffer.buffer);
        /* buffer->allocator unchanged */
        assert(buffer.allocator == old_buffer.allocator);

        /* output->len is 0 */
        assert(output.len == 0);
        /* output->capacity is len */
        assert(output.capacity == len);
        /* output->allocator is NULL (set by aws_byte_buf_from_array) */
        assert(output.allocator == NULL);

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    } else {
        /* FAILURE PATH */
        /* buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* output zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    }
}
