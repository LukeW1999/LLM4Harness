// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_advance returns true (success):
//   - buffer->len: CHANGES to old_buffer_len + len
//   - buffer->capacity: UNCHANGED
//   - buffer->buffer: UNCHANGED
//   - buffer->allocator: UNCHANGED
//   - output->buffer: CHANGES to (buffer->buffer == NULL ? NULL : old_buffer->buffer + old_buffer_len)
//   - output->len: CHANGES to 0
//   - output->capacity: CHANGES to len
//   - output->allocator: CHANGES to NULL (aws_byte_buf_from_array sets allocator to NULL)
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_advance returns false (failure):
//   - buffer->len: UNCHANGED
//   - buffer->capacity: UNCHANGED
//   - buffer->buffer: UNCHANGED
//   - buffer->allocator: UNCHANGED
//   - output: AWS_ZERO_STRUCT applied, so all fields become 0/NULL
//     - output->len: 0
//     - output->capacity: 0
//     - output->buffer: NULL
//     - output->allocator: NULL
//
// === STEP 3: FRAME CONDITIONS ===
//   buffer (struct aws_byte_buf):
//     - len: CHANGED on success (old_len + len), UNCHANGED on failure
//     - capacity: UNCHANGED always
//     - buffer: UNCHANGED always
//     - allocator: UNCHANGED always
//   output (struct aws_byte_buf):
//     - len: CHANGED always (0 on success, 0 on failure)
//     - capacity: CHANGED always (len on success, 0 on failure)
//     - buffer: CHANGED always (ptr on success, NULL on failure)
//     - allocator: CHANGED always (NULL on success, NULL on failure)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buffer): YES (must hold after call)
//   - aws_byte_buf_is_valid(output): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_buf_advance_harness(void) {
    /* Set up buffer with nondeterministic but valid state */
    struct aws_byte_buf buffer;
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* Set up output buffer (will be overwritten) */
    struct aws_byte_buf output;

    /* Nondeterministic len to advance */
    size_t len = nondet_size_t();

    /* Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* Call the function */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        /* SUCCESS PATH */
        /* buffer->len increased by len */
        assert(buffer.len == old_buffer.len + len);
        /* buffer->capacity unchanged */
        assert(buffer.capacity == old_buffer.capacity);
        /* buffer->buffer unchanged */
        assert(buffer.buffer == old_buffer.buffer);
        /* buffer->allocator unchanged */
        assert(buffer.allocator == old_buffer.allocator);

        /* output->len is 0 */
        assert(output.len == 0);
        /* output->capacity is len */
        assert(output.capacity == len);
        /* output->buffer points to old buffer position (or NULL if buffer was NULL) */
        if (old_buffer.buffer == NULL) {
            assert(output.buffer == NULL);
        } else {
            assert(output.buffer == old_buffer.buffer + old_buffer.len);
        }
        /* output->allocator is NULL (set by aws_byte_buf_from_array) */
        assert(output.allocator == NULL);

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    } else {
        /* FAILURE PATH */
        /* buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* output zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    }
}
