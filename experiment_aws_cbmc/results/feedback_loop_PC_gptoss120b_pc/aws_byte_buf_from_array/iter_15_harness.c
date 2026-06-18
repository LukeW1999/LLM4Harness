#include <assert.h>
#include <string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE ((size_t)256)

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest;
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_buf old_src = src;

    int result = aws_byte_buf_from_array(&dest, alloc, src.buffer, src.capacity);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == alloc);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.capacity);
        if (src.buffer != NULL) {
            assert(dest.buffer != NULL);
            assert(memcmp(dest.buffer, src.buffer, src.capacity) == 0);
        } else {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        }
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL) {
        assert(memcmp(src.buffer, old_src.buffer, src.len) == 0);
    }

    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
