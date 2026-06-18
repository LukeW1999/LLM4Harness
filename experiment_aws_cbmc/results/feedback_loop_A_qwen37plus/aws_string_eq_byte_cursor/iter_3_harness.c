#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_string *str = NULL;
    struct aws_byte_cursor cur;
    struct aws_byte_cursor *cur_ptr = NULL;

    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    if (!str_is_null) {
        str = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        str->allocator = allocator;
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!cur_is_null) {
        cur_ptr = &cur;
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur_ptr, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(cur_ptr);
        __CPROVER_assume(aws_byte_cursor_is_valid(cur_ptr));
    }

    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    if (str == NULL && cur_ptr == NULL) {
        assert(result == true);
    } else if (str == NULL || cur_ptr == NULL) {
        assert(result == false);
    } else {
        if (result) {
            assert(str->len == cur_ptr->len);
            assert_bytes_match(str->bytes, cur_ptr->ptr, str->len);
        }
        assert(aws_string_is_valid(str));
        assert(aws_byte_cursor_is_valid(cur_ptr));
    }
}
