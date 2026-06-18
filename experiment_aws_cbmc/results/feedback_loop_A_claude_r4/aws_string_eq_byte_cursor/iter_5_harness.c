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

void harness(void) {
    /* Non-deterministically create a valid aws_string or NULL */
    struct aws_string *str = nondet_allocate_aws_string_bounded(MAX_STRING_LEN);
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministically create a valid aws_byte_cursor or use NULL */
    struct aws_byte_cursor cur_val;
    bool use_cur;
    const struct aws_byte_cursor *cur = NULL;

    if (use_cur) {
        cur_val.len = nondet_size_t();
        __CPROVER_assume(cur_val.len <= MAX_CURSOR_LEN);
        ensure_byte_cursor_has_allocated_buffer_member(&cur_val);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_val));
        cur = &cur_val;
    }

    /* Save old state for immutability checks */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    size_t old_cur_len = (cur != NULL) ? cur->len : 0;
    uint8_t *old_cur_ptr = (cur != NULL) ? cur->ptr : NULL;

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postcondition: Exactly one NULL => false */
    if (str == NULL && cur != NULL) {
        assert(result == false);
    }
    if (str != NULL && cur == NULL) {
        assert(result == false);
    }

    /* Postcondition: Neither NULL, lengths differ => false */
    if (str != NULL && cur != NULL) {
        if (str->len != cur->len) {
            assert(result == false);
        }
    }

    /* Postcondition: str fields unchanged (immutability) */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        assert(aws_string_is_valid(str));
    }

    /* Postcondition: cur fields unchanged (immutability) */
    if (cur != NULL) {
        assert(cur->len == old_cur_len);
        assert(cur->ptr == old_cur_ptr);
        assert(aws_byte_cursor_is_valid(cur));
    }
}
