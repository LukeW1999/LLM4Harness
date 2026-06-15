#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_string_eq_byte_cursor_harness() {
    /* Non-deterministic inputs */
    struct aws_string *str = nondet_bool() ? malloc(sizeof(struct aws_string)) : NULL;
    struct aws_byte_cursor *cur = nondet_bool() ? malloc(sizeof(struct aws_byte_cursor)) : NULL;

    /* If str is non-NULL, assume it is valid, bounded, and its bytes are readable */
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_bounded(str, MAX_BUFFER_SIZE));
        __CPROVER_assume(aws_string_is_valid(str));
        /* Ensure that str->bytes points to a readable buffer of size at least str->len + 1 (null terminator) */
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len + 1));
    }

    /* If cur is non-NULL, assume it is bounded, valid, and its buffer is readable */
    if (cur != NULL) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
        __CPROVER_assume(AWS_MEM_IS_READABLE(cur->ptr, cur->len));
    }

    /* Save old state for immutability checks (only if non-NULL) */
    struct aws_string *old_str = NULL;
    struct store_byte_from_buffer *old_str_bytes = NULL;
    if (str != NULL) {
        old_str = malloc(sizeof(struct aws_string));
        *old_str = *str;  /* shallow copy */
        /* Save bytes from str->bytes for later comparison */
        old_str_bytes = malloc(sizeof(struct store_byte_from_buffer));
        save_byte_from_array(str->bytes, str->len, old_str_bytes);
    }
    struct aws_byte_cursor *old_cur = NULL;
    struct store_byte_from_buffer *old_cur_bytes = NULL;
    if (cur != NULL) {
        old_cur = malloc(sizeof(struct aws_byte_cursor));
        *old_cur = *cur;
        old_cur_bytes = malloc(sizeof(struct store_byte_from_buffer));
        save_byte_from_array(cur->ptr, cur->len, old_cur_bytes);
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postconditions based on inputs */
    if (str == NULL && cur == NULL) {
        /* Both NULL -> true */
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        /* Exactly one NULL -> false */
        assert(result == false);
    } else {
        /* Both non-NULL -> comparison result */
        bool expected = aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
        assert(result == expected);
    }

    /* Immutability checks: fields of str (if non-NULL) must not change */
    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        /* Verify bytes themselves are unchanged */
        assert_bytes_match(str->bytes, old_str->bytes, str->len);
        /* Also ensure valid string after call */
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(cur->ptr == old_cur->ptr);
        assert(cur->len == old_cur->len);
        /* Verify bytes pointed to by cur are unchanged */
        assert_bytes_match(cur->ptr, old_cur->ptr, cur->len);
        assert(aws_byte_cursor_is_valid(cur));
    }
}
