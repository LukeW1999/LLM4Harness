#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t *array;
    __CPROVER_assume(array != NULL);
    __CPROVER_assume(__CPROVER_is_fresh(array, len));

    /* Preserve the original array contents for later comparison */
    uint8_t *old_array = array;
    size_t i;
    for (i = 0; i < len; ++i) {
        old_array[i] = array[i];
    }

    aws_byte_buf_from_array(&buf, array, len);

    assert(buf.buffer == array);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    if (len > 0) {
        for (i = 0; i < len; ++i) {
            assert(buf.buffer[i] == old_array[i]);
        }
    }

    assert(aws_byte_buf_is_valid(&buf));
}
