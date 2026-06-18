#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    /* source buffer */
    size_t src_capacity = nondet_size_t();
    __CPROVER_assume(src_capacity > 0 && src_capacity <= MAX_BUFFER_SIZE);
    buffer.buffer = malloc(src_capacity);
    __CPROVER_assume(buffer.buffer != NULL);
    buffer.capacity = src_capacity;
    buffer.len = nondet_size_t();
    __CPROVER_assume(buffer.len <= src_capacity);
    buffer.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* destination buffer (will be overwritten by advance) */
    size_t out_capacity = nondet_size_t();
    __CPROVER_assume(out_capacity <= MAX_BUFFER_SIZE);
    output.buffer = malloc(out_capacity);
    __CPROVER_assume(out_capacity == 0 || output.buffer != NULL);
    output.capacity = out_capacity;
    output.len = 0;
    output.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* length to advance */
    len = nondet_size_t();
    __CPROVER_assume(len <= buffer.len);
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        /* source buffer invariants after advance */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.buffer == (uint8_t *)old_buffer.buffer + len);

        /* destination buffer reflects the slice taken from source */
        assert(output.buffer == old_buffer.buffer);
        assert(output.len == len);
        assert(output.capacity == len);
        assert(output.allocator == old_buffer.allocator);
    }

    /* both buffers must remain valid regardless of success */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
