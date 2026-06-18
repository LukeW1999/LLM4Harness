#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness(void) {
    /* 1. Set up buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Nondet zero_contents flag */
    bool zero_contents;

    /* 3. Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* 5. Assert postconditions */
    /* len should be reset to 0 */
    assert(buf.len == 0);

    /* capacity and buffer pointer should remain the same */
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* buf is still valid */
    assert(aws_byte_buf_is_valid(&buf));
}
