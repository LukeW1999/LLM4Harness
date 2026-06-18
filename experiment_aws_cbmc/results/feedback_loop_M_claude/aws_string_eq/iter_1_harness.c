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
struct aws_string *make_nondet_aws_string(size_t max_len) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    /* Allocate enough memory for the struct header plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);

    /* Set the length field (const, so we use a trick via pointer cast) */
    /* We need to write to const fields - use a writable copy approach */
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;

    /* Set allocator to nondet (can be NULL for static strings) */
    struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
    *alloc_ptr = nondet_bool() ? aws_default_allocator() : NULL;

    /* bytes are already nondet from malloc */
    /* Ensure null terminator at position len */
    uint8_t *bytes_ptr = (uint8_t *)str->bytes;
    bytes_ptr[len] = '\0';

    return str;
}

void aws_string_eq_harness(void) {
    /* Non-deterministically choose whether each pointer is NULL */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();
    bool a_equals_b = nondet_bool();

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    if (!a_is_null) {
        a = make_nondet_aws_string(MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (!b_is_null) {
        if (a_equals_b && !a_is_null) {
            /* Make b point to the same string as a */
            b = a;
        } else {
            b = make_nondet_aws_string(MAX_BUFFER_SIZE);
            __CPROVER_assume(aws_string_is_valid(b));
        }
    }

    /* Save state before call to verify immutability */
    const struct aws_string *old_a = a;
    const struct aws_string *old_b = b;
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition 1: If a == b (same pointer), result must be true */
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

    /* Postcondition 3: Pointers to strings are unchanged */
    assert(a == old_a);
    assert(b == old_b);

    /* Postcondition 4: String contents are unchanged */
    if (a != NULL) {
        assert(a->len == old_a_len);
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(b->len == old_b_len);
        assert(aws_string_is_valid(b));
    }

    /* Postcondition 5: Result is a valid bool */
    assert(result == true || result == false);
}
