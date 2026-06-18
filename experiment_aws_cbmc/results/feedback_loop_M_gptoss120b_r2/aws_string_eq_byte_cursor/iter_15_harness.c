#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    uint8_t old_str_bytes_buf[MAX_BUFFER_SIZE];
    bool have_old_str_bytes = false;
    uint8_t *str_bytes = NULL;

    if (!str_is_null) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        uint8_t str_storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)str_storage;

        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = str_len;

        str_bytes = (uint8_t *)(str + 1);

        for (size_t i = 0; i < str_len; ++i) {
            str_bytes[i] = nondet_uint8_t();
        }

        old_str_allocator = str->allocator;
        old_str_len = str->len;
        if (str_len > 0) {
            have_old_str_bytes = true;
            for (size_t i = 0; i < str_len; ++i) {
                old_str_bytes_buf[i] = str_bytes[i];
            }
        }
    }

    struct aws_byte_cursor old_cur;
    uint8_t old_cur_bytes_buf[MAX_BUFFER_SIZE];
    bool have_old_cur_bytes = false;

    struct aws_byte_cursor cur_obj;
    if (!cur_is_null) {
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);

        cur_obj.len = cur_len;

        uint8_t cur_buf[MAX_BUFFER_SIZE];
        cur_obj.ptr = cur_buf;

        for (size_t i = 0; i < cur_len; ++i) {
            cur_buf[i] = nondet_uint8_t();
        }

        cur = &cur_obj;

        old_cur = *cur;
        if (cur_len > 0) {
            have_old_cur_bytes = true;
            for (size_t i = 0; i < cur_len; ++i) {
                old_cur_bytes_buf[i] = ((uint8_t *)cur->ptr)[i];
            }
        }
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    bool expected;
    if (str == NULL && cur == NULL) {
        expected = true;
    } else if (str == NULL || cur == NULL) {
        expected = false;
    } else {
        if (str->len != cur->len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (str_bytes[i] != ((uint8_t *)cur->ptr)[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }

    assert(result == expected);

    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        if (have_old_str_bytes) {
            for (size_t i = 0; i < str->len; ++i) {
                assert(str_bytes[i] == old_str_bytes_buf[i]);
            }
        }
        assert(aws_string_is_valid(str));
    }

    if (cur != NULL) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
        if (have_old_cur_bytes) {
            for (size_t i = 0; i < cur->len; ++i) {
                assert(((uint8_t *)cur->ptr)[i] == old_cur_bytes_buf[i]);
            }
        }
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
