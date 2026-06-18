#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    /* dest is an output buffer; its internal pointer is uninitialized on entry */

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src   = src;

    struct store_byte_from_buffer src_storage;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_storage);
    }

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be the one passed in */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* When source buffer is NULL, destination must be empty */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Destination must have a non‑NULL buffer */
            assert(dest.buffer != NULL);
            /* Length and capacity must match source */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* Buffer contents must be a copy of source */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* Source must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
        if (src.buffer != NULL && src.len > 0) {
            assert_byte_from_buffer_matches(src.buffer, &src_storage);
        }
    } else {
        /* On failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* Destination must not retain any previous state */
        (void)old_dest; /* old_dest is unused in the failure case */

        /* Source must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
        if (src.buffer != NULL && src.len > 0) {
            assert_byte_from_buffer_matches(src.buffer, &src_storage);
        }
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
