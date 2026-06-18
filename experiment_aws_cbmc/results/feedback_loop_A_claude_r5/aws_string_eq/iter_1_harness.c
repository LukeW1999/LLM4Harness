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
 * 1. Changed fields: none (pure predicate, no mutation)
 * 2. Unchanged fields: all fields of a and b remain unchanged
 * 3. Failure: returns false when strings differ or one is NULL
 * 4. Validity invariants: a and b remain valid after the call
 */

/* Helper to allocate a valid aws_string non-deterministically */
struct aws_string *make_arbitrary_aws_string_nondet_len_with_max(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate enough memory for the struct plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    /* Cast away const to initialize */
    *(size_t *)&str->len = len;
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    /* bytes are non-deterministic (already set by malloc in CBMC) */
    /* Ensure null terminator */
    ((uint8_t *)str->bytes)[len] = '\0';
    return str;
}

void aws_string_eq_harness(void) {
    /* Declare two aws_string pointers non-deterministically (may be NULL) */
    struct aws_string *a;
    struct aws_string *b;

    /* Non-deterministically decide if each pointer is NULL or valid */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    if (a_is_null) {
        a = NULL;
    } else {
        a = make_arbitrary_aws_string_nondet_len_with_max(4);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (b_is_null) {
        b = NULL;
    } else {
        /* Non-deterministically decide if b is the same pointer as a */
        bool b_same_as_a = nondet_bool();
        if (b_same_as_a && !a_is_null) {
            b = a;
        } else {
            b = make_arbitrary_aws_string_nondet_len_with_max(4);
            __CPROVER_assume(aws_string_is_valid(b));
        }
    }

    /* Save state before call */
    const struct aws_string *old_a = a;
    const struct aws_string *old_b = b;

    /* Save lengths and allocators if non-null */
    size_t a_len = (a != NULL) ? a->len : 0;
    size_t b_len = (b != NULL) ? b->len : 0;
    struct aws_allocator *a_alloc = (a != NULL) ? a->allocator : NULL;
    struct aws_allocator *b_alloc = (b != NULL) ? b->allocator : NULL;

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition 1: If a == b (same pointer, both non-null), must return true */
    if (a != NULL && b != NULL && a == b) {
        assert(result == true);
    }

    /* Postcondition 2: If either is NULL (and they're not the same pointer), must return false */
    if (a == NULL || b == NULL) {
        /* If a == b == NULL, they are the same pointer, but the implementation
         * checks a == b first only if a == b as pointers. Since NULL == NULL,
         * if both are NULL, a == b is true, so it returns true. */
        if (a == b) {
            /* Both NULL: a == b is true, returns true */
            assert(result == true);
        } else {
            assert(result == false);
        }
    }

    /* Postcondition 3: Pointers themselves are unchanged */
    assert(a == old_a);
    assert(b == old_b);

    /* Postcondition 4: Fields of a and b are unchanged */
    if (a != NULL) {
        assert(a->len == a_len);
        assert(a->allocator == a_alloc);
        assert(aws_string_is_valid(a));
    }

    if (b != NULL) {
        assert(b->len == b_len);
        assert(b->allocator == b_alloc);
        assert(aws_string_is_valid(b));
    }

    /* Postcondition 5: Result is a boolean (true or false) */
    assert(result == true || result == false);
}
