#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness() {
    size_t len;
    uint8_t *array;

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(array != NULL);
    __CPROVER_assume(__CPROVER_r_ok(array, len));

    struct aws_byte_buf result = aws_byte_buf_from_array(array, len);

    assert(result.buffer == array);
    assert(result.len == len);
    assert(result.capacity == len);
    assert(result.allocator == NULL);
    assert(aws_byte_buf_is_valid(&result));
}
