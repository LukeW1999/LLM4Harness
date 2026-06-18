#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness(void) {
    /* 1. Set up a nondet array and length */
    size_t len;
    uint8_t *array;
    __CPROVER_assume(len == 0 || array != NULL);

    /* 2. Destination buffer on stack */
    struct aws_byte_buf buf;

    /* 3. Call function under test */
    aws_byte_buf_from_array(&buf, (const void *)array, len);

    /* 4. Assert postconditions */
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf is valid");
    __CPROVER_assert(buf.len == len, "buf.len == len");
    __CPROVER_assert(buf.capacity == len, "buf.capacity == len");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator == NULL");
}
