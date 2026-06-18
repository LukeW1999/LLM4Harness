#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

/* nondeterministic helpers for CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
bool nondet_bool(void);

void aws_byte_buf_eq_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* ---------- a ---------- */
    struct aws_byte_buf a;
    a.capacity = nondet_size_t();
    __CPROVER_assume(a.capacity <= 1024);               /* bound for tractability */
    a.len = nondet_size_t();
    __CPROVER_assume(a.len <= a.capacity);
    a.buffer = (uint8_t *)aws_mem_acquire(alloc, a.capacity);
    __CPROVER_assume(a.buffer != NULL || a.capacity == 0);
    a.allocator = alloc;

    if (a.buffer) {
        for (size_t i = 0; i < a.capacity; ++i) {
            a.buffer[i] = nondet_uint8_t();
        }
    }

    /* ---------- b ---------- */
    struct aws_byte_buf b;
    b.capacity = nondet_size_t();
    __CPROVER_assume(b.capacity <= 1024);
    b.len = nondet_size_t();
    __CPROVER_assume(b.len <= b.capacity);
    b.buffer = (uint8_t *)aws_mem_acquire(alloc, b.capacity);
    __CPROVER_assume(b.buffer != NULL || b.capacity == 0);
    b.allocator = alloc;

    if (b.buffer) {
        for (size_t i = 0; i < b.capacity; ++i) {
            b.buffer[i] = nondet_uint8_t();
        }
    }

    /* Preserve old state for frame condition checks */
    struct aws_byte_buf a_old = a;
    struct aws_byte_buf b_old = b;

    uint8_t *a_old_content = NULL;
    uint8_t *b_old_content = NULL;

    if (a.buffer) {
        a_old_content = malloc(a.capacity);
        __CPROVER_assume(a_old_content != NULL);
        for (size_t i = 0; i < a.capacity; ++i) {
            a_old_content[i] = a.buffer[i];
        }
    }

    if (b.buffer) {
        b_old_content = malloc(b.capacity);
        __CPROVER_assume(b_old_content != NULL);
        for (size_t i = 0; i < b.capacity; ++i) {
            b_old_content[i] = b.buffer[i];
        }
    }

    /* Call the function under verification */
    bool result = aws_byte_buf_eq(&a, &b);

    /* ---------- Post‑conditions ---------- */

    /* 1. Return‑value correctness (semantic equivalence) */
    bool expected = aws_array_eq(a.buffer, a.len, b.buffer, b.len);
    assert(result == expected);

    /* 2. Length / capacity invariants (unchanged) */
    assert(a.len == a_old.len);
    assert(a.capacity == a_old.capacity);
    assert(b.len == b_old.len);
    assert(b.capacity == b_old.capacity);
    assert(a.allocator == a_old.allocator);
    assert(b.allocator == b_old.allocator);

    /* 3. Frame conditions – memory not modified beyond contract */
    if (a.buffer && a_old_content) {
        assert(aws_array_eq(a.buffer, a.capacity, a_old_content, a.capacity));
    }
    if (b.buffer && b_old_content) {
        assert(aws_array_eq(b.buffer, b.capacity, b_old_content, b.capacity));
    }

    /* Buffers must remain valid */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Clean up */
    if (a.buffer) {
        aws_mem_release(alloc, a.buffer);
    }
    if (b.buffer) {
        aws_mem_release(alloc, b.buffer);
    }
    free(a_old_content);
    free(b_old_content);

    return 0;
}
