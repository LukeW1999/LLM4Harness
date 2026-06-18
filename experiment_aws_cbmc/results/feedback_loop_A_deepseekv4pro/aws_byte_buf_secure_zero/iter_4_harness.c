#include <assert.h>
#include <string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1000

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    uint8_t *old_buffer = buf.buffer;
    size_t old_len = buf.len;

    aws_byte_buf_secure_zero(&buf);

    /* Postconditions */
    assert(buf.len == 0);
    assert(buf.buffer == old_buffer);

    /* If there was any data, it must be zeroed */
    if (old_buffer != NULL && old_len > 0) {
        for (size_t i = 0; i < old_len; i++) {
            assert(buf.buffer[i] == 0);
        }
    }
}
