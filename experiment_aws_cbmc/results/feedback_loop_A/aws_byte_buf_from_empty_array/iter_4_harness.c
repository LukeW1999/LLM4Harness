#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf byte_buf;
    uint8_t array[1]; // Small array to use as buffer
    size_t array_size = sizeof(array);

    /* 2. Initialize source buffer */
    __CPROVER_assume(array_size <= MAX_BUFFER_SIZE);
    struct aws_byte_buf src = aws_byte_buf_from_empty_array(array, array_size);

    /* 3. Call function under test */
    aws_byte_buf_from_empty_array(&byte_buf, array);

    /* 4. Assert postconditions */
    assert(byte_buf.buffer == array);
    assert(byte_buf.len == 0);
    assert(byte_buf.capacity == array_size);
    assert(aws_byte_buf_is_valid(&byte_buf));
}
