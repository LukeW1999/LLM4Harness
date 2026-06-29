#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_init_copy_harness(void) {
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Ground-truth preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save src state before call */
    size_t old_src_len = src.len;
    size_t old_src_capacity = src.capacity;
    uint8_t *old_src_buffer = src.buffer;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postcondition 1: Return value correctness */
    if (result == AWS_OP_SUCCESS) {
        /* Postcondition 2: Output buffer validity */
        assert(aws_byte_buf_is_valid(&dest));

        /* Postcondition 3: Allocator is set correctly */
        assert(dest.allocator == allocator);

        if (old_src_buffer == NULL) {
            /* If src had no buffer, dest should have no buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest buffer should not be NULL */
            assert(dest.buffer != NULL);

            /* dest len and capacity should match src */
            assert(dest.len == old_src_len);
            assert(dest.capacity == old_src_capacity);

            /* dest buffer should be a different allocation than src buffer */
            assert(dest.buffer != old_src_buffer);
        }
    } else {
        /* On failure, dest should be zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* Frame condition: src should not be modified */
    assert(src.len == old_src_len);
    assert(src.capacity == old_src_capacity);
    assert(src.buffer == old_src_buffer);
}
