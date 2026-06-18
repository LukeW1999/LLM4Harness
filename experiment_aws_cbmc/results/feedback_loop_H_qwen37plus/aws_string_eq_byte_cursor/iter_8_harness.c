#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string *str = NULL;
    struct aws_byte_cursor cur;
    AWS_ZERO_STRUCT(cur);
    struct aws_byte_cursor *cur_ptr = NULL;

    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    if (!str_is_null) {
        str = ensure_string_is_allocated_bounded_length(MAX_STRING_LENGTH);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!cur_is_null) {
        cur = ensure_byte_cursor_has_allocated_buffer_member_bounded_length(MAX_STRING_LENGTH);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
        cur_ptr = &cur;
    }

    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    if (str == NULL || cur_ptr == NULL) {
        assert(result == false);
    } else {
        if (result) {
            assert(str->len == cur_ptr->len);
            assert(aws_array_eq(str->bytes, str->len, cur_ptr->ptr, cur_ptr->len));
        } else {
            assert(str->len != cur_ptr->len || !aws_array_eq(str->bytes, str->len, cur_ptr->ptr, cur_ptr->len));
        }
    }
}
