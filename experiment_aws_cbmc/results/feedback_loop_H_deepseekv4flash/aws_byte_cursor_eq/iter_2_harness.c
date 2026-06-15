#include <aws/common/byte_buf.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_eq_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    /* Bound lengths */
    size_t a_len;
    size_t b_len;
    __CPROVER_assume(a_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(b_len <= MAX_BUFFER_SIZE);
    a.len = a_len;
    b.len = b_len;

    /* Allocate buffer memory; allow zero-length with NULL pointer */
    if (a.len == 0) {
        a.ptr = NULL;
    } else {
        a.ptr = malloc(a.len);
        __CPROVER_assume(a.ptr != NULL);
    }
    if (b.len == 0) {
        b.ptr = NULL;
    } else {
        b.ptr = malloc(b.len);
        __CPROVER_assume(b.ptr != NULL);
    }

    /* Assume cursors are valid */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* Save old state */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* Call function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Assert postconditions */
    assert(result == true || result == false);
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
