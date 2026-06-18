#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* allocator – use the default allocator as required by the harness rules */
    struct aws_allocator *alloc = aws_default_allocator();

    /* src buffer – nondeterministically generated and bounded */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save src state for immutability checks */
    struct aws_byte_buf old_src = src;

    /* dest buffer – uninitialized struct that will be populated by the function */
    struct aws_byte_buf dest;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* The function must always leave the structures in a valid state */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    /* src must be unchanged regardless of success or failure */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* On success the destination must reflect the source */
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.capacity == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
        }

        /* The copied bytes must be identical */
        if (src.len > 0) {
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure the destination is zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
