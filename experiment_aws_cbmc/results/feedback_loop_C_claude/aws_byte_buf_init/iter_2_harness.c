#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_harness(void) {
    /* Non-deterministic inputs */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity;

    /* Preconditions: allocator must be non-NULL */
    __CPROVER_assume(allocator != NULL);

    /* Bound capacity to keep proof tractable */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        /* Step 1: Success path assertions */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
        /* Step 4: Validity invariant on success */
        assert(aws_byte_buf_is_valid(&buf));
    } else {
        /* Step 2: Failure path assertions */
        assert(result == AWS_OP_ERR);
        /* AWS_ZERO_STRUCT was applied, so all fields are zeroed */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
        assert(buf.buffer == NULL);
        /* Validity: len=0 <= capacity=0, capacity==0 so buffer can be NULL */
        assert(aws_byte_buf_is_valid(&buf));
        /* Failure only possible when capacity != 0 and allocation failed */
        assert(capacity != 0);
    }
}
