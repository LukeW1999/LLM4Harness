#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old = buf;
    uint8_t *old_buffer = buf.buffer;
    size_t old_capacity = buf.capacity;

    /* 3. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Post‑conditions */
    /* allocator must be cleared */
    assert(buf.allocator == NULL);
    /* buffer pointer must be cleared */
    assert(buf.buffer == NULL);
    /* length and capacity must be reset */
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* 5. Invariant: the buffer remains valid after cleanup */
    assert(aws_byte_buf_is_valid(&buf));
}
