#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>

#define MAX_STRING_LEN 64

void aws_string_eq_harness(void) {
    uint8_t a_storage[sizeof(struct aws_string) + MAX_STRING_LEN];
    uint8_t b_storage[sizeof(struct aws_string) + MAX_STRING_LEN];

    struct aws_string *a = (struct aws_string *)a_storage;
    struct aws_string *b = (struct aws_string *)b_storage;

    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_STRING_LEN);
    __CPROVER_assume(len_b <= MAX_STRING_LEN);

    struct aws_allocator *allocator = aws_default_allocator();
    a->allocator = allocator;
    b->allocator = allocator;

    a->len = len_a;
    b->len = len_b;

    for (size_t i = 0; i < len_a; ++i) {
        a->bytes[i] = nondet_uint8_t();
    }
    for (size_t i = 0; i < len_b; ++i) {
        b->bytes[i] = nondet_uint8_t();
    }

    __CPROVER_assume(aws_string_is_valid(a));
    __CPROVER_assume(aws_string_is_valid(b));

    struct aws_allocator *old_a_allocator = a->allocator;
    size_t old_a_len = a->len;
    struct aws_allocator *old_b_allocator = b->allocator;
    size_t old_b_len = b->len;

    uint8_t old_a_bytes[MAX_STRING_LEN];
    uint8_t old_b_bytes[MAX_STRING_LEN];
    for (size_t i = 0; i < len_a; ++i) {
        old_a_bytes[i] = a->bytes[i];
    }
    for (size_t i = 0; i < len_b; ++i) {
        old_b_bytes[i] = b->bytes[i];
    }

    bool result = aws_string_eq(a, b);

    bool expected = false;
    if (a == b) {
        expected = true;
    } else if (a != NULL && b != NULL) {
        if (a->len == b->len) {
            bool bytes_eq = true;
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    bytes_eq = false;
                    break;
                }
            }
            expected = bytes_eq;
        }
    }
    __CPROVER_assert(result == expected, "aws_string_eq result matches specification");

    __CPROVER_assert(a->allocator == old_a_allocator, "allocator unchanged for a");
    __CPROVER_assert(a->len == old_a_len, "length unchanged for a");
    for (size_t i = 0; i < len_a; ++i) {
        __CPROVER_assert(a->bytes[i] == old_a_bytes[i], "bytes unchanged for a");
    }

    __CPROVER_assert(b->allocator == old_b_allocator, "allocator unchanged for b");
    __CPROVER_assert(b->len == old_b_len, "length unchanged for b");
    for (size_t i = 0; i < len_b; ++i) {
        __CPROVER_assert(b->bytes[i] == old_b_bytes[i], "bytes unchanged for b");
    }

    __CPROVER_assert(aws_string_is_valid(a), "a remains valid");
    __CPROVER_assert(aws_string_is_valid(b), "b remains valid");
}
