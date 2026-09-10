#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness(void) {
    /* Set up buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Nondet zero_contents flag */
    bool zero_contents;

    /* Save old state */
    struct aws_byte_buf old = buf;

    /* Call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* Postconditions from expert harness */
    assert(buf.len == 0);
    assert(buf.allocator == old.allocator);
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    if (zero_contents) {
        assert_all_bytes_are(buf.buffer, 0, buf.capacity);
    }
}
