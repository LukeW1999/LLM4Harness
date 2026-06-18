#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 8
#endif

#ifndef MAX_CURSOR_LEN
#define MAX_CURSOR_LEN 8
#endif

void aws_string_eq_byte_cursor_harness(void) {
    /* Non-deterministically choose whether str and cur are NULL */
    bool str_is_null;
    bool cur_is_null;

    const struct aws_string *str = NULL;
    struct aws_byte_cursor cur_val;
    const struct aws_byte_cursor *cur = NULL;

    if (!str_is_null) {
        str = ensure_string_is_allocated_bounded(MAX_STRING_LEN);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_val, MAX_CURSOR_LEN));
        ensure_byte_cursor_has_allocated_buffer_member(&cur_val);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_val));
        cur = &cur_val;
    }

    /* Save old state for immutability checks */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    size_t old_cur_len = 0;
    uint8_t *old_cur_ptr = NULL;
    if (cur != NULL) {
        old_cur_len = cur_val.len;
        old_cur_ptr = cur_val.ptr;
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

    /* Postcondition 4: str fields unchanged (immutability) */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
    }

    /* Postcondition 5: cur fields unchanged (immutability) */
    if (cur != NULL) {
        assert(cur_val.len == old_cur_len);
        assert(cur_val.ptr == old_cur_ptr);
    }

    /* Postcondition 6: Validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
