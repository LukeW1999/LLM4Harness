// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_clean_up_secure is void - no return value.
// It calls aws_byte_buf_secure_zero (zeros buffer contents, sets len=0)
// then aws_byte_buf_clean_up (frees buffer, sets buffer=NULL, len=0, capacity=0, allocator=NULL)
// After call:
//   - buf->len: CHANGES to 0
//   - buf->buffer: CHANGES to NULL (freed and nulled)
//   - buf->capacity: CHANGES to 0
//   - buf->allocator: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// Function is void, no failure path.
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - buf->len: CHANGED to 0
//   - buf->buffer: CHANGED to NULL
//   - buf->capacity: CHANGED to 0
//   - buf->allocator: CHANGED to NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
// After clean_up_secure, the buf is zeroed out.
// aws_byte_buf_is_valid checks: buf != NULL, and if buffer==NULL then len==0 and capacity==0,
// or if buffer!=NULL then len<=capacity.
// After clean_up_secure: buffer=NULL, len=0, capacity=0 => valid.
// aws_byte_buf_is_valid(buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    /* Allocate and initialize a nondeterministic aws_byte_buf */
    struct aws_byte_buf buf;

    /* Set up nondeterministic fields */
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = NULL; /* Use NULL allocator to avoid actual memory operations */

    /* Ensure len <= capacity for a valid buffer */
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate a buffer if capacity > 0 */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* If buffer is NULL, len and capacity must be 0 */
    if (buf.buffer == NULL) {
        buf.len = 0;
        buf.capacity = 0;
    }

    /* Precondition: buf must be valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Post-conditions: after clean_up_secure, all fields should be zeroed */
    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* The resulting buf should be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
