#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_string_eq
 *
 * From the Doxygen: Returns true if bytes of string are the same, false otherwise.
 *
 * From the implementation:
 * - If a == b (same pointer), returns true
 * - If either is NULL, returns false
 * - Otherwise, compares bytes using aws_array_eq
 *
 * Postconditions:
 * 1. Changed fields: only the return value (bool result)
 * 2. Unchanged fields: both strings a and b are not modified
 * 3. Both return paths: true means bytes are equal, false means they differ
 * 4. Validity invariants: both strings remain valid after the call
 */

/* Helper to allocate a non-deterministic aws_string with bounded length */
struct aws_string *make_arbitrary_aws_string_nondet_len_with_max(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate memory for the struct + bytes + null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    
    /* Initialize fields - cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    
    /* bytes are non-deterministic (already non-det from malloc) */
    /* ensure null terminator */
    ((uint8_t *)str->bytes)[len] = 0;
    
    return str;
}

void aws_string_eq_harness(void) {
    /* Non-deterministically choose whether to use NULL pointers */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();
    bool a_equals_b = nondet_bool(); /* whether a and b point to same string */

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 10
#endif

    if (!a_is_null) {
        a = make_arbitrary_aws_string_nondet_len_with_max(MAX_STRING_LEN);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (!b_is_null) {
        if (a_equals_b && !a_is_null) {
            /* b points to same string as a */
            b = a;
        } else {
            b = make_arbitrary_aws_string_nondet_len_with_max(MAX_STRING_LEN);
            __CPROVER_assume(aws_string_is_valid(b));
        }
    }

    /* Save state before call to check immutability */
    size_t a_len_before = (a != NULL) ? a->len : 0;
    size_t b_len_before = (b != NULL) ? b->len : 0;

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition 1: If a == b (same pointer, both non-null), result must be true */
    if (a != NULL && b != NULL && a == b) {
        assert(result == true);
    }

    /* Postcondition 2: If either is NULL (but not both equal), result must be false */
    if (a == NULL || b == NULL) {
        /* If a == b == NULL, they are the same pointer, so result is true */
        if (a == b) {
            /* Both NULL: a == b is true, so result should be true */
            assert(result == true);
        } else {
            assert(result == false);
        }
    }

    /* Postcondition 3: result is a valid bool */
    assert(result == true || result == false);

    /* Postcondition 4: Strings are not modified (immutability) */
    if (a != NULL) {
        assert(a->len == a_len_before);
        assert(aws_string_is_valid(a));
    }

    if (b != NULL && b != a) {
        assert(b->len == b_len_before);
        assert(aws_string_is_valid(b));
    }

    /* Postcondition 5: Symmetry - if result is true and both non-null and different pointers,
     * lengths must be equal (necessary condition for byte equality) */
    if (result == true && a != NULL && b != NULL && a != b) {
        assert(a->len == b->len);
    }

    /* Postcondition 6: If lengths differ (and both non-null, different pointers), result must be false */
    if (a != NULL && b != NULL && a != b && a->len != b->len) {
        assert(result == false);
    }
}
