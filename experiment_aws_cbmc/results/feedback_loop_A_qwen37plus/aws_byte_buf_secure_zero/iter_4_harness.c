#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    aws_byte_buf_secure_zero(&buf);

    assert(aws_byte_buf_is_valid(&buf));
    if (buf.buffer != NULL) {
        for (size_t i = 0; i < buf.len; i++) {
            assert(buf.buffer[i] == 0);
        }
    }
}
