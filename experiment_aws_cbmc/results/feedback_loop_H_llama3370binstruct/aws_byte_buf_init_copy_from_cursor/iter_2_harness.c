#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_cursor src = ensure_byte_cursor_is_bounded_and_valid(MAX_BUFFER_SIZE);

    aws_byte_buf_init(&dest, allocator, 0);

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity >= src.len);
        assert(dest.allocator == allocator);
        assert(memcmp(dest.buffer, src.ptr, src.len) == 0);
    } else {
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
    }

    assert(dest.allocator == allocator || dest.allocator == NULL);

    assert(aws_byte_buf_is_valid(&dest));
}
