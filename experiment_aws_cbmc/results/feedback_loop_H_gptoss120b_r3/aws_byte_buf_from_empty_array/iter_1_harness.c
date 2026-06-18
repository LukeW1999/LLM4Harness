#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>

/* Harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness() {
    /* Allocate destination buffer struct (output) */
    struct aws_byte_buf dest;

    /* Allocate source buffer struct and its internal buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Use the default allocator as required by the library */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Save old states for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src = src;

    /* Call the function under verification */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Allocator must be set to the one passed in */
        assert(dest.allocator == alloc);

        /* Length and capacity must match the source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        /* Buffer pointer and contents must reflect the source */
        if (src.buffer != NULL) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        }
    } else {
        /* On allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* Source buffer must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
