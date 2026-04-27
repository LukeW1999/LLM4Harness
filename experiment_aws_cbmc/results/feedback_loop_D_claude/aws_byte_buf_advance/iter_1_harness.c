// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_advance returns true (success):
//   - buffer->len: CHANGES to buffer->len + len
//   - buffer->capacity: UNCHANGED
//   - buffer->buffer: UNCHANGED
//   - buffer->allocator: UNCHANGED
//   - output->buffer: CHANGES to (buffer->buffer == NULL ? NULL : buffer->buffer + old_len)
//   - output->capacity: CHANGES to len (from aws_byte_buf_from_array)
//   - output->len: CHANGES to 0 (explicitly set after aws_byte_buf_from_array)
//   - output->allocator: CHANGES to NULL (from aws_byte_buf_from_array which sets allocator=NULL)
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_advance returns false (failure):
//   - buffer->len: UNCHANGED
//   - buffer->capacity: UNCHANGED
//   - buffer->buffer: UNCHANGED
//   - buffer->allocator: UNCHANGED
//   - output: AWS_ZERO_STRUCT applied, so all fields zeroed:
//     - output->len = 0
//     - output->buffer = NULL
//     - output->capacity = 0
//     - output->allocator = NULL
//
// === STEP 3: FRAME CONDITIONS ===
//   buffer (struct aws_byte_buf):
//     - buffer->len: CHANGED on success (+=len), UNCHANGED on failure
//     - buffer->capacity: UNCHANGED always
//     - buffer->buffer: UNCHANGED always
//     - buffer->allocator: UNCHANGED always
//   output (struct aws_byte_buf):
//     - output->len: CHANGED always (0 on success, 0 on failure)
//     - output->buffer: CHANGED always (buffer->buffer+old_len on success, NULL on failure)
//     - output->capacity: CHANGED always (len on success, 0 on failure)
//     - output->allocator: CHANGED always (NULL on success, NULL on failure)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buffer): YES (must hold after call)
//   - aws_byte_buf_is_valid(output): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_buf_advance_harness(void) {
    /* Set up buffer */
    struct aws_byte_buf buffer;
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* Set up output (will be overwritten) */
    struct aws_byte_buf output;

    /* Non-deterministic len */
    size_t len = nondet_size_t();

    /* Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* Call the function */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* === Verify frame conditions for buffer === */
    /* buffer->capacity is always unchanged */
    assert(buffer.capacity == old_buffer.capacity);
    /* buffer->buffer pointer is always unchanged */
    assert(buffer.buffer == old_buffer.buffer);
    /* buffer->allocator is always unchanged */
    assert(buffer.allocator == old_buffer.allocator);

    if (result) {
        /* === SUCCESS PATH === */
        /* Condition: old capacity - old len >= len */
        assert(old_buffer.capacity - old_buffer.len >= len);

        /* buffer->len increased by len */
        assert(buffer.len == old_buffer.len + len);

        /* output->len is 0 */
        assert(output.len == 0);

        /* output->capacity is len */
        assert(output.capacity == len);

        /* output->allocator is NULL (aws_byte_buf_from_array sets allocator=NULL) */
        assert(output.allocator == NULL);

        /* output->buffer points to correct location */
        if (old_buffer.buffer == NULL) {
            assert(output.buffer == NULL);
        } else {
            assert(output.buffer == old_buffer.buffer + old_buffer.len);
        }

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));

    } else {
        /* === FAILURE PATH === */
        /* Condition: old capacity - old len < len */
        assert(old_buffer.capacity - old_buffer.len < len);

        /* buffer->len is unchanged */
        assert(buffer.len == old_buffer.len);

        /* output is zeroed */
        assert(output.len == 0);
        assert(output.buffer == NULL);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    }
}
