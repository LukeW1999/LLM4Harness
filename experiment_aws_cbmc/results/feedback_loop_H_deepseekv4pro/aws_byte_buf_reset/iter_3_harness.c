#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;
    bool zero_content;

    /* Create a valid byte_buf with an allocated buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save original attributes */
    size_t original_capacity = buf.capacity;
    uint8_t *original_buffer = buf.buffer;

    /* Call function under test */
    aws_byte_buf_reset(&buf, zero_content);

    /* Postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == original_capacity);
    assert(buf.buffer == original_buffer);
    if (zero_content && original_capacity > 0) {
        assert_all_zeroes(buf.buffer, original_capacity);
    }
    assert(aws_byte_buf_is_valid(&buf));
}
