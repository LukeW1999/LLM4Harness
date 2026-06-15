#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness() {
    /* Non-deterministic inputs */
    size_t len;
    uint8_t *array;

    /* Assumptions to avoid overflow and ensure memory safety */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(array != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(array, len));

    /* Call the function under verification */
    struct aws_byte_buf result = aws_byte_buf_from_array(array, len);

    /* Postcondition: The returned buffer has the same pointer and length */
    assert(result.buffer == array);
    assert(result.len == len);
    assert(result.capacity == len);
    assert(result.allocator == NULL);

    /* The buffer content is readable */
    assert(AWS_MEM_IS_READABLE(result.buffer, result.len));

    /* The buffer is always valid */
    assert(aws_byte_buf_is_valid(&result));
}
