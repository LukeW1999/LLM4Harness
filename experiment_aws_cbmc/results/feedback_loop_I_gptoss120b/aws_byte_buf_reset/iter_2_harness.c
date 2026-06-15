#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_byte_buf_reset_harness(void) {
    /* allocator – use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* buffer to be reset */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for frame condition checks */
    struct aws_byte_buf old_buf = buf;

    /* Save a byte from the buffer for content‑equality checks (if any) */
    struct store_byte_from_buffer buf_byte;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &buf_byte);
    }

    /* Call the function under test */
    aws_byte_buf_reset(&buf);

    /* Post‑conditions */

    /* Length invariant: length must be zero after reset */
    assert(buf.len == 0);

    /* Capacity must remain unchanged */
    assert(buf.capacity == old_buf.capacity);

    /* Buffer pointer must remain unchanged */
    assert(buf.buffer == old_buf.buffer);

    /* Allocator must remain unchanged */
    assert(buf.allocator == allocator);

    /* If the buffer had data, its contents must be unchanged */
    if (old_buf.buffer != NULL && old_buf.len > 0) {
        assert_byte_from_buffer_matches(old_buf.buffer, &buf_byte);
    }

    /* Validity predicate: the buffer must still satisfy its invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
