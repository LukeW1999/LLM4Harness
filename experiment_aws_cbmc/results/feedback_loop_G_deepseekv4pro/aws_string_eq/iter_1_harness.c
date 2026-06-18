#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define MAX_STRING_LEN 16

void aws_string_eq_harness() {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* Non-deterministically allocate and initialize a */
    bool a_is_null = nondet_bool();
    if (!a_is_null) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        uint8_t *mem_a = malloc(sizeof(struct aws_string) + len_a + 1);
        __CPROVER_assume(mem_a != NULL);
        struct aws_string *str_a = (struct aws_string *)mem_a;
        struct aws_allocator *alloc_a = nondet_bool() ? NULL : aws_default_allocator();
        memcpy((void *)&str_a->allocator, &alloc_a, sizeof(alloc_a));
        memcpy((void *)&str_a->len, &len_a, sizeof(len_a));
        for (size_t i = 0; i < len_a; i++) {
            str_a->bytes[i] = nondet_uint8_t();
        }
        str_a->bytes[len_a] = 0; /* null terminator */
        a = str_a;
    }

    /* Non-deterministically allocate and initialize b */
    bool b_is_null = nondet_bool();
    if (!b_is_null) {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        uint8_t *mem_b = malloc(sizeof(struct aws_string) + len_b + 1);
        __CPROVER_assume(mem_b != NULL);
        struct aws_string *str_b = (struct aws_string *)mem_b;
        struct aws_allocator *alloc_b = nondet_bool() ? NULL : aws_default_allocator();
        memcpy((void *)&str_b->allocator, &alloc_b, sizeof(alloc_b));
        memcpy((void *)&str_b->len, &len_b, sizeof(len_b));
        for (size_t i = 0; i < len_b; i++) {
            str_b->bytes[i] = nondet_uint8_t();
        }
        str_b->bytes[len_b] = 0;
        b = str_b;
    }

    /* Assume strings are valid (if non-NULL) */
    if (a != NULL) {
        __CPROVER_assume(aws_string_is_valid(a));
    }
    if (b != NULL) {
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save old state for a */
    size_t old_len_a = 0;
    struct aws_allocator *old_alloc_a = NULL;
    uint8_t old_bytes_a[MAX_STRING_LEN];
    if (a != NULL) {
        old_len_a = a->len;
        old_alloc_a = a->allocator;
        for (size_t i = 0; i < a->len; i++) {
            old_bytes_a[i] = a->bytes[i];
        }
    }

    /* Save old state for b */
    size_t old_len_b = 0;
    struct aws_allocator *old_alloc_b = NULL;
    uint8_t old_bytes_b[MAX_STRING_LEN];
    if (b != NULL) {
        old_len_b = b->len;
        old_alloc_b = b->allocator;
        for (size_t i = 0; i < b->len; i++) {
            old_bytes_b[i] = b->bytes[i];
        }
    }

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Assert strings are unchanged */
    if (a != NULL) {
        assert(a->len == old_len_a);
        assert(a->allocator == old_alloc_a);
        for (size_t i = 0; i < a->len; i++) {
            assert(a->bytes[i] == old_bytes_a[i]);
        }
    }
    if (b != NULL) {
        assert(b->len == old_len_b);
        assert(b->allocator == old_alloc_b);
        for (size_t i = 0; i < b->len; i++) {
            assert(b->bytes[i] == old_bytes_b[i]);
        }
    }

    /* Assert return value correctness */
    bool expected;
    if (a == b) {
        expected = true;
    } else if (a == NULL || b == NULL) {
        expected = false;
    } else {
        if (a->len != b->len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < a->len; i++) {
                if (a->bytes[i] != b->bytes[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    assert(result == expected);
}

void aws_string_eq_harness(void) {
    aws_string_eq_harness();
    return 0;
}
