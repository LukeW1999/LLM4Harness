#include <aws/common/byte_buf.h>
#include <stddef.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 10
#endif

void aws_byte_cursor_from_buf_harness() {
    struct aws_byte_buf buf;

    /* Nondeterministic initialization of buffer fields */
    buf.allocator = nondet_bool() ? NULL : malloc(sizeof(struct aws_allocator));
    buf.buffer = malloc(MAX_BUFFER_SIZE);
    buf.capacity = nondet_size_t();
    buf.len = nondet_size_t();

    /* Assume buffer is valid: buffer not NULL, len <= capacity, capacity <= MAX_BUFFER_SIZE */
    __CPROVER_assume(buf.buffer != NULL);
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* Postconditions */
    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);

    /* Buffer unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* Cursor validity */
    assert(aws_byte_cursor_is_valid(&cur));

    /* Buffer validity preserved */
    assert(aws_byte_buf_is_valid(&buf));
}
