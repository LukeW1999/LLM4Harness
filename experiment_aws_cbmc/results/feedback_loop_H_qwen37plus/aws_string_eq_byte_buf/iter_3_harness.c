#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_string *str = NULL;
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = NULL;

    if (nondet_bool()) {
        str = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE);
        __CPROVER_assume(str != NULL);
        ensure_string_is_valid(str);
    }

    if (nondet_bool()) {
        buf_ptr = &buf;
        ensure_byte_buf_is_valid(buf_ptr);
        ensure_byte_buf_has_allocated_buffer_member(buf_ptr);
    }

    struct aws_string old_str;
    if (str != NULL) {
        old_str = *str;
    }

    struct aws_byte_buf old_buf;
    if (buf_ptr != NULL) {
        old_buf = *buf_ptr;
    }

    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(aws_string_is_valid(str));
    }

    if (buf_ptr != NULL) {
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->buffer == old_buf.buffer);
        assert(aws_byte_buf_is_valid(buf_ptr));
    }
}
