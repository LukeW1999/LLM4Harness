#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_harness() {
    /* data structure */
    struct aws_byte_buf buf; /* Precondition: buf is non-null */

    /* parameters */
    struct aws_allocator *allocator = aws_default_allocator(); /* Precondition: allocator is non-null */
    size_t capacity;

    /* Assume preconditions */
    __CPROVER_assume(buf.buffer == NULL || AWS_MEM_IS_READABLE(buf.buffer, buf.capacity));
    __CPROVER_assume(allocator != NULL);

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&buf));
        assert(buf.allocator == allocator);
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
            assert(AWS_MEM_IS_WRITABLE(buf.buffer, capacity));
        }
    } else {
        assert(aws_byte_buf_is_valid(&buf));
        assert(buf.allocator == old_buf.allocator);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.buffer == old_buf.buffer);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
}
