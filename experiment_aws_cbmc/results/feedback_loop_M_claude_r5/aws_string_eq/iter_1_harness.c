#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
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
 * Postconditions:
 * 1. Changed fields: only the return value (bool result)
 * 2. Unchanged fields: both strings a and b are not modified
 * 3. Both return paths: true means bytes are equal, false means not equal
 * 4. Validity invariants: both strings remain valid after the call
 */

/* Helper to allocate a nondet aws_string with bounded length */
struct aws_string *make_nondet_aws_string(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate enough memory for the struct header plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) {
        return NULL;
    }

    /* Set the length field (const, so we use a cast trick) */
    *(size_t *)(&str->len) = len;

    /* Set allocator to NULL or default (non-deterministic) */
    struct aws_allocator **alloc_ptr = (struct aws_allocator **)(&str->allocator);
    *alloc_ptr = nondet_bool() ? NULL : aws_default_allocator();

    /* bytes are already non-deterministic from malloc */
    /* Ensure null terminator is present */
    uint8_t *bytes_ptr = (uint8_t *)str->bytes;
    bytes_ptr[len] = '\0';

    return str;
}

void aws_string_eq_harness(void) {
    /* Non-deterministically choose whether to use NULL or valid strings */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();
    bool a_equals_b = nondet_bool();

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    if (!a_is_null) {
        a = make_nondet_aws_string();
        __CPROVER_assume(a != NULL);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (!b_is_null) {
        if (a_equals_b && !a_is_null) {
            /* Make b point to the same string as a */
            b = a;
        } else {
            b = make_nondet_aws_string();
            __CPROVER_assume(b != NULL);
            __CPROVER_assume(aws_string_is_valid(b));
        }
    }

    /* Save state before call to verify immutability */
    size_t a_len_before = (a != NULL) ? a->len : 0;
    size_t b_len_before = (b != NULL) ? b->len : 0;

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition 1: If a == b (same pointer), result must be true */
    if (a != NULL && b != NULL && a == b) {
        assert(result == true);
    }

    /* Postcondition 2: If either is NULL (and they're not the same pointer), result must be false */
    if (a == NULL && b == NULL) {
        /* Both NULL: a == b is true (both are NULL pointer), so result is true */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* One is NULL, the other is not: a != b, and one is NULL, so false */
        assert(result == false);
    }

    /* Postcondition 3: Strings are not modified by the comparison */
    if (a != NULL) {
        assert(a->len == a_len_before);
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(b->len == b_len_before);
        assert(aws_string_is_valid(b));
    }

    /* Postcondition 4: If result is true and both are non-NULL and different pointers,
     * then lengths must be equal (necessary condition for byte equality) */
    if (result == true && a != NULL && b != NULL && a != b) {
        assert(a->len == b->len);
    }

    /* Postcondition 5: If lengths differ (and both non-NULL, different pointers), result must be false */
    if (a != NULL && b != NULL && a != b && a->len != b->len) {
        assert(result == false);
    }
}
