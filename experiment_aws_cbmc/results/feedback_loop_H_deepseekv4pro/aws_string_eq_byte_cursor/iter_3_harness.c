#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *str = NULL;
    size_t str_len = 0;
    uint8_t old_str_bytes[MAX_BUFFER_SIZE];

    bool have_str = nondet_bool();
    if (have_str) {
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        uint8_t data[MAX_BUFFER_SIZE];
        for (size_t i = 0; i < str_len; i++) {
            data[i] = nondet_uint8_t();
        }
        str = aws_string_new_from_array(allocator, data, str_len);
        __CPROVER_assume(str != NULL);
        for (size_t i = 0; i < str_len; i++) {
            old_str_bytes[i] = str->bytes[i];
        }
    }

    struct aws_byte_cursor cur = {0};
    bool have_cur = nondet_bool();
    size_t cur_len = 0;
    uint8_t *old_cur_ptr = NULL;
    size_t old_cur_len = 0;
    uint8_t old_cur_bytes[MAX_BUFFER_SIZE];
    uint8_t cur_data[MAX_BUFFER_SIZE]; /* moved outside to keep data alive */

    if (have_cur) {
        cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
        for (size_t i = 0; i < cur_len; i++) {
            cur_data[i] = nondet_uint8_t();
        }
        cur = aws_byte_cursor_from_array(cur_data, cur_len);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
        old_cur_ptr = cur.ptr;
        old_cur_len = cur.len;
        for (size_t i = 0; i < cur_len; i++) {
            old_cur_bytes[i] = cur.ptr[i];
        }
    }

    struct aws_byte_cursor *cur_ptr = have_cur ? &cur : NULL;
    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    if (have_str) {
        assert(str->allocator == allocator);
        assert(str->len == str_len);
        for (size_t i = 0; i < str_len; i++) {
            assert(str->bytes[i] == old_str_bytes[i]);
        }
        assert(str->bytes[str_len] == 0);
        assert(aws_string_is_valid(str));
    }

    if (have_cur) {
        assert(cur.ptr == old_cur_ptr);
        assert(cur.len == old_cur_len);
        for (size_t i = 0; i < old_cur_len; i++) {
            assert(cur.ptr[i] == old_cur_bytes[i]);
        }
        assert(aws_byte_cursor_is_valid(&cur));
    }

    bool expected;
    if (str == NULL && !have_cur) {
        expected = true;
    } else if (str == NULL || !have_cur) {
        expected = false;
    } else {
        if (str->len != cur.len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < str->len; i++) {
                if (str->bytes[i] != cur.ptr[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    assert(result == expected);
}
