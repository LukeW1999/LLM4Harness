#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

size_t nondet_size_t(void);

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_allocator_is_valid(allocator));

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_src = src;

    size_t byte_index = nondet_size_t();
    uint8_t old_src_byte = 0;
    if (src.len > 0) {
        __CPROVER_assume(byte_index < src.len);
        old_src_byte = src.buffer[byte_index];
    }

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(src.len == old_src.len);
    assert(src.buffer == old_src.buffer);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (old_src.len > 0) {
        assert(src.buffer[byte_index] == old_src_byte);
    }
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_src.len);
        assert(dest.capacity == old_src.len);

        if (old_src.len == 0) {
            assert(dest.buffer == NULL);
            assert(dest.capacity == 0);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.buffer != old_src.buffer);
            assert(dest.allocator == allocator);
            assert(dest.buffer[byte_index] == old_src_byte);
        }

        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(aws_byte_buf_is_valid(&dest));
    }
}
