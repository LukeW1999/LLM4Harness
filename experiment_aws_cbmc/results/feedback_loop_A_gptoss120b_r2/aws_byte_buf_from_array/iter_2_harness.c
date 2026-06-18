#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

/* Harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest; /* output buffer, contents will be set by the function */

    /* Save old state of both buffers */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;

    /* Use the default allocator as required */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 3. Postcondition checks */

    /* src must remain unchanged regardless of the outcome */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* Success guarantees */
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer == NULL) {
            /* When source has no buffer, destination should also have no buffer */
            assert(dest.buffer == NULL);
        } else {
            /* Destination must have a newly allocated buffer different from source */
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
        }
    } else {
        /* Failure guarantees (allocation failure) */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 4. Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
