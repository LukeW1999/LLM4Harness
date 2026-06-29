#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_harness(void) {
    /* Allocate buf non-deterministically on the heap so CBMC can track it */
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));

    /* Precondition: buf must be non-null */
    __CPROVER_assume(buf != NULL);

    /* Use the default allocator (satisfies allocator != NULL precondition) */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* capacity is fully unconstrained (any size_t) */
    size_t capacity;

    /* Call the function under verification */
    int result = aws_byte_buf_init(buf, allocator, capacity);

    /* Postcondition: return value must be either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* Postcondition: buf->len must be 0 on success */
        assert(buf->len == 0);

        /* Postcondition: buf->capacity must equal the requested capacity */
        assert(buf->capacity == capacity);

        /* Postcondition: length <= capacity invariant */
        assert(buf->len <= buf->capacity);

        /* Postcondition: allocator is stored correctly */
        assert(buf->allocator == allocator);

        /* Postcondition: buffer pointer consistency */
        if (capacity == 0) {
            assert(buf->buffer == NULL);
        } else {
            assert(buf->buffer != NULL);
        }

        /* Postcondition: aws_byte_buf_is_valid holds */
        assert(aws_byte_buf_is_valid(buf));

    } else {
        /* result == AWS_OP_ERR: allocation must have failed (capacity != 0) */
        assert(capacity != 0);

        /* Postcondition: buf is zeroed out on failure */
        assert(buf->buffer == NULL);
        assert(buf->len == 0);
        assert(buf->capacity == 0);
        assert(buf->allocator == NULL);
    }
}
