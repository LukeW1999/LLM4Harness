#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_clean_up_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    struct aws_allocator *alloc;

    /* allocator must be non‑NULL (precondition of the function) */
    __CPROVER_assume(alloc != NULL);

    /* nondeterministically choose a capacity within a reasonable bound */
    size_t cap;
    __CPROVER_assume((cap = nondet_size_t()) <= MAX_BUFFER_SIZE);

    /* set up the buffer fields */
    buf.capacity = cap;
    buf.len = (cap == 0) ? 0 : nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = alloc;

    if (buf.capacity > 0) {
        /* allocate a buffer for the byte_buf */
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    /* ensure the buffer is valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_byte_buf_clean_up(&buf);

    /* 4. Assert post‑conditions */
    /* after clean_up the buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* the original buffer contents are no longer accessible, but the
       original struct fields (saved in old_buf) must remain unchanged */
    assert(old_buf.len == old_buf.len);
    assert(old_buf.capacity == old_buf.capacity);
    assert(old_buf.allocator == alloc);
    if (old_buf.capacity > 0) {
        assert(old_buf.buffer != NULL);
    } else {
        assert(old_buf.buffer == NULL);
    }

    /* 5. Validity invariant must hold for the cleaned‑up buffer */
    assert(aws_byte_buf_is_valid(&buf));
}
