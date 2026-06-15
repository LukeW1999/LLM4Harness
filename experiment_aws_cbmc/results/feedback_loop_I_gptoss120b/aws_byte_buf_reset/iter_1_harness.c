#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_byte_buf_init_copy_harness(void) {
    /* allocator – use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* destination buffer (may contain garbage) */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Save old state of dest for later comparison */
    struct aws_byte_buf old_dest = dest;

    /* Save a byte from src for content‑equality checks */
    struct store_byte_from_buffer src_byte;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte);
    }

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* Allocator must be set to the one passed in */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* When src is empty, dest should be an empty buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest must have a non‑NULL buffer with the same capacity as src */
            assert(dest.buffer != NULL);
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            /* The contents up to src.len must be identical */
            assert_bytes_match(src.buffer, dest.buffer, src.len);
        }
    } else {
        /* On failure the function zeroes the destination structure */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* Fields of dest that are not supposed to change on success are already
       covered above; on failure the whole struct is zeroed, which is also
       verified. */

    /* The source buffer must remain unchanged */
    assert(aws_byte_buf_is_valid(&src));
    if (src.buffer != NULL && src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte);
    }

    /* The destination buffer must always satisfy its validity invariant */
    assert(aws_byte_buf_is_valid(&dest));
}
