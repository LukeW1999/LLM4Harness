/*******************************************************************
 * CBMC harness for aws_byte_buf_eq
 *
 * Verifies:
 *   1. Return value matches the specification (equality of contents).
 *   2. Length, capacity and allocator fields are unchanged.
 *   3. The underlying memory buffers are not modified.
 *
 * Uses aws_default_allocator() for any allocations.
 *******************************************************************/

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>

#include <proof_helpers/make_common_data_structures.h>

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Nondeterministic generators */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_buf_eq_harness(void) {
    /* -----------------------------------------------------------------
     *  Create first byte buffer (a)
     * ----------------------------------------------------------------- */
    struct aws_byte_buf a = {0};

    /* Nondeterministic capacity and length */
    size_t cap_a = nondet_size_t();
    __CPROVER_assume(cap_a <= 256U);               /* bound the size for tractability */
    a.capacity = cap_a;

    if (cap_a > 0) {
        a.buffer = (uint8_t *)malloc(cap_a);
        __CPROVER_assume(a.buffer != NULL);        /* assume allocation succeeds */
    } else {
        a.buffer = NULL;
    }

    size_t len_a = nondet_size_t();
    __CPROVER_assume(len_a <= cap_a);
    a.len = len_a;

    /* Fill the allocated region with nondeterministic data */
    for (size_t i = 0U; i < a.capacity; ++i) {
        a.buffer[i] = nondet_uint8_t();
    }

    a.allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    /* Keep a copy of the original contents for the frame condition */
    uint8_t *orig_a = NULL;
    if (a.buffer != NULL) {
        orig_a = (uint8_t *)malloc(a.capacity);
        __CPROVER_assume(orig_a != NULL);
        memcpy(orig_a, a.buffer, a.capacity);
    }

    /* -----------------------------------------------------------------
     *  Create second byte buffer (b)
     * ----------------------------------------------------------------- */
    struct aws_byte_buf b = {0};

    size_t cap_b = nondet_size_t();
    __CPROVER_assume(cap_b <= 256U);
    b.capacity = cap_b;

    if (cap_b > 0) {
        b.buffer = (uint8_t *)malloc(cap_b);
        __CPROVER_assume(b.buffer != NULL);
    } else {
        b.buffer = NULL;
    }

    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_b <= cap_b);
    b.len = len_b;

    for (size_t i = 0U; i < b.capacity; ++i) {
        b.buffer[i] = nondet_uint8_t();
    }

    b.allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    uint8_t *orig_b = NULL;
    if (b.buffer != NULL) {
        orig_b = (uint8_t *)malloc(b.capacity);
        __CPROVER_assume(orig_b != NULL);
        memcpy(orig_b, b.buffer, b.capacity);
    }

    /* -----------------------------------------------------------------
     *  Preserve the pre‑state of structural fields
     * ----------------------------------------------------------------- */
    const size_t saved_len_a = a.len;
    const size_t saved_cap_a = a.capacity;
    const struct aws_allocator *saved_alloc_a = a.allocator;

    const size_t saved_len_b = b.len;
    const size_t saved_cap_b = b.capacity;
    const struct aws_allocator *saved_alloc_b = b.allocator;

    /* -----------------------------------------------------------------
     *  Call the function under verification
     * ----------------------------------------------------------------- */
    bool result = aws_byte_buf_eq(&a, &b);

    /* -----------------------------------------------------------------
     *  Post‑condition 1: return value correctness
     * ----------------------------------------------------------------- */
    bool expected = false;
    if (a.len == b.len) {
        if (a.len == 0U) {
            expected = true;
        } else {
            expected = (memcmp(a.buffer, b.buffer, a.len) == 0);
        }
    }
    assert(result == expected);

    /* -----------------------------------------------------------------
     *  Post‑condition 2: structural invariants unchanged
     * ----------------------------------------------------------------- */
    assert(a.len == saved_len_a);
    assert(a.capacity == saved_cap_a);
    assert(a.allocator == saved_alloc_a);

    assert(b.len == saved_len_b);
    assert(b.capacity == saved_cap_b);
    assert(b.allocator == saved_alloc_b);

    /* -----------------------------------------------------------------
     *  Post‑condition 3: memory frame condition (contents unchanged)
     * ----------------------------------------------------------------- */
    if (a.buffer != NULL) {
        assert(memcmp(a.buffer, orig_a, a.capacity) == 0);
    }
    if (b.buffer != NULL) {
        assert(memcmp(b.buffer, orig_b, b.capacity) == 0);
    }

    /* Clean up */
    if (a.buffer) free(a.buffer);
    if (b.buffer) free(b.buffer);
    if (orig_a)   free(orig_a);
    if (orig_b)   free(orig_b);

    return 0;
}
