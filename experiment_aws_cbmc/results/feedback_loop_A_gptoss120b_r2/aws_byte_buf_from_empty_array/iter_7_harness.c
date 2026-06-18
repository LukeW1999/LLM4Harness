#define MAX_BUFFER_SIZE 256

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;

    uint8_t backing[MAX_BUFFER_SIZE];
    uint8_t *array = NULL;
    if (nondet_bool()) {
        array = backing;
    }

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    if (array == backing) {
        __CPROVER_assume(capacity <= sizeof(backing));
    }

    aws_byte_buf_from_empty_array(&buf, array, capacity);

    assert(buf.buffer == array);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
