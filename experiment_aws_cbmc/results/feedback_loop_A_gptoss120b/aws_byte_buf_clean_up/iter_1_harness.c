#include <assert.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *alloc;

    /* allocator must be non‑NULL (precondition of the function) */
    __CPROVER_assume(alloc != NULL);

    /* bound the source buffer and make its internal pointer allocated */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* bound the destination buffer (its contents will be overwritten) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* success: dest reflects a copy of src */
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.capacity > 0) {
            assert(dest.buffer != NULL);
        } else {
            assert(dest.buffer == NULL);
        }

        /* src must be unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    } else {
        /* failure: dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* src must be unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    }

    /* 5. Fields that must NOT change regardless of result (dest fields that are
       overwritten on success are already checked above; the only untouched
       field is the allocator pointer passed in, which we already asserted) */

    /* 6. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
