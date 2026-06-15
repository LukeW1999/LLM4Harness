#include <assert.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    struct aws_byte_buf src;
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest;

    struct aws_byte_buf old_src = src;

    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    /* dest must be a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));

    if (src.buffer == NULL) {
        assert(result == AWS_OP_SUCCESS);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == alloc);
    } else {
        if (result == AWS_OP_SUCCESS) {
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.buffer != NULL);
            assert(dest.allocator == alloc);
        } else {
            assert(result == AWS_OP_ERR);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == NULL);
        }
    }
}
