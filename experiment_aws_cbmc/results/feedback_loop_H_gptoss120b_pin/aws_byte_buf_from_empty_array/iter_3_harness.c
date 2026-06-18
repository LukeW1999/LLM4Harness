#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    uint8_t static_array[MAX_BUFFER_SIZE];
    size_t capacity;

    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(!(capacity > 0 && static_array == NULL));
    __CPROVER_assume(!(static_array == NULL && capacity != 0));

    uint8_t *array = (capacity > 0) ? static_array : NULL;

    aws_byte_buf_from_empty_array(&buf, array, capacity);

    assert(buf.buffer == array);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
