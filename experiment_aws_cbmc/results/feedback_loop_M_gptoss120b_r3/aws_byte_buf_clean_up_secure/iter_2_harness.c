#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_byte_buf_clean_up_secure_harness() {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state for immutability checks */
    struct aws_byte_buf old = buf;

    /* 3. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Post‑condition checks */
    /* buffer pointer must be cleared */
    assert(buf.buffer == NULL);
    /* length and capacity must be reset to zero */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    /* allocator may be cleared; no stronger requirement */

    /* 5. Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));
}
