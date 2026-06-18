#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>

#define MAX_CAPACITY 1024

uint64_t nondet_uint64_t(void);

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    uint8_t arr[MAX_CAPACITY];
    uint8_t *array = arr;
    size_t capacity = (size_t)nondet_uint64_t();

    __CPROVER_assume(capacity <= MAX_CAPACITY);

    aws_byte_buf_from_empty_array(&buf, array, capacity);

    assert(buf.buffer == array);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
}
