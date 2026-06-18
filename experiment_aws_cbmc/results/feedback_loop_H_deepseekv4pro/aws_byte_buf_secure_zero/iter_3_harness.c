#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;

    /* Initialize buf with a valid, allocated buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t buf_len = buf.len;
    struct store_byte_from_buffer old_byte;
    if (buf_len > 0) {
        save_byte_from_array(buf.buffer, buf_len, &old_byte);
    }

    aws_byte_buf_secure_zero(&buf);

    /* Postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);               /* length is set to 0 */

    if (buf_len > 0) {
        assert_all_zeroes(buf.buffer, buf_len); /* buffer is zeroed */
    }
}
