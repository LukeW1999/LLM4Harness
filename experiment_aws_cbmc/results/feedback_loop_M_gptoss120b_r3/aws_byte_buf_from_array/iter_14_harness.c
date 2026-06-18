#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf buf;

    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (len > 0) {
        array = (uint8_t *)malloc(len);
        __CPROVER_assume(__CPROVER_is_fresh(array, len));
    }

    aws_byte_buf_from_array(&buf, array, len);

    assert(buf.buffer == array);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
