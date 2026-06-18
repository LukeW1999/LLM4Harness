#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_allocator *allocator = aws_default_allocator();

    /* Save old state for immutability checks */
    struct aws_byte_buf old_src = src;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);
        if (src.buffer != NULL) {
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.buffer == NULL);
            assert(dest.capacity == 0);
            assert(dest.len == 0);
        }
    } else {
        /* On failure, dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.capacity == 0);
        assert(dest.len == 0);
        assert(dest.allocator == NULL);
    }

    /* src must remain unchanged */
    assert(src.allocator == old_src.allocator);
    assert(src.capacity == old_src.capacity);
    assert(src.len == old_src.len);
    assert(src.buffer == old_src.buffer);
    assert(aws_byte_buf_is_valid(&src));
}

void aws_byte_buf_secure_zero_harness(void) {
    aws_byte_buf_init_copy_harness();
    return 0;
}
