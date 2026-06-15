#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf *buf = ensure_byte_buf_is_bounded_and_allocated(MAX_BUFFER_SIZE);
    struct aws_allocator *allocator = aws_default_allocator();
    const uint8_t *array = buf->buffer;
    size_t len = buf->len;

    struct aws_byte_buf dest;
    int result = aws_byte_buf_from_array(&dest, allocator, array, len);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == len);
        assert(dest.capacity >= len);
        assert(dest.allocator == allocator);
        assert_bytes_match(dest.buffer, array, len);
    } else {
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
    }

    assert(aws_byte_buf_is_valid(&dest));
}
