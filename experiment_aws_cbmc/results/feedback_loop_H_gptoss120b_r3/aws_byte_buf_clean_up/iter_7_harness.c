#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_clean_up_harness() {
    /* Use the default allocator as required by the harness rules */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* -----------------------------------------------------------------
     * 1. Declare and bound the byte buffer (input)
     * ----------------------------------------------------------------- */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = alloc;

    if (buf.capacity > 0) {
        ensure_byte_buf_has_allocated_buffer_member(&buf);
    } else {
        buf.buffer = NULL;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Keep a copy of the original buffer for any needed checks */
    struct aws_byte_buf old_buf = buf;

    /* -----------------------------------------------------------------
     * 2. Call the function under verification
     * ----------------------------------------------------------------- */
    aws_byte_buf_clean_up(&buf);

    /* -----------------------------------------------------------------
     * 3. Post‑condition checks
     * ----------------------------------------------------------------- */
    __CPROVER_assert(buf.buffer == NULL, "buf.buffer NULL after clean_up");
    __CPROVER_assert(buf.len == 0, "buf.len zero after clean_up");
    __CPROVER_assert(buf.capacity == 0, "buf.capacity zero after clean_up");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator NULL after clean_up");

    /* -----------------------------------------------------------------
     * 4. Verify that the original buffer's allocator was not altered before
     *    the clean‑up operation.
     * ----------------------------------------------------------------- */
    __CPROVER_assert(old_buf.allocator == alloc, "original allocator unchanged");
}
