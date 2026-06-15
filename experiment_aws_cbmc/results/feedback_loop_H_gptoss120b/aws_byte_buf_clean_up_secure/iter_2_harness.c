#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare the byte buffer and give it a valid allocator */
    struct aws_byte_buf buf;
    buf.allocator = aws_default_allocator();               /* allocator must stay unchanged */

    /* 2. Bound the buffer and allocate its internal array */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 3. Save the old state for later comparison */
    struct aws_byte_buf old = buf;

    /* 4. Capture a pointer to a byte inside the buffer (if any) */
    struct store_byte_from_buffer storage = {0};
    if (buf.buffer != NULL && buf.capacity > 0) {
        save_byte_from_array(buf.buffer, buf.capacity, &storage);
    }

    /* 5. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 6. Post‑condition checks */

    /* allocator must be unchanged */
    assert(buf.allocator == old.allocator);

    /* buffer fields must be cleared */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* the captured byte must have been zeroed before the memory was released */
    if (old.buffer != NULL && old.capacity > 0) {
        assert(storage.ptr != NULL);
        assert(*storage.ptr == 0);
    }

    /* the resulting buffer must still satisfy its invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
