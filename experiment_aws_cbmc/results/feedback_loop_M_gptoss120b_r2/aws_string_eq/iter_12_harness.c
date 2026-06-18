#include <stdbool.h>
#include <stddef.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUF_SIZE 64

void aws_string_eq_harness(void) {
    bool a_is_null = nondet_bool();
    struct aws_string *a = NULL;
    size_t a_len = 0;
    uint8_t a_buf[sizeof(struct aws_string) + MAX_BUF_SIZE];
    if (!a_is_null) {
        a_len = nondet_size_t();
        __CPROVER_assume(a_len <= MAX_BUF_SIZE);
        a = (struct aws_string *)a_buf;
        a->allocator = aws_default_allocator();
        a->len = a_len;
        for (size_t i = 0; i < a_len; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
    }

    bool b_is_null = nondet_bool();
    struct aws_string *b = NULL;
    size_t b_len = 0;
    uint8_t b_buf[sizeof(struct aws_string) + MAX_BUF_SIZE];
    if (!b_is_null) {
        b_len = nondet_size_t();
        __CPROVER_assume(b_len <= MAX_BUF_SIZE);
        b = (struct aws_string *)b_buf;
        b->allocator = aws_default_allocator();
        b->len = b_len;
        for (size_t i = 0; i < b_len; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
    }

    struct aws_allocator *old_a_alloc = NULL;
    size_t old_a_len = 0;
    struct store_byte_from_buffer a_bytes_store;
    if (a != NULL) {
        old_a_alloc = a->allocator;
        old_a_len = a->len;
        save_byte_from_array(a->bytes, a->len, &a_bytes_store);
    }

    struct aws_allocator *old_b_alloc = NULL;
    size_t old_b_len = 0;
    struct store_byte_from_buffer b_bytes_store;
    if (b != NULL) {
        old_b_alloc = b->allocator;
        old_b_len = b->len;
        save_byte_from_array(b->bytes, b->len, &b_bytes_store);
    }

    bool result = aws_string_eq(a, b);

    bool expected;
    if (a == b) {
        expected = true;
    } else if (a == NULL || b == NULL) {
        expected = false;
    } else {
        expected = (a->len == b->len);
        if (expected) {
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    assert(result == expected);

    if (a != NULL) {
        assert(a->allocator == old_a_alloc);
        assert(a->len == old_a_len);
        assert_byte_from_buffer_matches(a->bytes, &a_bytes_store);
    }

    if (b != NULL) {
        assert(b->allocator == old_b_alloc);
        assert(b->len == old_b_len);
        assert_byte_from_buffer_matches(b->bytes, &b_bytes_store);
    }
}
