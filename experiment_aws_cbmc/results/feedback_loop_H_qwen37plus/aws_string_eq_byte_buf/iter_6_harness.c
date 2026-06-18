#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string str;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    bool str_valid = nondet_bool();
    bool buf_valid = nondet_bool();

    if (str_valid) {
        str.allocator = allocator;
        ensure_string_is_valid(&str);
    }

    if (buf_valid) {
        buf.allocator = allocator;
        ensure_byte_buf_is_valid(&buf);
        ensure_byte_buf_has_allocated_buffer_member(&buf);
    }

    struct aws_string old_str;
    if (str_valid) {
        old_str = str;
    }

    struct aws_byte_buf old_buf;
    if (buf_valid) {
        old_buf = buf;
    }

    bool result = aws_string_eq_byte_buf(str_valid ? &str : NULL, buf_valid ? &buf : NULL);
    (void)result;

    if (str_valid) {
        assert(str.allocator == old_str.allocator);
        assert(str.len == old_str.len);
        assert(aws_string_is_valid(&str));
    }

    if (buf_valid) {
        assert(buf.allocator == old_buf.allocator);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.buffer == old_buf.buffer);
        assert(aws_byte_buf_is_valid(&buf));
    }
}
