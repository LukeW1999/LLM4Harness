#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Harness for aws_string_eq
 *
 * From Doxygen: Returns true if bytes of string are the same, false otherwise.
 *
 * From implementation:
 * - If a == b (same pointer), returns true
 * - If either is NULL, returns false
 * - Otherwise, compares bytes using aws_array_eq
 *
 * 1. Changed fields: none (pure predicate, no mutation)
 * 2. Unchanged fields: all fields of a and b remain unchanged
 * 3. Failure: returns false when strings differ
 * 4. Validity invariants: a and b remain valid after the call
 */

/* Helper to allocate a non-deterministic aws_string with bounded length */
struct aws_string *make_arbitrary_aws_string_nondet_len_with_max(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate memory for the struct + len bytes + 1 null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    /* Initialize fields - cast away const for initialization */
    *(size_t *)&str->len = len;
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    /* bytes are non-deterministic (already non-det from malloc) */
    /* ensure null terminator */
    ((uint8_t *)str->bytes)[len] = '\0';
    return str;
}

void aws_string_eq_harness(void) {
    /* Non-deterministically choose whether to use NULL or valid pointers */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();
    bool a_eq_b = nondet_bool();

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 8
#endif

    struct aws_string *a;
    struct aws_string *b;

    if (a_is_null) {
        a = NULL;
    } else {
        a = make_arbitrary_aws_string_nondet_len_with_max(MAX_STRING_LEN);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (b_is_null) {
        b = NULL;
    } else if (a_eq_b && !a_is_null) {
        /* Make b point to the same string as a */
        b = a;
    } else {
        b = make_arbitrary_aws_string_nondet_len_with_max(MAX_STRING_LEN);
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save state before call */
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;
    struct aws_allocator *old_a_allocator = (a != NULL) ? a->allocator : NULL;
    struct aws_allocator *old_b_allocator = (b != NULL) ? b->allocator : NULL;

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postconditions */

    /* If a == b (same pointer), result must be true */
    if (a != NULL && b != NULL && a == b) {
        assert(result == true);
    }

    /* If either is NULL (but not both equal), result must be false */
    if (a == NULL || b == NULL) {
        /* If a == b == NULL, they are the same pointer, so result is true */
        if (a == b) {
            /* Both NULL: a == b is true, so result should be true */
            assert(result == true);
        } else {
            assert(result == false);
        }
    }

    /* Unchanged fields: a and b must not be modified */
    if (a != NULL) {
        assert(a->len == old_a_len);
        assert(a->allocator == old_a_allocator);
        assert(aws_string_is_valid(a));
    }

    if (b != NULL) {
        assert(b->len == old_b_len);
        assert(b->allocator == old_b_allocator);
        assert(aws_string_is_valid(b));
    }

    /* Symmetry: aws_string_eq(a, b) == aws_string_eq(b, a) */
    bool result2 = aws_string_eq(b, a);
    assert(result == result2);
}
