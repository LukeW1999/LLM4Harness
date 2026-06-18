#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound input structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest; /* output, will be initialized by the function */

    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Save old state of src (immutable input) */
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect a successful copy */
        assert(dest.allocator == allocator);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);
        if (src.capacity > 0) {
            assert(dest.buffer != NULL);
        } else {
            assert(dest.buffer == NULL);
        }
        if (src.len > 0) {
            for (size_t i = 0; i < src.len; ++i) {
                __CPROVER_assert(dest.buffer[i] == src.buffer[i],
                                 "copied byte matches source");
            }
        }

        /* src must be unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);

        /* validity invariants */
        assert(aws_byte_buf_is_valid(&dest));
        assert(aws_byte_buf_is_valid(&src));
    } else {
        /* on failure dest must be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* src must be unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);

        /* validity invariants */
        assert(aws_byte_buf_is_valid(&dest));
        assert(aws_byte_buf_is_valid(&src));
    }
}
