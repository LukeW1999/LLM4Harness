/*=== Contract for aws_byte_buf_eq ===
Preconditions:
  - a != NULL && b != NULL
  - aws_byte_buf_is_valid(a)
  - aws_byte_buf_is_valid(b)
Postconditions (validity):
  - aws_byte_buf_is_valid(a) && aws_byte_buf_is_valid(b)  // buffers remain valid
  - return value == (a->len == b->len && (a->len == 0 || memcmp(a->buffer, b->buffer, a->len) == 0))
Postconditions (frame):
  - No memory locations outside a->buffer[0..a->capacity-1] and b->buffer[0..b->capacity-1] are modified.
  - a->len, a->capacity, a->allocator, b->len, b->capacity, b->allocator remain unchanged.
===*/

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_eq_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* Create nondeterministic capacities and lengths */
    size_t cap_a = nondet_size_t();
    size_t cap_b = nondet_size_t();
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    /* Reasonable bounds to keep the model tractable */
    __CPROVER_assume(cap_a <= 256);
    __CPROVER_assume(cap_b <= 256);
    __CPROVER_assume(len_a <= cap_a);
    __CPROVER_assume(len_b <= cap_b);

    /* Allocate buffers */
    uint8_t *buf_a = NULL;
    uint8_t *buf_b = NULL;
    if (cap_a > 0) {
        buf_a = (uint8_t *)aws_mem_acquire(alloc, cap_a);
        __CPROVER_assume(buf_a != NULL);
    }
    if (cap_b > 0) {
        buf_b = (uint8_t *)aws_mem_acquire(alloc, cap_b);
        __CPROVER_assume(buf_b != NULL);
    }

    /* Fill buffers with nondeterministic data */
    for (size_t i = 0; i < cap_a; ++i) {
        buf_a[i] = nondet_uint8_t();
    }
    for (size_t i = 0; i < cap_b; ++i) {
        buf_b[i] = nondet_uint8_t();
    }

    struct aws_byte_buf a = {
        .buffer = buf_a,
        .len = len_a,
        .capacity = cap_a,
        .allocator = alloc
    };
    struct aws_byte_buf b = {
        .buffer = buf_b,
        .len = len_b,
        .capacity = cap_b,
        .allocator = alloc
    };

    /* Assume the buffers are valid according to the library's invariant */
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Snapshot the original memory contents */
    uint8_t snapshot_a[256];
    uint8_t snapshot_b[256];
    for (size_t i = 0; i < cap_a; ++i) {
        snapshot_a[i] = buf_a[i];
    }
    for (size_t i = 0; i < cap_b; ++i) {
        snapshot_b[i] = buf_b[i];
    }

    /* Call the function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* Verify postconditions */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Buffers must be unchanged */
    for (size_t i = 0; i < cap_a; ++i) {
        assert(buf_a[i] == snapshot_a[i]);
    }
    for (size_t i = 0; i < cap_b; ++i) {
        assert(buf_b[i] == snapshot_b[i]);
    }

    /* Compute expected result */
    bool expected;
    if (a.len != b.len) {
        expected = false;
    } else if (a.len == 0) {
        expected = true;
    } else {
        expected = (memcmp(a.buffer, b.buffer, a.len) == 0);
    }
    assert(result == expected);

    /* Clean up */
    if (buf_a) {
        aws_mem_release(alloc, buf_a);
    }
    if (buf_b) {
        aws_mem_release(alloc, buf_b);
    }

    return 0;
}
