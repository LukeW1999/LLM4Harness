#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        struct aws_string str_obj;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str_obj.allocator = aws_default_allocator();
        str_obj.len = len;
        ensure_string_has_allocated_buffer_member(&str_obj);
        __CPROVER_assume(aws_string_is_valid(&str_obj));
        str = &str_obj;
    }

    struct aws_byte_cursor *cur;
    if (nondet_bool()) {
        cur = NULL;
    } else {
        struct aws_byte_cursor cur_obj;
        ensure_byte_cursor_has_allocated_buffer_member(&cur_obj);
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_obj, MAX_BUFFER_SIZE));
        cur = &cur_obj;
    }

    struct aws_string old_str;
    struct store_byte_from_buffer old_str_byte;
    if (str != NULL) {
        old_str = *str;
        if (str->len > 0) {
            save_byte_from_array(str->bytes, str->len, &old_str_byte);
        }
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_byte;
    if (cur != NULL) {
        old_cur = *cur;
        if (cur->len > 0) {
            save_byte_from_array(cur->ptr, cur->len, &old_cur_byte);
        }
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        bool expected = (str->len == cur->len) &&
                        (str->len == 0 || memcmp(str->bytes, cur->ptr, str->len) == 0);
        assert(result == expected);
    }

    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        if (str->len > 0) {
            assert_byte_from_buffer_matches(str->bytes, &old_str_byte);
        }
    }

    if (cur != NULL) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
        if (cur->len > 0) {
            assert_byte_from_buffer_matches(cur->ptr, &old_cur_byte);
        }
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
