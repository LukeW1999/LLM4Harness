#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));

    /* Preconditions: pointer is non-null */
    __CPROVER_assume(buf != NULL);

    /* Ensure buf is a valid aws_byte_buf */
    __CPROVER_assume(aws_byte_buf_is_valid(buf));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_buf = *buf;

    /* Call the function */
    aws_byte_buf_secure_zero(buf);

    /* Postconditions */
    /* Buffer should be zeroed */
    if (buf->buffer != NULL && buf->len > 0) {
        size_t i;
        for (i = 0; i < buf->len; i++) {
            assert(buf->buffer[i] == 0);
        }
    }
    /* len should be 0 */
    assert(buf->len == 0);
    /* capacity should remain unchanged */
    assert(buf->capacity == old_buf.capacity);
    /* allocator should remain unchanged */
    assert(buf->allocator == old_buf.allocator);
    /* buffer pointer should remain unchanged */
    assert(buf->buffer == old_buf.buffer);
    /* buf is a valid aws_byte_buf */
    assert(aws_byte_buf_is_valid(buf));
}
