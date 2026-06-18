#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness(void) {
    /* 1. Set up a nondet array pointer and length */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t *array = bounded_malloc(len);

    /* 2. Set up dest buffer (uninitialized output) */
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
