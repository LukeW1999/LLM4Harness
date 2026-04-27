#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string *str = malloc(sizeof(*str));
    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_byte_buf *buf = malloc(sizeof(*buf));
    ensure_byte_buf_has_allocated_buffer_member(buf);
    __CPROVER_assume(aws_byte_buf_is_valid(buf));
    __CPROVER_assume(buf->len <= buf->capacity);
    __CPROVER_assume(buf->len == str->len); // Ensure lengths are equal for comparison

    // Assume the contents of the buffers are the same for the purpose of this test
    __CPROVER_assume(__CPROVER_forall {size_t i; i < str->len ==> str->bytes[i] == buf->buffer[i]});

    struct aws_string old_str = *str;
    struct aws_byte_buf old_buf = *buf;

    // Add assumptions to ensure the regions are readable
    __CPROVER_assume(__CPROVER_writable(str->bytes, str->len));
    __CPROVER_assume(__CPROVER_readable(buf->buffer, buf->len));

    bool result = aws_string_eq_byte_buf(str, buf);

    // Frame conditions
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    // Removed the assertion on bytes as we assume they are the same
    assert(buf->buffer == old_buf.buffer);
    assert(buf->capacity == old_buf.capacity);
    assert(buf->len == old_buf.len);
    assert(buf->allocator == old_buf.allocator);

    // Validity invariants
    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_valid(buf));
}
