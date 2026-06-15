#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

/* Bound for string length – reuse existing max buffer size macro */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN MAX_BUFFER_SIZE
#endif

void aws_string_eq_harness(void) {
    /* 1. Allocate and nondeterministically initialize two strings */
    struct aws_string *a = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    struct aws_string *b = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);

    /* If allocation fails, nothing to verify */
    __CPROVER_assume(a != NULL);
    __CPROVER_assume(b != NULL);

    /* allocator may be NULL (static string) or nondet – we keep it NULL for simplicity */
    a->allocator = NULL;
    b->allocator = NULL;

    a->len = nondet_size_t();
    b->len = nondet_size_t();

    __CPROVER_assume(a->len <= MAX_STRING_LEN);
    __CPROVER_assume(b->len <= MAX_STRING_LEN);

    /* Bytes are nondeterministic – no need to initialise them explicitly */

    /* 2. Assume both strings satisfy the validity predicate */
    __CPROVER_assume(aws_string_is_valid(a));
    __CPROVER_assume(aws_string_is_valid(b));

    /* 3. Save old immutable state for later comparison */
    struct aws_string old_a = *a;
    struct aws_string old_b = *b;

    struct store_byte_from_buffer a_storage;
    struct store_byte_from_buffer b_storage;
    save_byte_from_array(a->bytes, a->len, &a_storage);
    save_byte_from_array(b->bytes, b->len, &b_storage);

    /* 4. Call the function under test */
    bool result = aws_string_eq(a, b);

    /* 5. Post‑condition: fields of a and b must remain unchanged */
    assert(a->allocator == old_a.allocator);
    assert(b->allocator == old_b.allocator);
    assert(a->len == old_a.len);
    assert(b->len == old_b.len);
    assert_byte_from_buffer_matches(a->bytes, &a_storage);
    assert_byte_from_buffer_matches(b->bytes, &b_storage);

    /* 6. Validity invariants must still hold */
    assert(aws_string_is_valid(a));
    assert(aws_string_is_valid(b));

    /* 7. Functional correctness of the return value */
    if (a == b) {
        /* Same pointer (including both NULL) → true */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* One is NULL, the other is not → false */
        assert(result == false);
    } else {
        /* Both non‑NULL and distinct pointers */
        if (result) {
            /* Equality reported → lengths equal and all bytes equal */
            assert(a->len == b->len);
            for (size_t i = 0; i < a->len; ++i) {
                assert(a->bytes[i] == b->bytes[i]);
            }
        } else {
            /* Inequality reported → either lengths differ or some byte differs */
            bool all_equal = true;
            if (a->len != b->len) {
                all_equal = false;
            } else {
                for (size_t i = 0; i < a->len; ++i) {
                    if (a->bytes[i] != b->bytes[i]) {
                        all_equal = false;
                        break;
                    }
                }
            }
            assert(!all_equal);
        }
    }

    /* 8. Clean up */
    free(a);
    free(b);
}
