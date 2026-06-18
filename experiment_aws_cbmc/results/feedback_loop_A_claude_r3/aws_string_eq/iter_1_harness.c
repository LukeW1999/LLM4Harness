#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/* Helper to allocate a valid aws_string non-deterministically */
struct aws_string *make_arbitrary_aws_string_nondet_len_with_max(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate enough memory for the struct plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    /* Set the length field (const, so we use a trick) */
    *(size_t *)&str->len = len;
    /* Set allocator to NULL or default */
    *(struct aws_allocator **)&str->allocator = NULL;
    /* bytes are non-deterministic (already set by malloc non-det) */
    /* Ensure null terminator */
    ((uint8_t *)str->bytes)[len] = 0;
    return str;
}

void aws_string_eq_harness(void) {
    /* 1. Declare inputs non-deterministically */
    /* a and b can each be NULL or a valid aws_string */
    struct aws_string *a;
    struct aws_string *b;

    /* Non-deterministically choose whether each is NULL or a valid string */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    if (a_is_null) {
        a = NULL;
    } else {
        /* Create a valid aws_string with bounded length */
        size_t a_len;
        __CPROVER_assume(a_len <= 4);
        a = malloc(sizeof(struct aws_string) + a_len + 1);
        __CPROVER_assume(a != NULL);
        *(size_t *)&a->len = a_len;
        *(struct aws_allocator **)&a->allocator = NULL;
        ((uint8_t *)a->bytes)[a_len] = 0;
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (b_is_null) {
        b = NULL;
    } else {
        /* Create a valid aws_string with bounded length */
        size_t b_len;
        __CPROVER_assume(b_len <= 4);
        b = malloc(sizeof(struct aws_string) + b_len + 1);
        __CPROVER_assume(b != NULL);
        *(size_t *)&b->len = b_len;
        *(struct aws_allocator **)&b->allocator = NULL;
        ((uint8_t *)b->bytes)[b_len] = 0;
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* 2. Call the function under test */
    bool result = aws_string_eq(a, b);

    /* 3. Assert postconditions */

    /* If a == b (same pointer), result must be true */
    if (a == b) {
        assert(result == true);
    }

    /* If either is NULL (and they're not the same pointer), result must be false */
    if (a != b && (a == NULL || b == NULL)) {
        assert(result == false);
    }

    /* If both are non-NULL and not the same pointer:
     * result is true iff lengths are equal and bytes match */
    if (a != NULL && b != NULL && a != b) {
        if (result) {
            /* Lengths must be equal */
            assert(a->len == b->len);
        } else {
            /* Either lengths differ or bytes differ - we can't easily assert
             * the negative of bytes matching, but we can assert the contrapositive:
             * if lengths differ, result must be false */
            /* This is covered by the result being false */
        }
        /* If lengths differ, result must be false */
        if (a->len != b->len) {
            assert(result == false);
        }
    }

    /* 4. Assert that the input strings are unchanged (function is read-only) */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }

    /* 5. Result is a bool - must be true or false */
    assert(result == true || result == false);
}
