#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str = NULL;
    if (!nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        struct aws_allocator *allocator = aws_default_allocator();
        uint8_t *buf = malloc(len);
        __CPROVER_assume(buf != NULL);
        str = aws_string_new_from_array(allocator, buf, len);
        free(buf);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_byte_cursor *cur = NULL;
    struct aws_byte_cursor cur_obj;
    if (!nondet_bool()) {
        ensure_byte_cursor_has_allocated_buffer_member(&cur_obj);
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_obj, MAX_BUFFER_SIZE));
        cur = &cur_obj;
    }

    struct aws_string old_str;
    struct store_byte_from_buffer old_str_byte;
    if (str != NULL) {
        old_str = *str;
        if (str->len > 0) {
            save_byte_from_array((uint8_t *)aws_string_bytes(str), str->len, &old_str_byte);
        }
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_byte;
    if (cur != NULL) {
        old_cur = *cur;
        if (cur->len > 0) {
            save_byte_from_array((uint8_t *)cur->ptr, cur->len, &old_cur_byte);
        }
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        bool expected = (str->len == cur->len) &&
                        (str->len == 0 || memcmp(aws_string_bytes(str), cur->ptr, str->len) == 0);
        assert(result == expected);
    }

    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        if (str->len > 0) {
            assert_byte_from_buffer_matches((uint8_t *)aws_string_bytes(str), &old_str_byte);
        }
    }

    if (cur != NULL) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
        if (cur->len > 0) {
            assert_byte_from_buffer_matches((uint8_t *)cur->ptr, &old_cur_byte);
        }
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
