// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_clean_up_secure calls aws_byte_buf_secure_zero then aws_byte_buf_clean_up
// aws_byte_buf_secure_zero: zeros the buffer contents (memset to 0), does not change struct fields
// aws_byte_buf_clean_up: frees the buffer memory and sets buf->buffer = NULL, buf->len = 0, buf->capacity = 0, buf->allocator = NULL
// After the call:
//   - buf->buffer: CHANGES to NULL
//   - buf->len: CHANGES to 0
//   - buf->capacity: CHANGES to 0
//   - buf->allocator: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// This function has no return value (void) and no failure path.
// It always executes both secure_zero and clean_up.
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - buf->buffer: CHANGED (freed and set to NULL)
//   - buf->len: CHANGED (set to 0)
//   - buf->capacity: CHANGED (set to 0)
//   - buf->allocator: CHANGED (set to NULL)
//
// === STEP 4: VALIDITY INVARIANTS ===
// After clean_up, the buf is in a "zeroed" state:
//   buffer=NULL, len=0, capacity=0, allocator=NULL
// aws_byte_buf_is_valid checks: if buffer is NULL then len==0 and capacity==0
// So aws_byte_buf_is_valid(buf) should hold after the call.

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;

    // Set up a valid aws_byte_buf with allocated buffer
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    // Call the function under test
    aws_byte_buf_clean_up_secure(&buf);

    // === STEP 1 & 3 assertions: After call, buf fields should be zeroed ===
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    // === STEP 4: Validity invariant ===
    assert(aws_byte_buf_is_valid(&buf));
}
