#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* allocator used for the operation */
    struct aws_allocator *alloc = aws_default_allocator();

    /* source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* save old state of src (both metadata and contents) */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_storage;
    save_byte_from_array(src.buffer, src.len, &src_storage);

    /* destination buffer (uninitialized) */
    struct aws_byte_buf dest;
    /* keep a copy of the uninitialized dest for the error case */
    struct aws_byte_buf old_dest = dest;

    /* call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the one we passed */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* when source has no buffer, destination must be empty */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* destination must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            /* length and capacity must match source */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* the first src.len bytes must be copied verbatim */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* on allocation failure the function zeroes the destination */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* source must be unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert_byte_from_buffer_matches(src.buffer, &src_storage);

    /* destination must always satisfy its validity invariant */
    assert(aws_byte_buf_is_valid(&dest));
}
