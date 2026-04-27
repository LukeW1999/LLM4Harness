#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness() {
    /* data structure */
    struct aws_byte_buf buf; /* Precondition: buf is non-null */

    /* parameters */
    struct aws_allocator *allocator = aws_default_allocator(); /* Precondition: allocator is non-null */
    size_t capacity = nondet_size_t();

    /* Bound the buffer size */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Initialize the buffer */
    if (aws_byte_buf_init(&buf, allocator, capacity) == AWS_OP_SUCCESS) {
        /* assertions before secure zero */
        assert(aws_byte_buf_is_valid(&buf));
        assert(buf.allocator == allocator);
        assert(buf.len == 0);
        assert(buf.capacity == capacity);

        /* Save old state */
        struct aws_byte_buf old = buf;

        /* Call the function under test */
        aws_byte_buf_secure_zero(&buf);

        /* assertions after secure zero */
        assert(aws_byte_buf_is_valid(&buf));
        assert(buf.allocator == old.allocator);
        assert(buf.len == 0);
        assert(buf.capacity == old.capacity);
        __CPROVER_assume(AWS_MEM_IS_READABLE(buf.buffer, buf.capacity)); /* Ensure memory is readable */
        assert(AWS_MEM_IS_ZERO(buf.buffer, buf.capacity)); /* Corrected assertion */
    }
}
