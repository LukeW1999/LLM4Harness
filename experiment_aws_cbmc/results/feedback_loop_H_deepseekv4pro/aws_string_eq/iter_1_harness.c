#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *a;
    struct aws_string *b;

    /* Non-deterministically choose whether each pointer is NULL or valid */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    if (a_is_null) {
        a = NULL;
    } else {
        /* Allocate and initialize a valid aws_string */
        size_t a_len = nondet_size_t();
        __CPROVER_assume(a_len <= MAX_BUFFER_SIZE);
        /* Allocate enough memory for header + data + null terminator */
        size_t a_alloc_size = sizeof(struct aws_string) + a_len;
        a = (struct aws_string *)malloc(a_alloc_size);
        if (a != NULL) {
            /* Initialize fields */
            *(struct aws_allocator **)&a->allocator = aws_default_allocator();
            *(size_t *)&a->len = a_len;
            /* bytes[1] is flexible; we have a_len + 1 bytes available */
            for (size_t i = 0; i < a_len; i++) {
                ((uint8_t *)a->bytes)[i] = nondet_uint8_t();
            }
            ((uint8_t *)a->bytes)[a_len] = '\0';
        }
    }

    if (b_is_null) {
        b = NULL;
    } else {
        size_t b_len = nondet_size_t();
        __CPROVER_assume(b_len <= MAX_BUFFER_SIZE);
        size_t b_alloc_size = sizeof(struct aws_string) + b_len;
        b = (struct aws_string *)malloc(b_alloc_size);
        if (b != NULL) {
            *(struct aws_allocator **)&b->allocator = aws_default_allocator();
            *(size_t *)&b->len = b_len;
            for (size_t i = 0; i < b_len; i++) {
                ((uint8_t *)b->bytes)[i] = nondet_uint8_t();
            }
            ((uint8_t *)b->bytes)[b_len] = '\0';
        }
    }

    /* Ensure validity of non-NULL strings */
    if (a != NULL) {
        __CPROVER_assume(aws_string_is_valid(a));
    }
    if (b != NULL) {
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string *old_a = a;
    struct aws_string *old_b = b;
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;

    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Assert postconditions */

    /* If both pointers are the same (including both NULL), result must be true */
    if (a == b) {
        assert(result == true);
    }

    /* If one is NULL and the other is not, result must be false */
    if (a == NULL && b != NULL) {
        assert(result == false);
    }
    if (a != NULL && b == NULL) {
        assert(result == false);
    }

    /* If both are non-NULL and point to different objects, result depends on content */
    if (a != NULL && b != NULL && a != b) {
        /* If lengths differ, result must be false */
        if (a->len != b->len) {
            assert(result == false);
        }
        /* If lengths are equal, result reflects byte-by-byte comparison */
        if (a->len == b->len) {
            bool expected = true;
            for (size_t i = 0; i < a->len; i++) {
                if (a->bytes[i] != b->bytes[i]) {
                    expected = false;
                    break;
                }
            }
            assert(result == expected);
        }
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* aws_string_eq is a const function — it does not modify its arguments */
    if (a != NULL) {
        assert(a == old_a);
        assert(a->allocator == old_a->allocator);
        assert(a->len == old_a_len);
        /* bytes content unchanged */
        for (size_t i = 0; i < old_a_len; i++) {
            assert(a->bytes[i] == old_a->bytes[i]);
        }
    }
    if (b != NULL) {
        assert(b == old_b);
        assert(b->allocator == old_b->allocator);
        assert(b->len == old_b_len);
        for (size_t i = 0; i < old_b_len; i++) {
            assert(b->bytes[i] == old_b->bytes[i]);
        }
    }

    /* 6. Assert validity invariant always holds */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }

    /* Free allocated memory */
    if (a != NULL) {
        free(a);
    }
    if (b != NULL) {
        free(b);
    }
}
