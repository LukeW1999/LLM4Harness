#include <aws/common/byte_buf.h>
#include <assert.h>

void aws_byte_cursor_from_buf_harness() {
    struct aws_byte_buf buf;
    buf.buffer = (uint8_t *)malloc(nondet_size_t() * sizeof(uint8_t));
    __CPROVER_assume(buf.buffer != NULL);
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    struct aws_byte_cursor cur;

    cur = aws_byte_cursor_from_buf(&buf);

    // Assert frame conditions
    assert(buf.buffer == cur.ptr);
    assert(buf.len == cur.len);

    // Assert validity invariants
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&cur));

    free(buf.buffer);
}
