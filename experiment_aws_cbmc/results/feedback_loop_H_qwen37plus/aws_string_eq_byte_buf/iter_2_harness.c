#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    if (nondet_bool()) {
        str = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE);
        __CPROVER_assume(str != NULL);
        ensure_string_is_valid(str);
    }

    if (nondet_bool()) {
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);
        ensure_byte_buf_is_valid(buf);
        ensure_byte_buf_has_allocated_buffer_member(buf);
    }

    struct aws_string old_str;
    if (str != NULL) {
        old_str = *str;
    }

    struct aws_byte_buf old_buf;
    if (buf != NULL) {
        old_buf = *buf;
    }

    bool result = aws_string_eq_byte_buf(str, buf);

    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(aws_string_is_valid(str));
    }

    if (buf != NULL) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->len == old_buf.len);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->buffer == old_buf.buffer);
        assert(aws_byte_buf_is_valid(buf));
    }
}
