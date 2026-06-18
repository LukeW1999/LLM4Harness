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
    struct aws_allocator *allocator = aws_default_allocator();
    
    struct aws_byte_buf dest;
    
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer old_src_bytes;
    if (src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &old_src_bytes);
    }

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);
    
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.len > 0) {
            assert_byte_from_buffer_matches(dest.buffer, &old_src_bytes);
        }
    } else {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
    
    assert(aws_byte_buf_is_valid(&src));
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
}
