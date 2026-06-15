#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    int result = aws_byte_buf_secure_zero(&buf, allocator);

    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == allocator);
        assert(buf.buffer == NULL);
    } else {
        assert(buf.allocator == allocator);
        assert(buf.capacity == buf.capacity);
        assert(buf.len == buf.len);
        assert(buf.buffer == buf.buffer);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
