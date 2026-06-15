#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_LEN 32

void aws_string_eq_harness(void) {
    /* ---------- nondet allocation for string a ---------- */
    struct aws_string *a;
    if (nondet_bool()) {
        a = NULL;
    } else {
        size_t a_len = nondet_size_t();
        __CPROVER_assume(a_len <= MAX_STRING_LEN);
        a = malloc(sizeof(struct aws_string) + a_len - 1);
        __CPROVER_assume(a != NULL);
        a->len = a_len;
        a->allocator = aws_default_allocator();
        for (size_t i = 0; i < a_len; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* ---------- nondet allocation for string b ---------- */
    struct aws_string *b;
    if (nondet_bool()) {
        b = NULL;
    } else {
        size_t b_len = nondet_size_t();
        __CPROVER_assume(b_len <= MAX_STRING_LEN);
        b = malloc(sizeof(struct aws_string) + b_len - 1);
        __CPROVER_assume(b != NULL);
        b->len = b_len;
        b->allocator = aws_default_allocator();
        for (size_t i = 0; i < b_len; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* ---------- possibly make the two pointers equal ---------- */
    if (nondet_bool() && a != NULL && b != NULL) {
        b = a;
    }

    /* ---------- save old state (for immutability checks) ---------- */
    struct {
        struct aws_allocator *allocator;
        size_t               len;
    } old_a = {0}, old_b = {0};

    uint8_t *old_a_bytes = NULL;
    uint8_t *old_b_bytes = NULL;

    if (a != NULL) {
        old_a.allocator = a->allocator;
        old_a.len = a->len;
        if (a->len > 0) {
            old_a_bytes = malloc(a->len);
            __CPROVER_assume(old_a_bytes != NULL);
            for (size_t i = 0; i < a->len; ++i) {
                old_a_bytes[i] = a->bytes[i];
            }
        }
    }

    if (b != NULL) {
        old_b.allocator = b->allocator;
        old_b.len = b->len;
        if (b->len > 0) {
            old_b_bytes = malloc(b->len);
            __CPROVER_assume(old_b_bytes != NULL);
            for (size_t i = 0; i < b->len; ++i) {
                old_b_bytes[i] = b->bytes[i];
            }
        }
    }

    /* ---------- call function under test ---------- */
    bool result = aws_string_eq(a, b);

    /* ---------- postcondition: result follows the specification ---------- */
    bool expected;
    if (a == b) {
        expected = true;
    } else if (a == NULL || b == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
    }
    assert(result == expected);

    /* ---------- unchanged fields for a ---------- */
    if (a != NULL) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        for (size_t i = 0; i < a->len; ++i) {
            assert(a->bytes[i] == old_a_bytes[i]);
        }
    }

    /* ---------- unchanged fields for b ---------- */
    if (b != NULL) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        for (size_t i = 0; i < b->len; ++i) {
            assert(b->bytes[i] == old_b_bytes[i]);
        }
    }

    /* ---------- validity invariants ---------- */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }
}
