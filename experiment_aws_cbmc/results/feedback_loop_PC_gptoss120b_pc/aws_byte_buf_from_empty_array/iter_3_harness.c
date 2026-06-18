#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness(void) {
    size_t capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (capacity > 0) {
        array = (uint8_t *)malloc(capacity);
        __CPROVER_assume(array != NULL);
    }

    struct aws_byte_buf buf;
    aws_byte_buf_from_empty_array(&buf, array, capacity);

    assert(buf.buffer == array);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
