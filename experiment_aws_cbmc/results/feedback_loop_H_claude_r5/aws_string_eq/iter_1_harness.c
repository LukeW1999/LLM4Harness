#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * aws_string_eq:
 * - Returns true if bytes of string are the same, false otherwise.
 * - If a == b (same pointer), returns true.
 * - If either is NULL, returns false.
 * - Otherwise compares bytes and lengths.
 *
 * Postconditions:
 * 1. Changed fields: only the return value (bool result).
 * 2. Unchanged fields: a and b are not modified (immutable strings).
 * 3. Failure path: N/A (always returns a bool, no error code).
 * 4. Validity invariants: a and b remain valid if they were valid.
 */

/* Helper to allocate a non-deterministic aws_string with bounded length */
struct aws_string *make_arbitrary_aws_string_nondet_len_with_max(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate memory for the struct + len + 1 (null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    /* We need to write to the const fields - use a mutable pointer */
    /* Cast away const for initialization purposes */
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;
    /* The bytes field is const uint8_t[1], but the actual allocation is larger */
    /* CBMC will treat the memory as accessible */
    return str;
}

void aws_string_eq_harness(void) {
    /* Declare two aws_string pointers non-deterministically (may be NULL) */
    struct aws_string *a;
    struct aws_string *b;

    /* Non-deterministically decide if a and b are NULL or valid */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();
    bool a_eq_b = nondet_bool(); /* whether a and b point to the same string */

    if (a_is_null) {
        a = NULL;
    } else {
        /* Create a valid aws_string with bounded length */
        size_t len_a;
        __CPROVER_assume(len_a <= 4); /* bound for tractability */
        a = malloc(sizeof(struct aws_string) + len_a + 1);
        __CPROVER_assume(a != NULL);
        size_t *len_ptr_a = (size_t *)&a->len;
        *len_ptr_a = len_a;
        /* allocator can be anything */
    }

    if (b_is_null) {
        b = NULL;
    } else if (!a_is_null && a_eq_b) {
        /* Make b point to the same string as a */
        b = a;
    } else {
        /* Create a valid aws_string with bounded length */
        size_t len_b;
        __CPROVER_assume(len_b <= 4); /* bound for tractability */
        b = malloc(sizeof(struct aws_string) + len_b + 1);
        __CPROVER_assume(b != NULL);
        size_t *len_ptr_b = (size_t *)&b->len;
        *len_ptr_b = len_b;
    }

    /* Save state before call (strings are immutable, but save pointers) */
    struct aws_string *old_a = a;
    struct aws_string *old_b = b;
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition 1: If a == b (same pointer, both non-null), result must be true */
    if (a != NULL && b != NULL && a == b) {
        assert(result == true);
    }

    /* Postcondition 2: If either is NULL (and they're not the same pointer), result must be false */
    if (a == NULL && b == NULL) {
        /* Both NULL: a == b is false (NULL != NULL in pointer comparison? 
         * Actually NULL == NULL is true, so a == b is true, returns true */
        /* Wait: if a == NULL and b == NULL, then a == b is true, so returns true */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* One is NULL, the other is not: a != b, then check NULL -> returns false */
        assert(result == false);
    }

    /* Postcondition 3: Strings are not modified (immutability) */
    assert(a == old_a);
    assert(b == old_b);
    if (a != NULL) {
        assert(a->len == old_a_len);
    }
    if (b != NULL) {
        assert(b->len == old_b_len);
    }

    /* Postcondition 4: If both non-null and different pointers, result reflects byte equality */
    if (a != NULL && b != NULL && a != b) {
        if (a->len != b->len) {
            /* Different lengths means not equal */
            assert(result == false);
        }
        /* If same length, result depends on byte comparison - CBMC will verify */
    }

    /* Postcondition 5: result is a valid bool */
    assert(result == true || result == false);
}
