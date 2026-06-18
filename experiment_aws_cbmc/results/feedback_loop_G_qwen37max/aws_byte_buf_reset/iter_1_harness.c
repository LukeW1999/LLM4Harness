#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_src = src;
    uint8_t *src_buffer_ptr = src.buffer;
    size_t src_len = src.len;
    size_t src_cap = src.capacity;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Assert src is unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src_len);
        assert(dest.capacity == src_cap);
        
        if (src_buffer_ptr != NULL && src_len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src_buffer_ptr, src_len);
        } else if (src_buffer_ptr == NULL) {
            assert(dest.buffer == NULL);
        }
    } else {
        assert(result == AWS_OP_ERR);
        /* On failure, the implementation zeroes out the dest struct */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
