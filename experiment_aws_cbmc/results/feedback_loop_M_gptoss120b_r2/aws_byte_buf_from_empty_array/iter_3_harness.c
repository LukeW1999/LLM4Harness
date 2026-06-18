#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_init_copy_harness(void) {
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_src = src;

    struct aws_byte_buf dest;

    struct aws_allocator *alloc = aws_default_allocator();

    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.capacity > 0) {
            assert(dest.buffer != NULL);
            assert(!memcmp(dest.buffer, src.buffer, src.len));
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
