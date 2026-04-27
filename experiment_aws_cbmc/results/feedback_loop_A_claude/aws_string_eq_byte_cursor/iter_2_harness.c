#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* str can be NULL or a valid aws_string */
    struct aws_string *str = NULL;
    bool str_is_null;
    if (!str_is_null) {
        str = ensure_string_is_allocated_nondet_length();
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* cur can be NULL or a valid aws_byte_cursor */
    struct aws_byte_cursor *cur = NULL;
    bool cur_is_null;
    if (!cur_is_null) {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }

    /* Save old state for immutability checks */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    const uint8_t *old_str_bytes = (str != NULL) ? str->bytes : NULL;
    struct aws_allocator *old_str_allocator = (str != NULL) ? (struct aws_allocator *)str->allocator : NULL;

    size_t old_cur_len = (cur != NULL) ? cur->len : 0;
    uint8_t *old_cur_ptr = (cur != NULL) ? cur->ptr : NULL;

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Assert postconditions */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        if (str->len != cur->len) {
            assert(result == false);
        }
        if (str->len == 0 && cur->len == 0) {
            assert(result == true);
        }
        assert(result == true || result == false);
    }

    /* Assert immutability - str fields unchanged */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_bytes);
        assert((struct aws_allocator *)str->allocator == old_str_allocator);
    }

    /* Assert immutability - cur fields unchanged */
    if (cur != NULL) {
        assert(cur->len == old_cur_len);
        assert(cur->ptr == old_cur_ptr);
    }

    /* Assert validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
