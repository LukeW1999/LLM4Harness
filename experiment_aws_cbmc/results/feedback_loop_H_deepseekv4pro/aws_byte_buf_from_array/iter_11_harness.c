#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1000
#endif

void aws_byte_buf_from_array_harness() {
    uint8_t array[MAX_BUFFER_SIZE];
    size_t len;

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    struct aws_byte_buf buf = aws_byte_buf_from_array(array, len);

    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.buffer == array);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
}
