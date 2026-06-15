#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_eq_byte_buf_harness(void) {
    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    /* Allocate backing storage */
    uint8_t *buf_a = malloc(MAX_BUFFER_SIZE);
    uint8_t *buf_b = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(buf_a != NULL);
    __CPROVER_assume(buf_b != NULL);

    /* Nondeterministic lengths within bounds */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Initialise structures */
    a.ptr = buf_a;
    a.len = len_a;

    b.buffer = buf_b;
    b.len = len_b;
    b.capacity = MAX_BUFFER_SIZE;
    b.allocator = NULL;

    /* Assume initial validity */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old state */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    /* Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* Post‑condition checks */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);

    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    bool spec = false;
    if (a.len == b.len) {
        if (a.len == 0) {
            spec = true;
        } else if (a.ptr != NULL && b.buffer != NULL) {
            spec = (memcmp(a.ptr, b.buffer, a.len) == 0);
        }
    }
    assert(result == spec);

    /* Re‑validate invariants */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
