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
// buf (struct aws_byte_buf):
//   - buf->allocator: CHANGED to NULL always
//   - buf->buffer: CHANGED to NULL always
//   - buf->len: CHANGED to 0 always
//   - buf->capacity: CHANGED to 0 always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES (must hold after call)
//     After the call: capacity=0, len=0, buffer=NULL => valid (zero case)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_harness(void) {
    /* Allocate and initialize a nondeterministic aws_byte_buf */
    struct aws_byte_buf buf;

    /* Nondeterministically set up the buffer fields */
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));

    /* Ensure the buffer satisfies the validity precondition */
    /* Either: capacity==0 && len==0 && buffer==NULL, or capacity>0 && len<=capacity && buffer is writable */
    if (buf.capacity == 0) {
        buf.len = 0;
        buf.buffer = NULL;
    } else {
        __CPROVER_assume(buf.len <= buf.capacity);
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    }

    /* Precondition: buf must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_byte_buf_clean_up(&buf);

    /* === STEP 1 assertions: all fields changed to zero/NULL === */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* === STEP 4: validity invariant holds after call === */
    assert(aws_byte_buf_is_valid(&buf));
}
