#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness(void) {
    /* Set up buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Nondeterministic zero_contents flag */
    bool zero_contents;

    /* Save old state */
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;
    uint8_t *old_buffer = buf.buffer;

    /* Call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* Postconditions */
    assert(aws_byte_buf_is_valid(&buf));

    /* len must be reset to 0 */
    assert(buf.len == 0);

    /* capacity must not change */
    assert(buf.capacity == old_capacity);

    /* allocator must not change */
    assert(buf.allocator == old_allocator);

    /* buffer pointer must not change */
    assert(buf.buffer == old_buffer);
}
