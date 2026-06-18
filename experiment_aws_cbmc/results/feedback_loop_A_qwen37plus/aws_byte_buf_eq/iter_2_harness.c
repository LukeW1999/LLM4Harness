#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    bool result = aws_byte_buf_eq(&a, &b);

    if (result) {
        assert(a.len == b.len);
        if (a.len > 0) {
            assert_bytes_match(a.buffer, b.buffer, a.len);
        }
    }
}
