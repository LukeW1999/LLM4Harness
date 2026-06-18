#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1000
#endif

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    assert(allocator != NULL);

    struct aws_byte_buf dest;
    struct aws_byte_cursor src;

    /* Bound the cursor length and ensure validity */
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct aws_byte_cursor old_src = src;

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));

    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert(memcmp(dest.buffer, src.ptr, src.len) == 0);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
