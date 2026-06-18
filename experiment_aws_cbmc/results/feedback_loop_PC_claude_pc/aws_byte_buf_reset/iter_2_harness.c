#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness(void) {
    /* Set up a byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, 10));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for frame conditions */
    struct aws_byte_buf old_buf = buf;

    /* Nondeterministic zero_contents flag */
    bool zero_contents;

    /* Call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* Postconditions */
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
