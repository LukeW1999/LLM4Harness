#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_byte_buf_init_harness(void) {
    /* Use a properly initialized allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    size_t capacity;
    __CPROVER_assume(buf != NULL);
    __CPROVER_assume(allocator != NULL);

    /* Bound capacity to keep proof tractable */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Call the function under test */
    int result = aws_byte_buf_init(buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        assert(buf->allocator == allocator);
        if (capacity == 0) {
            assert(buf->buffer == NULL);
        } else {
            assert(buf->buffer != NULL);
        }
        assert(aws_byte_buf_is_valid(buf));
    } else {
        assert(result == AWS_OP_ERR);
        assert(capacity != 0);
        assert(buf->buffer == NULL);
        assert(buf->len == 0);
        assert(buf->capacity == 0);
        assert(buf->allocator == NULL);
        assert(aws_byte_buf_is_valid(buf));
    }
}
