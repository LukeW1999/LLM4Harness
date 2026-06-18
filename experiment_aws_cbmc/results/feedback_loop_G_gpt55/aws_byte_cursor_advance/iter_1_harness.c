#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer old_src_byte;
    if (src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &old_src_byte);
    }

    struct aws_byte_buf dest;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (old_src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &old_src_byte);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }

        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }

    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        aws_byte_buf_clean_up(&dest);
    }
}

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    struct aws_byte_cursor old_src = src;
    struct store_byte_from_buffer old_src_byte;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &old_src_byte);
    }

    struct aws_byte_buf dest;

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);
    if (old_src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &old_src_byte);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        if (src.ptr == NULL || src.len == 0) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.ptr);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        }

        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }

    assert(aws_byte_cursor_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        aws_byte_buf_clean_up(&dest);
    }
}
