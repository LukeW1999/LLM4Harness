#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness(void) {
    size_t capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (capacity > 0) {
        array = __CPROVER_allocate(capacity, 0);
        __CPROVER_assume(array != NULL);
    }

    struct aws_byte_buf buf;
    aws_byte_buf_from_empty_array(&buf, array, capacity);

    __CPROVER_assert(buf.buffer == array, "buf.buffer should equal array");
    __CPROVER_assert(buf.len == 0, "buf.len should be zero");
    __CPROVER_assert(buf.capacity == capacity, "buf.capacity should match capacity");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator should be NULL");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf should be valid");
}
