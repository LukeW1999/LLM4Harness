#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound the source byte buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(src.len <= src.capacity);

    /* Save old source state for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 2. Declare destination buffer (output) */
    struct aws_byte_buf dest;

    /* 3. Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* src is empty – dest should be zeroed but allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == alloc);
        } else {
            /* src has a buffer – allocation must have succeeded */
            assert(dest.buffer != NULL);
            assert(dest.capacity == src.capacity);
            assert(dest.len == 0);                     /* implementation leaves len at 0 */
            assert(dest.allocator == alloc);

            /* The first src.len bytes must have been copied */
            if (src.len > 0) {
                assert_bytes_match(src.buffer, dest.buffer, src.len);
            }
        }
    } else {
        /* Failure path – allocation failed, dest zeroed and allocator cleared */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 6. Fields that must remain unchanged regardless of result */
    /* src must be unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 7. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
