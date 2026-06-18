#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness(void) {
    /* 1. Set up buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;
    uint8_t *old_buffer = buf.buffer;

    /* Save a snapshot of the buffer contents if buffer is non-null */
    uint8_t saved_contents[MAX_BUFFER_SIZE];
    if (buf.buffer != NULL && buf.capacity > 0) {
        __CPROVER_array_copy(saved_contents, buf.buffer);
    }

    /* 3. Call function under test */
    aws_byte_buf_secure_zero(&buf);

    /* 4. Assert postconditions */
    /* The buffer pointer, capacity, and allocator should remain the same */
    assert(buf.buffer == old_buffer);
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_allocator);

    /* buf.len must be set to 0 (catches mutant_0003) */
    assert(buf.len == 0);

    /* buf is still valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* If buffer was non-null, all bytes must be zeroed (catches mutant_0476, mutant_0477, mutant_0483) */
    if (buf.buffer != NULL) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }
}
