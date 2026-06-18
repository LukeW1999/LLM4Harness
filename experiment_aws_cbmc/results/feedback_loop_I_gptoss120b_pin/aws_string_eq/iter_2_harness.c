#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    struct aws_string *a;
    struct aws_string *b;
    struct aws_allocator *default_alloc = aws_default_allocator();

    /* nondet decide if a is NULL */
    if (nondet_bool()) {
        a = NULL;
    } else {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        a = malloc(sizeof(struct aws_string) + (len_a > 0 ? len_a - 1 : 0));
        __CPROVER_assume(a != NULL);
        a->allocator = default_alloc;
        a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* nondet decide if b is NULL */
    if (nondet_bool()) {
        b = NULL;
    } else {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        b = malloc(sizeof(struct aws_string) + (len_b > 0 ? len_b - 1 : 0));
        __CPROVER_assume(b != NULL);
        b->allocator = default_alloc;
        b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save old state */
    size_t old_a_len = 0;
    struct aws_allocator *old_a_alloc = NULL;
    if (a != NULL) {
        old_a_len = a->len;
        old_a_alloc = a->allocator;
    }

    size_t old_b_len = 0;
    struct aws_allocator *old_b_alloc = NULL;
    if (b != NULL) {
        old_b_len = b->len;
        old_b_alloc = b->allocator;
    }

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postconditions */
    if (a == b) {
        /* both NULL or same pointer */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* one is NULL, the other is not */
        assert(result == false);
    } else {
        /* both non‑NULL and distinct */
        bool manual_eq = true;
        if (a->len != b->len) {
            manual_eq = false;
        } else {
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    manual_eq = false;
                    break;
                }
            }
        }
        assert(result == manual_eq);
    }

    /* Unchanged fields */
    if (a != NULL) {
        assert(a->len == old_a_len);
        assert(a->allocator == old_a_alloc);
    }
    if (b != NULL) {
        assert(b->len == old_b_len);
        assert(b->allocator == old_b_alloc);
    }

    /* Validity invariants */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }

    /* Clean up */
    free(a);
    free(b);
}
