#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state for immutability checks */
    struct aws_byte_buf old = buf;
    struct store_byte_from_buffer old_byte;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &old_byte);
    }

    /* 3. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Post‑condition checks */

    /* allocator must remain unchanged */
    assert(buf.allocator == old.allocator);

    /* after clean‑up the buffer should be empty and deallocated */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}
