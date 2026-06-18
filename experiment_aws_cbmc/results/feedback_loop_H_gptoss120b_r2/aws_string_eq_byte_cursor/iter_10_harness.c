#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        unsigned char str_buf[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)str_buf;

        str->len = len;
        str->allocator = aws_default_allocator();
        str->bytes = (const uint8_t *)(str_buf + sizeof(struct aws_string));

        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (nondet_bool()) {
        struct aws_byte_cursor cur_obj;
        cur = &cur_obj;

        unsigned char cur_buf[MAX_BUFFER_SIZE];
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);

        cur->ptr = cur_buf;
        cur->len = cur_len;

        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_buf;
    if (cur != NULL) {
        old_cur = *cur;
        save_byte_from_array(cur->ptr, cur->len, &old_cur_buf);
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        assert_byte_from_buffer_matches(cur->ptr, &old_cur_buf);
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    (void)result;
}
