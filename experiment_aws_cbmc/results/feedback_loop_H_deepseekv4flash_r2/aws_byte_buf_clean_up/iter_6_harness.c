#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf src;
    struct aws_byte_buf old_src;

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    old_src = src;

    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf dest;
    dest.allocator = NULL;
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.capacity >= dest.len);
            for (size_t i = 0; i < src.len; i++) {
                assert(dest.buffer[i] == src.buffer[i]);
            }
        }
    } else {
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(aws_byte_buf_is_valid(&dest));
    }

    assert(aws_byte_buf_is_valid(&src));
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
}
