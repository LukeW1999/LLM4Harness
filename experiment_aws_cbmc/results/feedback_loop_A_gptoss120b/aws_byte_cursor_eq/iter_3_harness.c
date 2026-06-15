#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024U

int main(void) {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    /* nondet length within bounds */
    a.len = (size_t) nondet_uint();
    __CPROVER_assume(a.len <= MAX_BUFFER_SIZE);
    b.len = (size_t) nondet_uint();
    __CPROVER_assume(b.len <= MAX_BUFFER_SIZE);

    /* allocate buffers if length > 0, otherwise set NULL */
    if (a.len > 0) {
        a.ptr = malloc(a.len);
        __CPROVER_assume(a.ptr != NULL);
    } else {
        a.ptr = NULL;
    }

    if (b.len > 0) {
        b.ptr = malloc(b.len);
        __CPROVER_assume(b.ptr != NULL);
    } else {
        b.ptr = NULL;
    }

    /* keep old copies for post‑condition checks */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    bool result = aws_byte_cursor_eq(&a, &b);

    /* compute expected result */
    bool expected = false;
    if (old_a.len == old_b.len) {
        if (old_a.ptr == old_b.ptr) {
            expected = true;
        } else if (old_a.ptr != NULL && old_b.ptr != NULL) {
            expected = (memcmp(old_a.ptr, old_b.ptr, old_a.len) == 0);
        }
    }

    /* post‑conditions */
    assert(result == expected);
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    assert(b.ptr == old_b.ptr);
    assert(b.len == old_b.len);
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    return 0;
}
