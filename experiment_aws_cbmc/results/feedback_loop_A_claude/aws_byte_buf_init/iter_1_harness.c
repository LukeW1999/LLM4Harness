#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

/**
 * Harness for aws_byte_buf_init
 *
 * Analysis:
 * 1. Fields that change on SUCCESS:
 *    - buf->buffer: set to NULL if capacity==0, else allocated memory
 *    - buf->len: set to 0
 *    - buf->capacity: set to the provided capacity
 *    - buf->allocator: set to the provided allocator
 *
 * 2. Fields unchanged (implicit): N/A — all fields are written on success
 *
 * 3. On FAILURE (capacity != 0 and allocation fails):
 *    - buf is zeroed out (AWS_ZERO_STRUCT(*buf))
 *    - returns AWS_OP_ERR
 *
 * 4. Validity invariants:
 *    - aws_byte_buf_is_valid(buf) must hold after successful call
 */

void aws_byte_buf_init_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(buf != NULL);

    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);

    /* Bound capacity to keep state space manageable */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 2. Call function under test */
    int result = aws_byte_buf_init(buf, allocator, capacity);

    /* 3. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: len == 0 */
        assert(buf->len == 0);

        /* On success: capacity matches what was passed in */
        assert(buf->capacity == capacity);

        /* On success: allocator matches what was passed in */
        assert(buf->allocator == allocator);

        /* On success: buffer is NULL iff capacity == 0 */
        if (capacity == 0) {
            assert(buf->buffer == NULL);
        } else {
            assert(buf->buffer != NULL);
        }

        /* Validity invariant must hold */
        assert(aws_byte_buf_is_valid(buf));

    } else {
        /* result == AWS_OP_ERR */
        /* On failure: buf is zeroed out */
        assert(buf->len == 0);
        assert(buf->capacity == 0);
        assert(buf->buffer == NULL);
        assert(buf->allocator == NULL);

        /* Failure only possible when capacity != 0 and allocation failed */
        assert(capacity != 0);
    }
}
