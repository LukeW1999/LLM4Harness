#include <assert.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        static unsigned char str_buf[MAX_BUFFER_SIZE];
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        struct aws_string s;
        s.allocator = aws_default_allocator();
        s.len = len;
        s.bytes = str_buf;
        str = &s;
    }

    /* nondeterministically decide if cur is NULL */
    if (nondet_bool()) {
        cur = NULL;
    } else {
        static unsigned char cur_buf[MAX_BUFFER_SIZE];
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        struct aws_byte_cursor c;
        c.ptr = cur_buf;
        c.len = len;
        cur = &c;
    }

    /* Save old state */
    struct aws_string old_str_copy;
    if (str != NULL) {
        old_str_copy = *str;
    }
    struct aws_byte_cursor old_cur_copy;
    if (cur != NULL) {
        old_cur_copy = *cur;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postconditions */
    bool expected;
    if (str == NULL && cur == NULL) {
        expected = true;
    } else if (str == NULL || cur == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
    }
    assert(result == expected);

    /* Unchanged fields */
    if (str != NULL) {
        assert(str->allocator == old_str_copy.allocator);
        assert(str->len == old_str_copy.len);
        assert(str->bytes == old_str_copy.bytes);
    }
    if (cur != NULL) {
        assert(cur->ptr == old_cur_copy.ptr);
        assert(cur->len == old_cur_copy.len);
    }

    /* Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
