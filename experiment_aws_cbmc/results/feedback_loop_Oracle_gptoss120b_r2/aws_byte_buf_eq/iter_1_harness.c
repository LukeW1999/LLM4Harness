#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

/* CBMC nondeterministic helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_buf_eq_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* --- Create two byte buffers with nondeterministic shape --- */
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    size_t cap_a = nondet_size_t();
    __CPROVER_assume(cap_a <= 256);
    size_t cap_b = nondet_size_t();
    __CPROVER_assume(cap_b <= 256);

    /* Initialise buffers (allocation may be zero‑length) */
    __CPROVER_assume(aws_byte_buf_init(&a, alloc, cap_a) == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_byte_buf_init(&b, alloc, cap_b) == AWS_OP_SUCCESS);

    /* Choose lengths that respect capacities */
    size_t len_a = nondet_size_t();
    __CPROVER_assume(len_a <= a.capacity);
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_b <= b.capacity);
    a.len = len_a;
    b.len = len_b;

    /* Fill the used portion of each buffer with nondeterministic bytes */
    for (size_t i = 0; i < a.len; ++i) {
        a.buffer[i] = nondet_uint8_t();
    }
    for (size_t i = 0; i < b.len; ++i) {
        b.buffer[i] = nondet_uint8_t();
    }

    /* Ensure the buffers satisfy the library’s validity predicate */
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* --- Snapshot the pre‑state (frame condition) --- */
    struct aws_byte_buf a_old = a;
    struct aws_byte_buf b_old = b;

    uint8_t *a_snapshot = NULL;
    uint8_t *b_snapshot = NULL;

    if (a.capacity > 0) {
        a_snapshot = malloc(a.capacity);
        __CPROVER_assume(a_snapshot != NULL);
        memcpy(a_snapshot, a.buffer, a.capacity);
    }
    if (b.capacity > 0) {
        b_snapshot = malloc(b.capacity);
        __CPROVER_assume(b_snapshot != NULL);
        memcpy(b_snapshot, b.buffer, b.capacity);
    }

    /* --- Call the function under test --- */
    bool result = aws_byte_buf_eq(&a, &b);

    /* --- Post‑conditions --------------------------------------------------- */

    /* 1. Return‑value correctness: result must equal the underlying array equality */
    bool expected = aws_array_eq(a.buffer, a.len, b.buffer, b.len);
    assert(result == expected);

    /* 2. Length / capacity invariants: the buffers must be unchanged */
    assert(a.len == a_old.len);
    assert(a.capacity == a_old.capacity);
    assert(b.len == b_old.len);
    assert(b.capacity == b_old.capacity);
    assert(a.allocator == a_old.allocator);
    assert(b.allocator == b_old.allocator);

    /* 3. Frame condition: memory backing the buffers must be unchanged */
    if (a.capacity > 0) {
        assert(memcmp(a.buffer, a_snapshot, a.capacity) == 0);
    }
    if (b.capacity > 0) {
        assert(memcmp(b.buffer, b_snapshot, b.capacity) == 0);
    }

    /* Clean up auxiliary allocations */
    if (a_snapshot) free(a_snapshot);
    if (b_snapshot) free(b_snapshot);
    aws_byte_buf_clean_up(&a);
    aws_byte_buf_clean_up(&b);

    return 0;
}
