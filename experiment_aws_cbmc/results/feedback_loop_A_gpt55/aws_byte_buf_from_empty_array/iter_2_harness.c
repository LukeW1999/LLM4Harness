#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness(void) {
    size_t capacity;
    uint8_t array[MAX_BUFFER_SIZE];

    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, capacity);

    __CPROVER_assert(buf.buffer == array, "buffer is set to the input array");
    __CPROVER_assert(buf.len == 0, "len is zero");
    __CPROVER_assert(buf.capacity == capacity, "capacity is set to the input capacity");
    __CPROVER_assert(buf.allocator == NULL, "allocator is NULL");
    __CPROVER_assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE), "byte buf is bounded");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "byte buf is valid");

    struct aws_byte_buf null_buf = aws_byte_buf_from_empty_array(NULL, 0);

    __CPROVER_assert(null_buf.buffer == NULL, "NULL buffer is preserved");
    __CPROVER_assert(null_buf.len == 0, "NULL buffer len is zero");
    __CPROVER_assert(null_buf.capacity == 0, "NULL buffer capacity is zero");
    __CPROVER_assert(null_buf.allocator == NULL, "NULL buffer allocator is NULL");
    __CPROVER_assert(aws_byte_buf_is_bounded(&null_buf, MAX_BUFFER_SIZE), "NULL byte buf is bounded");
    __CPROVER_assert(aws_byte_buf_is_valid(&null_buf), "NULL byte buf is valid");
}
