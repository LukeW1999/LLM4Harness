#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string str;
    struct aws_byte_cursor cur;

    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    struct aws_string *str_ptr = NULL;
    if (!str_is_null) {
        str.allocator = aws_default_allocator();
        str.len = nondet_size_t();
        __CPROVER_assume(str.len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(&str));
        str_ptr = &str;
    }

    const struct aws_byte_cursor *cur_ptr = NULL;
    if (!cur_is_null) {
        cur.len = nondet_size_t();
        __CPROVER_assume(cur.len <= MAX_BUFFER_SIZE);
        ensure_byte_cursor_has_allocated_buffer_member(&cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
        cur_ptr = &cur;
    }

    bool result = aws_string_eq_byte_cursor(str_ptr, cur_ptr);

    if (str_ptr == NULL && cur_ptr == NULL) {
        assert(result == true);
    } else if (str_ptr == NULL || cur_ptr == NULL) {
        assert(result == false);
    } else {
        if (result) {
            assert(str.len == cur.len);
            assert(aws_array_eq(str.bytes, str.len, cur.ptr, cur.len));
        } else {
            assert(str.len != cur.len || !aws_array_eq(str.bytes, str.len, cur.ptr, cur.len));
        }
    }
}
