#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        struct aws_string *tmp = (struct aws_string *)alloca(sizeof(struct aws_string) + len);
        tmp->allocator = allocator;
        tmp->len = len;
        __CPROVER_assume(aws_string_is_valid(tmp));
        str = tmp;
    }

    struct aws_byte_cursor cur_obj;
    struct aws_byte_cursor *cur;
    if (nondet_bool()) {
        cur = NULL;
    } else {
        cur = &cur_obj;
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
        uint8_t *cbuf = (uint8_t *)alloca(cur_len);
        cur->ptr = cbuf;
        cur->len = cur_len;
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    struct store_byte_from_buffer old_str_bytes;
    if (str) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_bytes;
    if (cur) {
        old_cur = *cur;
        save_byte_from_array(cur->ptr, cur->len, &old_cur_bytes);
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    bool expected;
    if (str == NULL && cur == NULL) {
        expected = true;
    } else if (str == NULL || cur == NULL) {
        expected = false;
    } else {
        expected = (str->len == cur->len) && (memcmp(str->bytes, cur->ptr, str->len) == 0);
    }
    assert(result == expected);

    if (str) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (cur) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        assert_byte_from_buffer_matches(cur->ptr, &old_cur_bytes);
    }

    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (cur) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
