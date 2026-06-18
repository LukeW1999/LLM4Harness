#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    struct aws_byte_buf src;
    struct aws_byte_buf dest = {0};

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer old_src_byte;
    save_byte_from_array(src.buffer, src.len, &old_src_byte);

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_src.len);
        assert(dest.capacity == old_src.capacity);
        assert(dest.allocator == allocator);

        if (old_src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.buffer != old_src.buffer);
            assert_bytes_match(dest.buffer, old_src.buffer, old_src.len);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(old_src.buffer != NULL);
        assert(dest.len == 0);
        assert(dest.buffer == NULL);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    assert(src.len == old_src.len);
    assert(src.buffer == old_src.buffer);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert_byte_from_buffer_matches(src.buffer, &old_src_byte);

    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
