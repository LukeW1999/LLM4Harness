#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN MAX_BUFFER_SIZE
#endif

void aws_string_eq_harness(void) {
    /* Allocate backing storage for possible strings */
    struct aws_string *a_mem = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    struct aws_string *b_mem = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(a_mem != NULL);
    __CPROVER_assume(b_mem != NULL);

    /* Nondeterministically decide whether each pointer is NULL or points to the allocated memory */
    struct aws_string *a = nondet_bool() ? a_mem : NULL;
    struct aws_string *b = nondet_bool() ? b_mem : NULL;

    /* Initialise non‑NULL strings */
    if (a != NULL) {
        a->allocator = NULL;
        a->len = nondet_size_t();
        __CPROVER_assume(a->len <= MAX_STRING_LEN);
        __CPROVER_assume(aws_string_is_valid(a));
    }
    if (b != NULL) {
        b->allocator = NULL;
        b->len = nondet_size_t();
        __CPROVER_assume(b->len <= MAX_STRING_LEN);
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save old immutable state */
    struct aws_string old_a;
    struct aws_string old_b;
    struct store_byte_from_buffer a_storage;
    struct store_byte_from_buffer b_storage;
    if (a != NULL) {
        old_a = *a;
        save_byte_from_array(a->bytes, a->len, &a_storage);
    }
    if (b != NULL) {
        old_b = *b;
        save_byte_from_array(b->bytes, b->len, &b_storage);
    }

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Post‑conditions: strings must remain unchanged */
    if (a != NULL) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        assert_byte_from_buffer_matches(a->bytes, &a_storage);
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        assert_byte_from_buffer_matches(b->bytes, &b_storage);
        assert(aws_string_is_valid(b));
    }

    /* Functional correctness of the return value */
    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        if (result) {
            assert(a->len == b->len);
            for (size_t i = 0; i < a->len; ++i) {
                assert(a->bytes[i] == b->bytes[i]);
            }
        } else {
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

    /* Clean up */
    free(a_mem);
    free(b_mem);
}
