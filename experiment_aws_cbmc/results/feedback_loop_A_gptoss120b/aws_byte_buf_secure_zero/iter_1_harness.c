#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc;

    /* allocator must be non‑NULL (precondition) */
    __CPROVER_assume(alloc != NULL);

    /* bound the buffers */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src,  MAX_BUFFER_SIZE));

    /* allocate internal members where needed */
    ensure_byte_buf_has_allocated_buffer_member(&src);
    ensure_byte_buf_has_allocated_buffer_member(&dest);

    /* src must be a valid byte buffer (precondition) */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect a copy of src */
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        if (src.buffer != NULL) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        /* src must be unchanged */
        assert(src.buffer   == old_src.buffer);
        assert(src.len      == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator== old_src.allocator);
    } else {
        /* on allocation failure the destination is zeroed */
        assert(dest.buffer   == NULL);
        assert(dest.len      == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator== NULL);

        /* src must be unchanged */
        assert(src.buffer   == old_src.buffer);
        assert(src.len      == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator== old_src.allocator);
    }

    /* 5. Invariant: both structures remain valid */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
