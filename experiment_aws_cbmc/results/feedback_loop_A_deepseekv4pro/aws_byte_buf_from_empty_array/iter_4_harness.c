#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness() {
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    uint8_t *array = malloc(len);

    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, len);

    if (array != NULL) {
        assert(buf.buffer == array);
        assert(buf.capacity == len);
    } else {
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
    }
    assert(buf.len == 0);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
