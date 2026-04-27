#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Set up source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Declare destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* 4. Save old src state to check immutability */
    struct aws_byte_buf old_src = src;

    /* Save a byte from src buffer for content verification */
    struct store_byte_from_buffer saved_byte;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &saved_byte);
    }

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest should be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest allocator should be set to the provided allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* When src->buffer is NULL, dest is zeroed except allocator */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest should have same len and capacity as src */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);

            /* dest buffer should be allocated (non-null) */
            assert(dest.buffer != NULL);

            /* Content should match src */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure: dest should be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 7. src must not have been modified */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 8. src remains valid */
    assert(aws_byte_buf_is_valid(&src));
}
