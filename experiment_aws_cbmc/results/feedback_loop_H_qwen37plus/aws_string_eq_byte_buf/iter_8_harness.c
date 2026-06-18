#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string *str = NULL;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    bool str_valid = nondet_bool();
    bool buf_valid = nondet_bool();

    if (str_valid) {
        str = ensure_string_is_allocated();
    }

    if (buf_valid) {
        buf.allocator = allocator;
        ensure_byte_buf_is_valid(&buf);
        ensure_byte_buf_has_allocated_buffer_member(&buf);
    }

    struct aws_string *old_str = NULL;
    if (str_valid) {
        old_str = ensure_string_is_allocated();
        old_str->allocator = str->allocator;
        old_str->len = str->len;
    }

    struct aws_byte_buf old_buf;
    if (buf_valid) {
        old_buf = buf;
    }

    const struct aws_string *str_ptr = str_valid ? str : (const struct aws_string *)NULL;
    const struct aws_byte_buf *buf_ptr = buf_valid ? &buf : (const struct aws_byte_buf *)NULL;
    bool result = aws_string_eq_byte_buf(str_ptr, buf_ptr);
    (void)result;

    if (str_valid) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert(aws_string_is_valid(str));
    }

    if (buf_valid) {
        assert(buf.allocator == old_buf.allocator);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.buffer == old_buf.buffer);
        assert(aws_byte_buf_is_valid(&buf));
    }
}
