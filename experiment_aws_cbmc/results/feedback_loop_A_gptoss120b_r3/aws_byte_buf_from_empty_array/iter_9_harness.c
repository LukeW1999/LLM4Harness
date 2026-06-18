#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness(void) {
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_storage;
    if (src.buffer != NULL) {
        save_byte_from_array(src.buffer, src.capacity, &src_storage);
    }

    struct aws_byte_buf dest;
    struct aws_allocator *alloc = aws_default_allocator();

    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* src must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL) {
        assert_byte_from_buffer_matches(src.buffer, &src_storage);
    }

    /* dest must be a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));

    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == alloc);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
        }
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
