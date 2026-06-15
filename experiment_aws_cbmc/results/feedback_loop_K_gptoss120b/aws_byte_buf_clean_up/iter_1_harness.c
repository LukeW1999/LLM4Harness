/*===  Contract block  =====================================================*/
/*
Preconditions:
  - buf != NULL
  - aws_byte_buf_is_valid(buf)

Postconditions (validity):
  - aws_byte_buf_is_valid(buf) holds after the call
  - buf->allocator == NULL
  - buf->buffer    == NULL
  - buf->len       == 0
  - buf->capacity  == 0

Postconditions (length):
  - buf->len == 0
  - buf->capacity == 0

Postconditions (frame):
  - Only the fields of *buf may be modified.
  - No other memory locations are required to stay unchanged.
*/
/*===  Harness  =============================================================*/

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>

#include "proof_helpers/make_common_data_structures.h"

/* nondet helpers ----------------------------------------------------------- */
size_t nondet_size_t(void);
bool   nondet_bool(void);

/* Helper to create a valid aws_byte_buf according to the precondition */
static void make_valid_byte_buf(struct aws_byte_buf *buf, struct aws_allocator *alloc) {
    /* Decide nondeterministically whether the buffer is empty or has storage */
    bool has_storage = nondet_bool();

    if (has_storage) {
        /* Choose a nondeterministic capacity > 0 */
        size_t capacity = nondet_size_t();
        __CPROVER_assume(capacity > 0);
        __CPROVER_assume(capacity < (SIZE_MAX >> 1)); /* avoid overflow in later ops */

        /* Allocate memory using the real allocator (the proof does not model allocation failures) */
        uint8_t *mem = aws_mem_acquire(alloc, capacity);
        __CPROVER_assume(mem != NULL); /* ensure allocation succeeded for the precondition */

        /* Choose a length that is ≤ capacity */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= capacity);

        buf->buffer    = mem;
        buf->len       = len;
        buf->capacity  = capacity;
        buf->allocator = alloc;
    } else {
        /* Empty buffer */
        buf->buffer    = NULL;
        buf->len       = 0;
        buf->capacity  = 0;
        buf->allocator = alloc;
    }

    /* Enforce the validity precondition */
    __CPROVER_assume(aws_byte_buf_is_valid(buf));
}

/*===  Main verification entry point  =======================================*/
void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Build a buffer that satisfies the precondition */
    make_valid_byte_buf(&buf, alloc);

    /* Save a copy of the original struct for frame checking */
    struct aws_byte_buf buf_old = buf;

    /* Call the function under verification */
    aws_byte_buf_clean_up(&buf);

    /* ---- Postcondition checks ------------------------------------------- */

    /* The buffer must be in a valid state */
    assert(aws_byte_buf_is_valid(&buf));

    /* All fields must be cleared */
    assert(buf.buffer    == NULL);
    assert(buf.allocator == NULL);
    assert(buf.len       == 0);
    assert(buf.capacity  == 0);

    /* ---- Frame condition ------------------------------------------------- */
    /* Only the fields of `buf` may have changed; the original memory pointed
       to by buf_old.buffer (if any) must remain unchanged.  We cannot directly
       observe the deallocation, but we can assert that the pointer value itself
       is no longer reachable from the struct. */
    if (buf_old.buffer != NULL) {
        /* The original memory region must not be aliased by any other field of `buf`. */
        assert(buf.buffer != buf_old.buffer);
    }

    return 0;
}
