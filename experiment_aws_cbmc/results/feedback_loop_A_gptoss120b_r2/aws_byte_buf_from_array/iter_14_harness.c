#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_from_array_harness(void) {
    struct aws_byte_buf buf;

    uint8_t src_array[MAX_BUFFER_SIZE];
    uint8_t *src = src_array;

    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    aws_byte_buf_from_array(&buf, src, len);

    __CPROVER_assert(buf.buffer == src, "buf.buffer should point to src");
    __CPROVER_assert(buf.len == len, "buf.len should be set to len");
    __CPROVER_assert(buf.capacity == len, "buf.capacity should be set to len");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator should be NULL");

    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf should remain valid");
}
