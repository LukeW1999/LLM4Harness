#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <alloca.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor cur;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str = (struct aws_string *)alloca(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        str->len = len;
        if (len > 0) {
            __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, len));
        }
    }

    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    struct aws_string *old_str = str;
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL && str->len > 0) {
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur = cur;
    struct store_byte_from_buffer old_cur_bytes;
    if (cur.ptr != NULL && cur.len > 0) {
        save_byte_from_array(cur.ptr, cur.len, &old_cur_bytes);
    }

    bool result = aws_string_eq_byte_cursor(str, &cur);

    bool expected;
    if (str == NULL && cur.ptr == NULL) {
        expected = true;
    } else if (str == NULL || cur.ptr == NULL) {
        expected = false;
    } else {
        if (str->len != cur.len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != cur.ptr[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }

    assert(result == expected);

    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        if (str->len > 0) {
            assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
        }
    }

    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);
    if (cur.ptr != NULL && cur.len > 0) {
        assert_byte_from_buffer_matches(cur.ptr, &old_cur_bytes);
    }

    assert(str == NULL || aws_string_is_valid(str));
    assert(aws_byte_cursor_is_valid(&cur));
}
