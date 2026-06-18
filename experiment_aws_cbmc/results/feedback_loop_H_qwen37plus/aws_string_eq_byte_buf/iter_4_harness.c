#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    (void)allocator;

    struct aws_string str;
    struct aws_string *str_ptr = NULL;
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = NULL;

    if (nondet_bool()) {
        str_ptr = &str;
        ensure_string_is_valid(str_ptr);
    }

    if (nondet_bool()) {
        buf_ptr = &buf;
        ensure_byte_buf_is_valid(buf_ptr);
        ensure_byte_buf_has_allocated_buffer_member(buf_ptr);
    }

    struct aws_string old_str;
    if (str_ptr != NULL) {
        old_str = *str_ptr;
    }

    struct aws_byte_buf old_buf;
    if (buf_ptr != NULL) {
        old_buf = *buf_ptr;
    }

    bool result = aws_string_eq_byte_buf(str_ptr, buf_ptr);
    (void)result;

    if (str_ptr != NULL) {
        assert(str_ptr->allocator == old_str.allocator);
        assert(str_ptr->len == old_str.len);
        assert(aws_string_is_valid(str_ptr));
    }

    if (buf_ptr != NULL) {
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->buffer == old_buf.buffer);
        assert(aws_byte_buf_is_valid(buf_ptr));
    }
}
