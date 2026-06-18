#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_harness(void) {
    struct aws_byte_buf buf;
    AWS_ZERO_STRUCT(buf);

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t capacity;
    __CPROVER_assume(capacity == (size_t)capacity);

    /* Save pre-state for frame condition */
    struct aws_allocator *allocator_before = allocator;

    int ret = aws_byte_buf_init(&buf, allocator, capacity);

    /* Postcondition: return value */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* Validity */
        assert(aws_byte_buf_is_valid(&buf));

        /* Length and fields */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
        assert(buf.allocator == allocator);
    } else {
        /* Error case: buffer zeroed */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    /* Frame condition */
    assert(allocator == allocator_before);
}
