#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_buf_harness(void) {
    /* Allocate a non-deterministic aws_byte_buf */
    struct aws_byte_buf buf;

    /* Non-deterministically initialize the fields */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save original values to check frame conditions */
    uint8_t *original_buffer   = buf.buffer;
    size_t   original_len      = buf.len;
    size_t   original_capacity = buf.capacity;
    struct aws_allocator *original_allocator = buf.allocator;

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_buf(&buf);

    /* Postconditions: result fields match buf fields */
    assert(result.ptr == buf.buffer);
    assert(result.len == buf.len);

    /* Postconditions: returned cursor is valid */
    assert(aws_byte_cursor_is_valid(&result));

    /* Postconditions: result.ptr == original buffer pointer */
    assert(result.ptr == original_buffer);

    /* Postconditions: result.len == original buf.len */
    assert(result.len == original_len);

    /* Frame conditions: buf is not modified */
    assert(buf.buffer    == original_buffer);
    assert(buf.len       == original_len);
    assert(buf.capacity  == original_capacity);
    assert(buf.allocator == original_allocator);
}
