#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 100

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    bool zero;
    __CPROVER_assume(zero == false || zero == true);

    int result = aws_byte_buf_reset(&buf, zero);

    assert(aws_byte_buf_is_valid(&buf));
    assert(result == AWS_OP_SUCCESS);
    assert(buf.len == 0);
    if (zero && buf.buffer != NULL) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }
}
