#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256U

void aws_byte_cursor_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    /* Allocate a valid buffer for the cursor */
    a.ptr = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(a.ptr != NULL);
    a.len = nondet_uint();
    __CPROVER_assume(a.len <= MAX_BUFFER_SIZE);

    /* Ensure the byte buffer is valid and its internal buffer is non‑NULL */
    ensure_byte_buf_is_valid(&b, allocator);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));
    __CPROVER_assume(b.len <= b.capacity);
    __CPROVER_assume(b.buffer != NULL);          /* memcmp requires a readable pointer */
    __CPROVER_assume(b.capacity >= b.len);

    /* Call under verification */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* Postcondition: inputs remain valid */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    (void)result; /* suppress unused variable warning */
}
