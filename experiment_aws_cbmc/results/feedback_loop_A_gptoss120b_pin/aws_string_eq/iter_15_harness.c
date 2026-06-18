#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    uint8_t a_storage[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
    if (!a_is_null) {
        size_t a_len = (size_t)nondet_uint64_t();
        __CPROVER_assume(a_len <= MAX_STRING_LEN);
        a = (struct aws_string *)a_storage;
        a->allocator = aws_default_allocator();
        a->len = a_len;
        for (size_t i = 0; i < a_len; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        a->bytes[a_len] = 0;
    }

    uint8_t b_storage[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
    if (!b_is_null) {
        size_t b_len = (size_t)nondet_uint64_t();
        __CPROVER_assume(b_len <= MAX_STRING_LEN);
        b = (struct aws_string *)b_storage;
        b->allocator = aws_default_allocator();
        b->len = b_len;
        for (size_t i = 0; i < b_len; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        b->bytes[b_len] = 0;
    }

    bool same_ptr = nondet_bool();
    if (same_ptr && a != NULL && b != NULL) {
        b = a;
    }

    struct aws_allocator *old_a_allocator = NULL;
    size_t old_a_len = 0;
    uint8_t old_a_bytes[MAX_STRING_LEN];
    if (a != NULL) {
        old_a_allocator = a->allocator;
        old_a_len = a->len;
        for (size_t i = 0; i < old_a_len; ++i) {
            old_a_bytes[i] = a->bytes[i];
        }
    }

    struct aws_allocator *old_b_allocator = NULL;
    size_t old_b_len = 0;
    uint8_t old_b_bytes[MAX_STRING_LEN];
    if (b != NULL) {
        old_b_allocator = b->allocator;
        old_b_len = b->len;
        for (size_t i = 0; i < old_b_len; ++i) {
            old_b_bytes[i] = b->bytes[i];
        }
    }

    bool result = aws_string_eq(a, b);

    bool expected;
    if (a == b) {
        expected = true;
    } else if (a == NULL || b == NULL) {
        expected = false;
    } else {
        if (a->len != b->len) {
            expected = false;
        } else {
            bool eq = true;
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    eq = false;
                    break;
                }
            }
            expected = eq;
        }
    }
    assert(result == expected);

    if (a != NULL) {
        assert(a->allocator == old_a_allocator);
        assert(a->len == old_a_len);
        for (size_t i = 0; i < a->len; ++i) {
            assert(a->bytes[i] == old_a_bytes[i]);
        }
        assert(aws_string_is_valid(a));
    }

    if (b != NULL) {
        assert(b->allocator == old_b_allocator);
        assert(b->len == old_b_len);
        for (size_t i = 0; i < b->len; ++i) {
            assert(b->bytes[i] == old_b_bytes[i]);
        }
        assert(aws_string_is_valid(b));
    }
}
