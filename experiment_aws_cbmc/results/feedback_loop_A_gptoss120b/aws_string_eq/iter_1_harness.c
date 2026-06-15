#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

/* bound for the flexible array member of aws_string */
#define MAX_STRING_LEN 8

void aws_string_eq_harness(void) {
    /* --------------------------------------------------------------------
     * 1. Declare two aws_string pointers, possibly NULL, and bound their
     *    lengths.  If they are non‑NULL we allocate enough space for the
     *    flexible array member.
     * -------------------------------------------------------------------- */
    struct aws_string *a;
    struct aws_string *b;

    /* nondeterministically decide whether the pointers are NULL */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    if (!a_is_null) {
        a = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
        __CPROVER_assume(a != NULL);
        /* allocator can be any pointer (including NULL) */
        a->allocator = (struct aws_allocator *)nondet_uint8_t();   /* nondet pointer */
        a->len = nondet_size_t();
        __CPROVER_assume(a->len <= MAX_STRING_LEN);
        /* initialise the bytes with nondet values */
        for (size_t i = 0; i < a->len; ++i) {
            ((uint8_t *)a->bytes)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    } else {
        a = NULL;
    }

    if (!b_is_null) {
        b = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
        __CPROVER_assume(b != NULL);
        b->allocator = (struct aws_allocator *)nondet_uint8_t();
        b->len = nondet_size_t();
        __CPROVER_assume(b->len <= MAX_STRING_LEN);
        for (size_t i = 0; i < b->len; ++i) {
            ((uint8_t *)b->bytes)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    } else {
        b = NULL;
    }

    /* --------------------------------------------------------------------
     * 2. Possibly make the two pointers equal (a == b) to hit that branch.
     * -------------------------------------------------------------------- */
    if (nondet_bool()) {
        /* force equality – both must be non‑NULL for this to be legal */
        if (a != NULL && b != NULL) {
            b = a;
        }
    }

    /* --------------------------------------------------------------------
     * 3. Save old immutable state for later comparison.
     * -------------------------------------------------------------------- */
    struct aws_allocator *old_a_allocator = a ? a->allocator : NULL;
    size_t               old_a_len       = a ? a->len : 0;
    struct store_byte_from_buffer a_store;
    if (a) {
        save_byte_from_array(a->bytes, a->len, &a_store);
    }

    struct aws_allocator *old_b_allocator = b ? b->allocator : NULL;
    size_t               old_b_len       = b ? b->len : 0;
    struct store_byte_from_buffer b_store;
    if (b) {
        save_byte_from_array(b->bytes, b->len, &b_store);
    }

    /* --------------------------------------------------------------------
     * 4. Call the function under test.
     * -------------------------------------------------------------------- */
    bool result = aws_string_eq(a, b);

    /* --------------------------------------------------------------------
     * 5. Post‑condition: return value follows the specification.
     * -------------------------------------------------------------------- */
    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
        assert(result == expected);
    }

    /* --------------------------------------------------------------------
     * 6. Unchanged fields (immutability guarantees).
     * -------------------------------------------------------------------- */
    if (a) {
        assert(a->allocator == old_a_allocator);
        assert(a->len == old_a_len);
        assert_byte_from_buffer_matches(a->bytes, &a_store);
    }
    if (b) {
        assert(b->allocator == old_b_allocator);
        assert(b->len == old_b_len);
        assert_byte_from_buffer_matches(b->bytes, &b_store);
    }

    /* --------------------------------------------------------------------
     * 7. Validity invariants must still hold.
     * -------------------------------------------------------------------- */
    if (a) {
        assert(aws_string_is_valid(a));
    }
    if (b) {
        assert(aws_string_is_valid(b));
    }
}
