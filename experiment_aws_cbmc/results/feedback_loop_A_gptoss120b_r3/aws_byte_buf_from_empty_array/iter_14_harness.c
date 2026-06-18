#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>

#define MAX_CAPACITY 1024

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    uint8_t arr[MAX_CAPACITY];
    uint8_t *array = arr;
    size_t capacity = nondet_uint64_t();

    __CPROVER_assume(capacity <= MAX_CAPACITY);

    aws_byte_buf_from_empty_array(&buf, array, capacity);

    __CPROVER_assert(buf.buffer == array, "buffer points to array");
    __CPROVER_assert(buf.len == 0, "len is zero");
    __CPROVER_assert(buf.capacity == capacity, "capacity matches");
    __CPROVER_assert(buf.allocator == NULL, "allocator is NULL");
}
