#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    if (nondet_bool()) {
        str = NULL;
    } else {
        static unsigned char buf[MAX_BUFFER_SIZE];
        static struct aws_string s;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        struct aws_allocator *allocator = aws_default_allocator();
        __CPROVER_assume(allocator != NULL);
        s.allocator = allocator;
        s.len = len;
        s.bytes = buf;
        str = &s;
    }

    if (nondet_bool()) {
        cur = NULL;
    } else {
        static unsigned char buf2[MAX_BUFFER_SIZE];
        static struct aws_byte_cursor c;
        size_t len2 = nondet_size_t();
        __CPROVER_assume(len2 <= MAX_BUFFER_SIZE);
        c.ptr = buf2;
        c.len = len2;
        cur = &c;
    }

    struct aws_string old_str;
    if (str) {
        old_str = *str;
    }
    struct aws_byte_cursor old_cur;
    if (cur) {
        old_cur = *cur;
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    bool expected;
    if (!str && !cur) {
        expected = true;
    } else if (!str || !cur) {
        expected = false;
    } else {
        expected = (str->len == cur->len) && (memcmp(str->bytes, cur->ptr, str->len) == 0);
    }
    assert(result == expected);

    if (str) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(str->bytes == old_str.bytes);
    }
    if (cur) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
    }

    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (cur) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
