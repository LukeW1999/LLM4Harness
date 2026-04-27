#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* 1. Set up a non-deterministic array pointer and length */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    
    uint8_t *array = len > 0 ? malloc(len) : NULL;
    __CPROVER_assume(len == 0 || array != NULL);

    /* 2. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, len);

    /* 3. Assert postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == len);
    assert(buf.buffer == (uint8_t *)array);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
