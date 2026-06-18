#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness(void) {
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest; /* uninitialized */

    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf old_src = src;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert_byte_buf_eq(&dest, &src);
        }
        assert_byte_buf_eq(&src, &old_src);
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert_byte_buf_eq(&src, &old_src);
    }

    assert(aws_byte_buf_is_valid(&src));
}
