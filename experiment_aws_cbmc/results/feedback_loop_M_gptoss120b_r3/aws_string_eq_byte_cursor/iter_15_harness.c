#include <assert.h>
#include <stdbool.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;

    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        struct aws_string str_local;
        str = &str_local;
        str->allocator = aws_default_allocator();
        str->len = len;
        ensure_aws_string_has_allocated_buffer_member(str);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_byte_cursor cur;
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    struct aws_string *old_str_ptr = str;
    struct aws_string old_str_struct;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str_struct = *str;
        save_byte_from_array((uint8_t *)str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur = cur;
    struct store_byte_from_buffer old_cur_bytes;
    save_byte_from_array((uint8_t *)cur.ptr, cur.len, &old_cur_bytes);

    bool result = aws_string_eq_byte_cursor(str, &cur);

    if (str == NULL) {
        assert(result == false);
    } else {
        struct aws_byte_cursor str_cur = aws_byte_cursor_from_string(str);
        bool expected = aws_byte_cursor_eq(&str_cur, &cur);
        assert(result == expected);
    }

    if (str != NULL) {
        assert(str->allocator == old_str_struct.allocator);
        assert(str->len == old_str_struct.len);
        assert_byte_from_buffer_matches((uint8_t *)str->bytes, &old_str_bytes);
    } else {
        assert(old_str_ptr == NULL);
    }

    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);
    assert_byte_from_buffer_matches((uint8_t *)cur.ptr, &old_cur_bytes);

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
