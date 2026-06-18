#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

size_t nondet_size_t(void); /* CBMC nondet size_t */

void aws_byte_buf_eq_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* --- buffer a --- */
    struct aws_byte_buf a;
    a.allocator = allocator;

    a.capacity = nondet_size_t();
    __CPROVER_assume(a.capacity <= MAX_BUFFER_SIZE);

    if (a.capacity > 0) {
        a.buffer = malloc(a.capacity);
        __CPROVER_assume(a.buffer != NULL);
    } else {
        a.buffer = NULL;
    }

    a.len = nondet_size_t();
    __CPROVER_assume(a.len <= a.capacity);

    /* --- buffer b --- */
    struct aws_byte_buf b;
    b.allocator = allocator;

    b.capacity = nondet_size_t();
    __CPROVER_assume(b.capacity <= MAX_BUFFER_SIZE);

    if (b.capacity > 0) {
        b.buffer = malloc(b.capacity);
        __CPROVER_assume(b.buffer != NULL);
    } else {
        b.buffer = NULL;
    }

    b.len = nondet_size_t();
    __CPROVER_assume(b.len <= b.capacity);

    /* Save old copies */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* Call function under test */
    bool result = aws_byte_buf_eq(&a, &b);
    (void)result; /* suppress unused variable warning */

    /* Verify post‑conditions */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.buffer == old_a.buffer);
    assert(a.allocator == old_a.allocator);

    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.buffer == old_b.buffer);
    assert(b.allocator == old_b.allocator);
}
