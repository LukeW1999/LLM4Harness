#include <stdbool.h>
#include <stddef.h>
#include <stdalign.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor cur;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        alignas(struct aws_string) uint8_t raw[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)raw;
        str->allocator = aws_default_allocator();
        str->len = len;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    uint8_t cur_buf[MAX_BUFFER_SIZE];
    cur.ptr = cur_buf;
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur = cur;
    struct store_byte_from_buffer old_cur_bytes;
    save_byte_from_array(cur.ptr, cur.len, &old_cur_bytes);

    bool result = aws_string_eq_byte_cursor(str, &cur);

    if (str == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, cur.ptr, cur.len);
        assert(result == expected);
    }

    if (str != NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);
    assert_byte_from_buffer_matches(cur.ptr, &old_cur_bytes);

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
