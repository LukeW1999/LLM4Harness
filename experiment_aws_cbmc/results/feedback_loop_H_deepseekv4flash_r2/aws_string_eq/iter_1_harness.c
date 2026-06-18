#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_SIZE 10

void aws_string_eq_harness() {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* Nondeterministically create string a */
    if (nondet_bool()) {
        size_t len_a;
        __CPROVER_assume(len_a <= MAX_STRING_SIZE);
        a = malloc(sizeof(struct aws_string) + len_a);
        __CPROVER_assume(a != NULL);
        /* Override const fields to set up the string */
        *(struct aws_allocator **)&a->allocator = nondet_bool() ? NULL : aws_default_allocator();
        *(size_t *)&a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        a->bytes[len_a] = 0; /* null terminator */
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* Nondeterministically create string b */
    if (nondet_bool()) {
        size_t len_b;
        __CPROVER_assume(len_b <= MAX_STRING_SIZE);
        b = malloc(sizeof(struct aws_string) + len_b);
        __CPROVER_assume(b != NULL);
        *(struct aws_allocator **)&b->allocator = nondet_bool() ? NULL : aws_default_allocator();
        *(size_t *)&b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        b->bytes[len_b] = 0; /* null terminator */
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save old byte contents for immutability checks */
    uint8_t old_bytes_a[MAX_STRING_SIZE];
    uint8_t old_bytes_b[MAX_STRING_SIZE];
    if (a != NULL) {
        __CPROVER_assume(a->len <= MAX_STRING_SIZE);
        for (size_t i = 0; i < a->len; ++i) {
            old_bytes_a[i] = a->bytes[i];
        }
    }
    if (b != NULL) {
        __CPROVER_assume(b->len <= MAX_STRING_SIZE);
        for (size_t i = 0; i < b->len; ++i) {
            old_bytes_b[i] = b->bytes[i];
        }
    }

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* ---- Postconditions ---- */

    /* 1. Return value correctness */
    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        /* Both non‑NULL and distinct pointers */
        bool bytes_equal = (a->len == b->len);
        if (bytes_equal) {
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    bytes_equal = false;
                    break;
                }
            }
        }
        assert(result == bytes_equal);
    }

    /* 2. Immutability of input strings */
    if (a != NULL) {
        /* len is const but we check it hasn't changed (it's the same struct) */
        /* Byte content must be unchanged */
        for (size_t i = 0; i < a->len; ++i) {
            assert(old_bytes_a[i] == a->bytes[i]);
        }
        /* String remains valid */
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        for (size_t i = 0; i < b->len; ++i) {
            assert(old_bytes_b[i] == b->bytes[i]);
        }
        assert(aws_string_is_valid(b));
    }
}
