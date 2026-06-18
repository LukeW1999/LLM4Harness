#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_buf_advance_harness() {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    /* Initialize buffer as a valid, bounded byte_buf */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* The function accesses buffer->buffer + buffer->len, so buffer must not be NULL */
    __CPROVER_assume(buffer.buffer != NULL);

    /* Bound len to keep state space manageable */
    len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Save old state of buffer */
    struct aws_byte_buf old_buffer = buffer;

    /* Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* Postconditions */
    if (result) {
        /* Success path: buffer->len increased by len */
        assert(buffer.len == old_buffer.len + len);
        /* buffer->buffer, capacity, allocator unchanged */
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);

        /* output fields as per Doxygen */
        assert(output.allocator == NULL);
        assert(output.len == 0);
        assert(output.capacity == len);
        /* output.buffer must point to the old current position */
        assert(output.buffer == old_buffer.buffer + old_buffer.len);

        /* output must be a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(&output));
    } else {
        /* Failure path: buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);

        /* output zeroed */
        assert(output.len == 0);
        assert(output.buffer == NULL);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    /* buffer must remain valid after call */
    assert(aws_byte_buf_is_valid(&buffer));
}
