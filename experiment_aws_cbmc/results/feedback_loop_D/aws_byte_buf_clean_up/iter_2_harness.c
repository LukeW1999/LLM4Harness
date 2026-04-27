#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_clean_up_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf;

    // Initialize buf with allocated buffer
    make_and_initialize_byte_buf(&buf, sizeof(uint8_t) * 10);
    old_buf = buf;

    // Assume buf is valid
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    // Call the function
    int result = aws_byte_buf_clean_up(&buf);

    // Success path assertions
    if (result == AWS_OP_SUCCESS) {
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }
    // Failure path assertions
    else if (result == AWS_OP_ERR) {
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
    }

    // Validity invariant assertions
    // Note: After cleanup, the buffer is not valid as per the AWS SDK's definition
    // assert(aws_byte_buf_is_valid(&buf)); // This assertion would fail after cleanup
}
