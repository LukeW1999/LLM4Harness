#include <proof_helpers/make_common_data_structures.h>

size_t nondet_uint(void);
uint8_t nondet_uint8_t(void);
bool nondet_bool(void);

#define MAX_LEN 256

bool aws_string_eq_byte_cursor(const struct aws_string *a, const struct aws_byte_cursor *b);

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    size_t str_len = (size_t)nondet_uint();
    __CPROVER_assume(str_len <= MAX_LEN);
    bool str_is_null = nondet_bool();
    if (str_is_null) {
        str = NULL;
    } else {
        uint8_t str_buf[sizeof(struct aws_string) + MAX_LEN + 1];
        str = (struct aws_string *)str_buf;
        str->allocator = aws_default_allocator();
        str->len = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            str->bytes[i] = (uint8_t)nondet_uint8_t();
        }
        str->bytes[str_len] = 0;
    }

    struct aws_byte_cursor cursor;
    size_t cur_len = (size_t)nondet_uint();
    __CPROVER_assume(cur_len <= MAX_LEN);
    bool cur_ptr_is_null = nondet_bool();
    if (cur_ptr_is_null) {
        cursor.ptr = NULL;
        cursor.len = 0;
    } else {
        uint8_t cur_buf[MAX_LEN];
        cursor.ptr = cur_buf;
        cursor.len = cur_len;
        for (size_t i = 0; i < cur_len; ++i) {
            ((uint8_t *)cursor.ptr)[i] = (uint8_t)nondet_uint8_t();
        }
    }

    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    size_t pre_str_len = 0;
    struct aws_allocator *pre_str_alloc = NULL;
    uint8_t pre_str_bytes[MAX_LEN];
    if (str != NULL) {
        pre_str_len = str->len;
        pre_str_alloc = str->allocator;
        for (size_t i = 0; i < pre_str_len; ++i) {
            pre_str_bytes[i] = str->bytes[i];
        }
    }

    uint8_t pre_cur_bytes[MAX_LEN];
    size_t pre_cur_len = cursor.len;
    const uint8_t *pre_cur_ptr = cursor.ptr;
    if (cursor.ptr != NULL) {
        for (size_t i = 0; i < pre_cur_len; ++i) {
            pre_cur_bytes[i] = ((uint8_t *)cursor.ptr)[i];
        }
    }

    bool result = aws_string_eq_byte_cursor(str, &cursor);

    bool expected;
    if (str == NULL && cursor.ptr == NULL) {
        expected = true;
    } else if (str == NULL || cursor.ptr == NULL) {
        expected = false;
    } else {
        if (str->len != cursor.len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != ((uint8_t *)cursor.ptr)[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }

    assert(result == expected);

    if (str != NULL) {
        assert(str->len == pre_str_len);
        assert(str->allocator == pre_str_alloc);
        for (size_t i = 0; i < pre_str_len; ++i) {
            assert(str->bytes[i] == pre_str_bytes[i]);
        }
    }

    assert(cursor.len == pre_cur_len);
    assert(cursor.ptr == pre_cur_ptr);
    if (cursor.ptr != NULL) {
        for (size_t i = 0; i < pre_cur_len; ++i) {
            assert(((uint8_t *)cursor.ptr)[i] == pre_cur_bytes[i]);
        }
    }
}
