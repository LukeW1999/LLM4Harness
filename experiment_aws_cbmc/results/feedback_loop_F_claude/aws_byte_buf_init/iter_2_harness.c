#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_byte_buf_init
 *
 * Assumptions:
 *     - buf is a non-null pointer to an aws_byte_buf
 *     - allocator is a valid aws_allocator (using aws_default_allocator())
 *     - capacity is non-deterministic (bounded by MAX_BUFFER_SIZE)
 *
 * Assertions:
 *     - On success: buf->len == 0, buf->capacity == capacity,
 *                   buf->allocator == allocator,
 *                   (capacity == 0 || buf->buffer != NULL),
 *                   aws_byte_buf_is_valid(buf)
 *     - On failure: buf is zeroed (AWS_ZERO_STRUCT applied),
 *                   aws_byte_buf_is_valid(buf)
 */
void aws_byte_buf_init_harness() {
    /* 1. Declare inputs */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity;

    /* 2. Preconditions: allocator must be non-null */
    __CPROVER_assume(allocator != NULL);

    /* Bound capacity to keep state space manageable */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 3. Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: len is 0, capacity matches, allocator matches,
           buffer is non-null if capacity > 0 */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
        /* Validity invariant */
        assert(aws_byte_buf_is_valid(&buf));
    } else {
        /* On failure: AWS_ZERO_STRUCT was applied, so all fields are zero/null */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
        assert(buf.buffer == NULL);
        /* Validity invariant still holds (capacity==0 and buffer==NULL is valid) */
        assert(aws_byte_buf_is_valid(&buf));
    }
}
