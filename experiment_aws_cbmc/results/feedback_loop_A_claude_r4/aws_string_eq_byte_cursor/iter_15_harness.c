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
    /* Non-deterministically create a valid aws_string */
    struct aws_string *str = ensure_string_is_allocated_bounded_length(MAX_STRING_LEN);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Non-deterministically create a valid aws_byte_cursor */
    struct aws_byte_cursor cur;
    __CPROVER_assume(cur.len <= MAX_CURSOR_LEN);
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    /* Save old state for immutability checks */
    size_t old_str_len = str->len;
    struct aws_allocator *old_str_allocator = str->allocator;

    size_t old_cur_len = cur.len;
    uint8_t *old_cur_ptr = cur.ptr;

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* Postcondition: lengths differ => false */
    if (str->len != cur.len) {
        assert(result == false);
    }

    /* Postcondition: str fields unchanged (immutability) */
    assert(str->len == old_str_len);
    assert(str->allocator == old_str_allocator);
    assert(aws_string_is_valid(str));

    /* Postcondition: cur fields unchanged (immutability) */
    assert(cur.len == old_cur_len);
    assert(cur.ptr == old_cur_ptr);
    assert(aws_byte_cursor_is_valid(&cur));
}
