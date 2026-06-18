#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness(void) {
    /* 1. Set up buffer with bounded size */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 3. Nondet zero_contents flag */
    bool zero_contents;

    /* 4. Call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* 5. Assert postconditions */
    /* len must be reset to 0 */
    assert(buf.len == 0);

    /* capacity must remain the same */
    assert(buf.capacity == old_buf.capacity);

    /* buffer pointer must remain the same */
    assert(buf.buffer == old_buf.buffer);

    /* allocator must remain the same */
    assert(buf.allocator == old_buf.allocator);

    /* buf must still be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
