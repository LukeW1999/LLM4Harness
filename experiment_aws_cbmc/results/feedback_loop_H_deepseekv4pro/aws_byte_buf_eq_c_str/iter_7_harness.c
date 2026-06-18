#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t c_str_len;
    const char *c_str = ensure_c_str_is_nd_allocated(MAX_BUFFER_SIZE, &c_str_len);

    struct aws_byte_buf old = buf;
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);
}
