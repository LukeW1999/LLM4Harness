// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_clean_up always "succeeds" (void return).
// After the call:
//   - buf->allocator: CHANGES to NULL
//   - buf->buffer: CHANGES to NULL
//   - buf->len: CHANGES to 0
//   - buf->capacity: CHANGES to 0
//
// === STEP 2: FAILURE PATH ===
// No failure path (void function). N/A.
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf *):
//   - buf->allocator: CHANGED to NULL
//   - buf->buffer: CHANGED to NULL
//   - buf->len: CHANGED to 0
//   - buf->capacity: CHANGED to 0
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES (must hold after call)
//   The resulting buf has all-zero/NULL fields, which satisfies the
//   (capacity==0 && len==0 && buffer==NULL) branch of aws_byte_buf_is_valid.

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_byte_buf_clean_up_harness(void) {
    /* Allocate and set up a valid aws_byte_buf */
    struct aws_byte_buf buf;

    /* Nondeterministically initialize the buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* We need a valid allocator for clean_up to call aws_mem_release */
    buf.allocator = aws_default_allocator();

    /* Ensure the buffer satisfies validity precondition */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under test */
    aws_byte_buf_clean_up(&buf);

    /* === STEP 1 assertions: all fields zeroed/nulled === */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* === STEP 4: validity invariant === */
    assert(aws_byte_buf_is_valid(&buf));
}
