/*@
Preconditions:
  - The pointers `a` and `b` passed to `aws_byte_cursor_eq` are non‑NULL.
  - `a->len` and `b->len` are arbitrary `size_t` values bounded by a reasonable maximum (e.g., 1024).
  - If `a->len > 0` then `a->ptr` points to a valid memory region of at least `a->len` bytes.
    If `a->len == 0` then `a->ptr` may be NULL.
  - The same conditions hold for `b`.
  - No other global state is required for this call.

Postconditions (validity):
  - The function returns `true` iff the two cursor byte‑sequences are equal
    (same length and each corresponding byte identical).
  - The call does **not** modify the contents of `*a`, `*b`, nor the memory
    referenced by `a->ptr` or `b->ptr`.

Postconditions (frame):
  - No global variables are modified.
  - The memory regions pointed to by `a->ptr` and `b->ptr` remain unchanged.
@*/

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Manual reference implementation of cursor equality */
static bool manual_cursor_eq(const struct aws_byte_cursor *a, const struct aws_byte_cursor *b) {
    if (a->len != b->len) {
        return false;
    }
    for (size_t i = 0; i < a->len; ++i) {
        if (a->ptr[i] != b->ptr[i]) {
            return false;
        }
    }
    return true;
}

void aws_byte_cursor_eq_harness(void) {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    /* nondet lengths bounded to avoid excessive allocation */
    size_t len_a;
    size_t len_b;
    __CPROVER_assume(len_a <= 1024);
    __CPROVER_assume(len_b <= 1024);
    a.len = len_a;
    b.len = len_b;

    /* Allocate memory for a.ptr if needed */
    if (len_a > 0) {
        a.ptr = malloc(len_a);
        __CPROVER_assume(a.ptr != NULL);
    } else {
        a.ptr = NULL;
    }

    /* Allocate memory for b.ptr if needed */
    if (len_b > 0) {
        b.ptr = malloc(len_b);
        __CPROVER_assume(b.ptr != NULL);
    } else {
        b.ptr = NULL;
    }

    /* Preserve original state for frame checking */
    struct aws_byte_cursor a_old = a;
    struct aws_byte_cursor b_old = b;

    /* Compute expected result using the reference implementation */
    bool expected = manual_cursor_eq(&a, &b);

    /* Call the function under verification */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Verify return value matches specification */
    __CPROVER_assert(result == expected,
                     "aws_byte_cursor_eq must return true iff the byte sequences are equal");

    /* Verify that the cursor structures themselves are unchanged */
    __CPROVER_assert(a.len == a_old.len, "a.len unchanged");
    __CPROVER_assert(b.len == b_old.len, "b.len unchanged");
    __CPROVER_assert(a.ptr == a_old.ptr, "a.ptr unchanged");
    __CPROVER_assert(b.ptr == b_old.ptr, "b.ptr unchanged");

    /* Verify that the underlying memory contents are unchanged */
    if (a.len > 0 && a.ptr == a_old.ptr) {
        for (size_t i = 0; i < a.len; ++i) {
            __CPROVER_assert(a.ptr[i] == a_old.ptr[i],
                             "memory pointed to by a.ptr unchanged");
        }
    }
    if (b.len > 0 && b.ptr == b_old.ptr) {
        for (size_t i = 0; i < b.len; ++i) {
            __CPROVER_assert(b.ptr[i] == b_old.ptr[i],
                             "memory pointed to by b.ptr unchanged");
        }
    }

    return 0;
}
