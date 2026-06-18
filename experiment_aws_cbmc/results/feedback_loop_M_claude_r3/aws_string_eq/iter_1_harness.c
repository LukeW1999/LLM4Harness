#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_string_eq:
 * Returns true if bytes of string are the same, false otherwise.
 *
 * Analysis:
 * 1. Changed fields: none — this is a pure comparison function
 * 2. Unchanged fields: all fields of both strings remain unchanged
 * 3. Failure path: returns false when strings differ or one is NULL
 * 4. Validity invariants: both strings remain valid after the call
 */

/* Helper to allocate a valid aws_string with bounded length */
struct aws_string *make_bounded_aws_string(size_t max_len) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    /* Allocate memory for the string header + bytes + null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);

    /* Initialize the fields — cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;

    /* bytes are nondet (already nondet from malloc) */
    /* Ensure null terminator */
    ((uint8_t *)str->bytes)[len] = 0;

    return str;
}

void aws_string_eq_harness(void) {
    /* Non-deterministically choose whether each pointer is NULL */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    if (!a_is_null) {
        a = make_bounded_aws_string(MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (!b_is_null) {
        b = make_bounded_aws_string(MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save state before call */
    size_t a_len_before = (a != NULL) ? a->len : 0;
    size_t b_len_before = (b != NULL) ? b->len : 0;
    struct aws_allocator *a_alloc_before = (a != NULL) ? a->allocator : NULL;
    struct aws_allocator *b_alloc_before = (b != NULL) ? b->allocator : NULL;

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postconditions */

    /* If both are NULL, they are considered equal (a == b case) */
    if (a == NULL && b == NULL) {
        /* a == b (both NULL), so returns true */
        assert(result == true);
    }

    /* If one is NULL and the other is not, returns false */
    if (a == NULL && b != NULL) {
        assert(result == false);
    }
    if (a != NULL && b == NULL) {
        assert(result == false);
    }

    /* If a == b (same pointer), returns true */
    if (a != NULL && a == b) {
        assert(result == true);
    }

    /* Unchanged fields: strings must not be modified */
    if (a != NULL) {
        assert(a->len == a_len_before);
        assert(a->allocator == a_alloc_before);
        assert(aws_string_is_valid(a));
    }

    if (b != NULL) {
        assert(b->len == b_len_before);
        assert(b->allocator == b_alloc_before);
        assert(aws_string_is_valid(b));
    }

    /* If result is true and both are non-null and different pointers,
     * then lengths must be equal */
    if (result == true && a != NULL && b != NULL && a != b) {
        assert(a->len == b->len);
    }

    /* If lengths differ and both are non-null, result must be false */
    if (a != NULL && b != NULL && a != b && a->len != b->len) {
        assert(result == false);
    }
}
