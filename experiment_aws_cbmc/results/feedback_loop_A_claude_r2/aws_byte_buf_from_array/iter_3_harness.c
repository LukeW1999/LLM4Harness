#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness(void) {
    /* 1. Set up nondeterministic array and length */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    uint8_t *array;
    if (len > 0) {
        array = malloc(len);
        __CPROVER_assume(array != NULL);
    } else {
        array = NULL;
    }

    /* 2. Set up dest buffer (stack-allocated output) */
    struct aws_byte_buf buf;

    /* 3. Call function under test */
    aws_byte_buf_from_array(&buf, array, len);

    /* 4. Assert postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    if (len == 0) {
        assert(buf.buffer == NULL || buf.buffer == (uint8_t *)array);
    } else {
        assert(buf.buffer == (uint8_t *)array);
    }
}
