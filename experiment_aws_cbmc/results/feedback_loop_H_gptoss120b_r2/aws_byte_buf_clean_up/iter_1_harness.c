#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* Harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* 3. Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Save old state of source (must remain unchanged) */
    struct aws_byte_buf old_src = src;

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Destination must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* Allocator is set to the one passed in */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* When source has no buffer, destination is empty */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Destination buffer is newly allocated */
            assert(dest.buffer != NULL);
            /* Length and capacity copy source's values */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* Contents must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
            /* New allocation, so pointers must differ */
            assert(dest.buffer != src.buffer);
        }
    } else {
        /* Failure case – function returns AWS_OP_ERR */
        assert(result == AWS_OP_ERR);

        /* Destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* Still a valid (empty) buffer */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 7. Source buffer must be unchanged regardless of outcome */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));
}
