#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string *str_ptr = NULL;
    const struct aws_byte_cursor *cur_ptr = NULL;

    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    if (!str_is_null) {
        str_ptr = ensure_string_is_allocated_bounded_length(MAX_BUFFER_SIZE);
    }

    if (!cur_is_null) {
        cur_ptr = ensure_byte_cursor_has_allocated_buffer_member_bounded_length(MAX_BUFFER_SIZE);
    }

    bool result = aws_string_eq_byte_cursor(str_ptr, cur_ptr);

    if (str_ptr == NULL && cur_ptr == NULL) {
        assert(result == true);
    } else if (str_ptr == NULL || cur_ptr == NULL) {
        assert(result == false);
    } else {
        if (result) {
            assert(str_ptr->len == cur_ptr->len);
            assert(aws_array_eq(str_ptr->bytes, str_ptr->len, cur_ptr->ptr, cur_ptr->len));
        } else {
            assert(str_ptr->len != cur_ptr->len || !aws_array_eq(str_ptr->bytes, str_ptr->len, cur_ptr->ptr, cur_ptr->len));
        }
    }
}
