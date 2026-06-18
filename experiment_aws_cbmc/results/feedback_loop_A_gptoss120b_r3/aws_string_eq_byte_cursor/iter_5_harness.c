#include <assert.h>
#include <stdbool.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* wrapper for string with flexible array space */
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_BUFFER_SIZE];
    } str_wrapper;

    /* wrapper for cursor with buffer */
    struct {
        struct aws_byte_cursor c;
        uint8_t buf[MAX_BUFFER_SIZE];
    } cur_wrapper;

    /* possibly NULL string */
    if (nondet_bool()) {
        str = NULL;
    } else {
        str = &str_wrapper.s;
        str->allocator = aws_default_allocator();

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str_wrapper.bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* possibly NULL cursor */
    if (nondet_bool()) {
        cur = NULL;
    } else {
        cur = &cur_wrapper.c;

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        cur->len = len;
        cur->ptr = cur_wrapper.buf;

        for (size_t i = 0; i < len; ++i) {
            cur_wrapper.buf[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str;                     /* copy header fields */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_bytes;
    if (cur != NULL) {
        old_cur = *cur;                     /* copy header fields */
        save_byte_from_array(cur->ptr, cur->len, &old_cur_bytes);
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Relationship between inputs and result */
    if (str == NULL && cur == NULL) {
        assert(result);
    } else if (str == NULL || cur == NULL) {
        assert(!result);
    } else {
        assert(result == aws_array_eq(str->bytes, str->len, cur->ptr, cur->len));
    }

    /* Unchanged fields for str */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    /* Unchanged fields for cur */
    if (cur != NULL) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
        assert_byte_from_buffer_matches(cur->ptr, &old_cur_bytes);
    }

    /* Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
