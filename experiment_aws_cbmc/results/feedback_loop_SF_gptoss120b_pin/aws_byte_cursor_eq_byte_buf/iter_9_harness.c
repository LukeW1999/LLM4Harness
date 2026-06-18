#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256U

void aws_byte_cursor_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    /* Allocate and initialize a valid byte cursor */
    a.ptr = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(a.ptr != NULL);
    a.len = nondet_uint();
    __CPROVER_assume(a.len <= MAX_BUFFER_SIZE);

    /* Allocate and initialize a valid byte buffer */
    b.buffer = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(b.buffer != NULL);
    b.capacity = MAX_BUFFER_SIZE;
    b.len = nondet_uint();
    __CPROVER_assume(b.len <= b.capacity);
    b.allocator = allocator;

    /* Ensure both structures are valid before the call */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Call the function under verification */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* Postconditions: inputs remain valid */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    (void)result; /* suppress unused variable warning */
}
