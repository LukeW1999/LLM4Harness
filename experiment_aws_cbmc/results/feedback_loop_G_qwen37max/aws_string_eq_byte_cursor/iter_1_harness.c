#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string *str = NULL;
    struct {
        struct aws_allocator *allocator;
        size_t len;
        uint8_t bytes[MAX_BUFFER_SIZE];
    } str_storage;

    if (nondet_bool()) {
        str_storage.len = nondet_size_t();
        __CPROVER_assume(str_storage.len <= MAX_BUFFER_SIZE);
        str = (struct aws_string *)&str_storage;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_byte_cursor *cur = NULL;
    struct aws_byte_cursor cur_storage;

    if (nondet_bool()) {
        cur = &cur_storage;
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }

    size_t old_str_len = 0;
    struct aws_allocator *old_str_alloc = NULL;
    if (str != NULL) {
        old_str_len = str->len;
        old_str_alloc = str->allocator;
    }

    struct aws_byte_cursor old_cur;
    if (cur != NULL) {
        old_cur = *cur;
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    }

    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_alloc);
        assert(aws_string_is_valid(str));
    }
    
    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        assert(aws_byte_cursor_is_valid(cur));
    }
}
