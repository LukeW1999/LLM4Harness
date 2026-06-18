#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 10
#endif

#ifndef MAX_CURSOR_LEN
#    define MAX_CURSOR_LEN 10
#endif

void aws_string_eq_byte_cursor_harness(void) {
    /* Non-deterministically decide whether str and cur are NULL */
    bool str_is_null;
    bool cur_is_null;

    const struct aws_string *str = NULL;
    struct aws_byte_cursor cur_val;
    struct aws_byte_cursor *cur = NULL;

    /* Set up str if non-null */
    if (!str_is_null) {
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STRING_LEN);

        struct aws_string *s = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(s != NULL);

        *(struct aws_allocator **)&s->allocator = aws_default_allocator();
        *(size_t *)&s->len = str_len;
        ((uint8_t *)s->bytes)[str_len] = 0;

        __CPROVER_assume(aws_string_is_valid(s));
        str = s;
    }

    /* Set up cur if non-null */
    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_val, MAX_CURSOR_LEN));
        ensure_byte_cursor_has_allocated_buffer_member(&cur_val);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_val));
        cur = &cur_val;
    }

    /* Save old state for immutability checks */
    const struct aws_string *old_str = str;
    struct aws_byte_cursor old_cur_val;
    if (cur != NULL) {
        old_cur_val = *cur;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postcondition 1: Both NULL => true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }

    /* Postcondition 2: Exactly one NULL => false */
    if (str == NULL && cur != NULL) {
        assert(result == false);
    }
    if (str != NULL && cur == NULL) {
        assert(result == false);
    }

    /* Postcondition 3: Neither NULL => result depends on content comparison */
    if (str != NULL && cur != NULL) {
        if (str->len != cur->len) {
            assert(result == false);
        }
        if (str->len == 0 && cur->len == 0) {
            assert(result == true);
        }
    }

    /* Immutability: str pointer unchanged */
    assert(str == old_str);

    /* Immutability: cur fields unchanged */
    if (cur != NULL) {
        assert(cur->ptr == old_cur_val.ptr);
        assert(cur->len == old_cur_val.len);
    }

    /* Validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
