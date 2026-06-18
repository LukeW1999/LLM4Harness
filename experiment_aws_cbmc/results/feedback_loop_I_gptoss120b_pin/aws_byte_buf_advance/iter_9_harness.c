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

    /* allocate and initialize source buffer */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= MAX_BUFFER_SIZE);
    buffer.buffer = malloc(capacity);
    __CPROVER_assume(buffer.buffer != NULL);
    buffer.capacity = capacity;
    buffer.len = nondet_size_t();
    __CPROVER_assume(buffer.len <= capacity);
    buffer.allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* destination buffer starts empty but must be a valid struct */
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* length to advance: non‑deterministic but not larger than source */
    len = nondet_size_t();
    __CPROVER_assume(len <= buffer.len);
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    struct aws_byte_buf old_buffer = buffer;

    bool result = aws_byte_buf_advance(&buffer, &output, len);
    __CPROVER_assume(result); /* ensure the call succeeded */

    /* source buffer invariants after advance */
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);
    assert(buffer.len == old_buffer.len - len);
    assert(buffer.buffer == (uint8_t *)old_buffer.buffer + len);

    /* destination buffer reflects the slice taken */
    assert(output.buffer == old_buffer.buffer);
    assert(output.len == len);
    assert(output.capacity == len);
    assert(output.allocator == NULL);

    /* both buffers must remain valid */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
