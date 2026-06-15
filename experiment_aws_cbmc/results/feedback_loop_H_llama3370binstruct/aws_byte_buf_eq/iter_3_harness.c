#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf a;
    struct aws_byte_buf b;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    bool result = aws_byte_buf_eq(&a, &b);

    if (result) {
        assert(a.len == b.len);
        assert_bytes_match(a.buffer, b.buffer, a.len);
    } else {
        assert(a.len != b.len || !bytes_match(a.buffer, b.buffer, a.len));
    }

    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}

int main() {
    aws_byte_buf_eq_harness();
    return 0;
}
