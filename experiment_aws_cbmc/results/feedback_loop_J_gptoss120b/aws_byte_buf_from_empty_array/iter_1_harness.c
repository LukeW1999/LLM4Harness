#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>

/* Maximum size for bounding the source buffer – defined in the Makefile */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare structures and allocator */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Bound and allocate the source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 3. Save old state of source (dest is uninitialized) */
    struct aws_byte_buf old_src = src;

    /* 4. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest reflects src */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        assert(dest.allocator == alloc);

        if (src.buffer != NULL) {
            assert(dest.buffer != NULL);
            /* contents must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        /* src must be unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    } else {
        /* on failure dest is zeroed */
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

    /* 6. Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
