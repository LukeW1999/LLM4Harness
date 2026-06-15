#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_CAP 64

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    /* allocate a concrete backing array for the source buffer */
    size_t cap;
    __CPROVER_assume(cap > 0 && cap <= MAX_CAP);
    uint8_t *data = malloc(cap);
    __CPROVER_assume(data != NULL);

    buffer.buffer = data;
    buffer.capacity = cap;
    buffer.len = 0;                     /* will be set nondeterministically below */
    buffer.allocator = aws_default_allocator();

    /* nondet length of the source buffer, must be within capacity */
    __CPROVER_assume(buffer.len <= buffer.capacity);
    __CPROVER_assume(buffer.len <= cap);

    /* nondet advance length, cannot exceed current length */
    __CPROVER_assume(len <= buffer.len);

    /* set up output buffer as an empty, uninitialized buffer */
    output.buffer = NULL;
    output.capacity = 0;
    output.len = 0;
    output.allocator = NULL;

    /* keep copies of the original state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* postconditions about validity */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));

    if (result) {
        /* successful advance – source advanced, output reflects the slice */
        assert(buffer.buffer == old_buffer.buffer + len);
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == old_buffer.buffer);
        assert(output.len == len);
        /* output.capacity and allocator are implementation‑defined; we can still check allocator */
        assert(output.allocator == NULL);
    } else {
        /* failed advance – buffers unchanged */
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == old_output.buffer);
        assert(output.capacity == old_output.capacity);
        assert(output.len == old_output.len);
        assert(output.allocator == old_output.allocator);
    }

    free(data);
}
