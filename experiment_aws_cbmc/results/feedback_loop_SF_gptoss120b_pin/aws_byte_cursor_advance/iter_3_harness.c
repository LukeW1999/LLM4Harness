#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness(void) {
    /* allocator for any allocations we need */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic capacity for the backing buffer */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 1024);               /* bound for tractability */

    /* allocate backing memory */
    uint8_t *buffer = (uint8_t *)malloc(capacity);
    __CPROVER_assume(buffer != NULL || capacity == 0);

    /* construct a valid aws_byte_buf */
    struct aws_byte_buf buf;
    buf.buffer    = buffer;
    buf.len       = capacity;
    buf.capacity  = capacity;
    buf.allocator = allocator;
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* create a cursor from the buffer */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&buf);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* nondeterministic length to advance, constrained to be within the cursor */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= cursor.len);

    /* snapshot pre‑call state */
    size_t   old_len = cursor.len;
    uint8_t *old_ptr = cursor.ptr;

    /* call the function under verification */
    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    /* post‑condition assertions */
    assert(result.ptr == old_ptr);
    assert(result.len == len);
    assert(cursor.ptr == old_ptr + len);
    assert(cursor.len == old_len - len);
    assert(buf.buffer == buffer);
    assert(buf.len == capacity);
    assert(buf.capacity == capacity);
    assert(buf.allocator == allocator);
    assert(aws_byte_cursor_is_valid(&result));
}
