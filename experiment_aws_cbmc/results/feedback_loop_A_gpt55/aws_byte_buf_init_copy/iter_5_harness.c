#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(allocator != NULL);

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer old_src_byte;
    bool old_src_byte_saved = src.buffer != NULL && src.len > 0;
    if (old_src_byte_saved) {
        save_byte_from_array(src.buffer, src.len, &old_src_byte);
    }

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(src.len == old_src.len);
    assert(src.buffer == old_src.buffer);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (old_src_byte_saved) {
        assert_byte_from_buffer_matches(src.buffer, &old_src_byte);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_src.len);
        assert(dest.capacity == old_src.capacity);

        if (old_src.capacity == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.allocator == allocator);
            assert(dest.buffer != NULL);
            assert(dest.buffer != old_src.buffer);
            if (old_src.len > 0) {
                assert_bytes_match(dest.buffer, old_src.buffer, old_src.len);
            }
        }
    } else {
        assert(old_src.capacity > 0);
        assert(dest.len == 0);
        assert(dest.buffer == NULL);
        assert(dest.capacity == 0);
    }

    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
