#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static struct aws_string *ensure_bounded_aws_string_or_null(void) {
    if (nondet_bool()) {
        return NULL;
    }

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len <= SIZE_MAX - sizeof(struct aws_string));

    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    *(struct aws_allocator **)&str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
    *(size_t *)&str->len = len;

    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }
    ((uint8_t *)str->bytes)[len] = 0;

    __CPROVER_assume(aws_string_is_valid(str));
    __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);

    return str;
}

void aws_string_eq_byte_cursor_harness(void) {
    const struct aws_string *str = ensure_bounded_aws_string_or_null();

    struct aws_byte_cursor cur_obj;
    struct aws_byte_cursor *cur = NULL;

    if (nondet_bool()) {
        cur = &cur_obj;
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
        __CPROVER_assume(cur->len <= MAX_BUFFER_SIZE);
    }

    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    const uint8_t *old_str_bytes = NULL;
    struct store_byte_from_buffer old_str_byte;

    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        old_str_bytes = str->bytes;
        save_byte_from_array(str->bytes, str->len + 1, &old_str_byte);
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_byte;

    if (cur != NULL) {
        old_cur = *cur;
        save_byte_from_array(cur->ptr, cur->len, &old_cur_byte);
    }

    bool expected_result;
    if (str == NULL && cur == NULL) {
        expected_result = true;
    } else if (str == NULL || cur == NULL) {
        expected_result = false;
    } else if (str->len != cur->len) {
        expected_result = false;
    } else {
        expected_result = true;
        for (size_t i = 0; i < str->len; ++i) {
            if (str->bytes[i] != cur->ptr[i]) {
                expected_result = false;
            }
        }
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    assert(result == expected_result);

    if (result) {
        assert(str == NULL || cur == NULL || str->len == cur->len);
        if (str != NULL && cur != NULL) {
            assert_bytes_match(str->bytes, cur->ptr, str->len);
        }
    } else {
        assert(!(str == NULL && cur == NULL));
    }

    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_bytes);
        assert_byte_from_buffer_matches(str->bytes, &old_str_byte);
        assert(aws_string_is_valid(str));
    } else {
        assert(str == NULL);
    }

    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        assert_byte_from_buffer_matches(cur->ptr, &old_cur_byte);
        assert(aws_byte_cursor_is_valid(cur));
    } else {
        assert(cur == NULL);
    }
}
