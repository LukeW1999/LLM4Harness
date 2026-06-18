#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *a;
    struct aws_string *b;

    /* a may be NULL or a valid string */
    if (nondet_bool()) {
        a = NULL;
    } else {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        static struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } a_storage;
        a = &a_storage.s;
        a->allocator = allocator;
        a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* b may be NULL or a valid string */
    if (nondet_bool()) {
        b = NULL;
    } else {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        static struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } b_storage;
        b = &b_storage.s;
        b->allocator = allocator;
        b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save old state for immutability checks */
    size_t old_len_a = 0, old_len_b = 0;
    uint8_t *old_bytes_a = NULL, *old_bytes_b = NULL;

    if (a != NULL) {
        old_len_a = a->len;
        old_bytes_a = (uint8_t *)malloc(old_len_a);
        __CPROVER_assume(old_bytes_a != NULL);
        for (size_t i = 0; i < old_len_a; ++i) {
            old_bytes_a[i] = a->bytes[i];
        }
    }

    if (b != NULL) {
        old_len_b = b->len;
        old_bytes_b = (uint8_t *)malloc(old_len_b);
        __CPROVER_assume(old_bytes_b != NULL);
        for (size_t i = 0; i < old_len_b; ++i) {
            old_bytes_b[i] = b->bytes[i];
        }
    }

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Equality semantics */
    if (a == b) {
        __CPROVER_assert(result, "a == b implies result true");
    } else if (a == NULL || b == NULL) {
        __CPROVER_assert(!result, "one null implies result false");
    } else {
        if (result) {
            __CPROVER_assert(a->len == b->len, "lengths equal when result true");
            for (size_t i = 0; i < a->len; ++i) {
                __CPROVER_assert(a->bytes[i] == b->bytes[i], "bytes equal when result true");
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
            __CPROVER_assert(!all_equal, "not all equal when result false");
        }
    }

    /* Unchanged fields (immutability) */
    if (a != NULL) {
        __CPROVER_assert(a->allocator == allocator, "allocator unchanged for a");
        __CPROVER_assert(a->len == old_len_a, "length unchanged for a");
        for (size_t i = 0; i < old_len_a; ++i) {
            __CPROVER_assert(a->bytes[i] == old_bytes_a[i], "bytes unchanged for a");
        }
    }
    if (b != NULL) {
        __CPROVER_assert(b->allocator == allocator, "allocator unchanged for b");
        __CPROVER_assert(b->len == old_len_b, "length unchanged for b");
        for (size_t i = 0; i < old_len_b; ++i) {
            __CPROVER_assert(b->bytes[i] == old_bytes_b[i], "bytes unchanged for b");
        }
    }

    /* Validity invariants */
    if (a != NULL) {
        __CPROVER_assert(aws_string_is_valid(a), "a remains valid");
    }
    if (b != NULL) {
        __CPROVER_assert(aws_string_is_valid(b), "b remains valid");
    }

    /* Cleanup */
    free(old_bytes_a);
    free(old_bytes_b);
}
