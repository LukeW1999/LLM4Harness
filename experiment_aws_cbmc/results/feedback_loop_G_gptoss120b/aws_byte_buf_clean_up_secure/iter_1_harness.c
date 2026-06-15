#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old = buf;
    const uint8_t *old_buffer = buf.buffer;
    size_t old_capacity = buf.capacity;

    /* 2. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 3. Post‑conditions */

    /* Changed fields */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);

    /* Unchanged fields */
    assert(buf.allocator == old.allocator);

    /* If there was an allocated buffer, it must have been zeroed before being released */
    if (old_buffer != NULL) {
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(old_buffer[i] == 0);
        }
    }

    /* 4. Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
