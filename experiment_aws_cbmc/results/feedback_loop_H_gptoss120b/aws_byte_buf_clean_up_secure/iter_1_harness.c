#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old = buf;

    /* Optional: capture a byte to verify zeroing */
    struct store_byte_from_buffer storage;
    if (buf.buffer != NULL && buf.capacity > 0) {
        save_byte_from_array(buf.buffer, buf.capacity, &storage);
    }

    /* 3. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Post‑condition asserts */

    /* Unchanged field */
    assert(buf.allocator == old.allocator);

    /* Fields that must be cleared */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* Verify that a saved byte was zeroed before the buffer was released */
    if (old.buffer != NULL && old.capacity > 0) {
        assert(storage.byte == 0);
    }

    /* 5. Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
