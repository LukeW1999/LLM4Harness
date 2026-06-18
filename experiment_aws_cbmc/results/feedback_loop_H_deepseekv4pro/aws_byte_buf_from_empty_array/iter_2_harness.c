#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_byte_buf_from_empty_array_harness() {
    size_t len;
    uint8_t *array;

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    array = malloc(len);
    __CPROVER_assume(array != NULL);

    uint8_t *old_array = malloc(len);
    __CPROVER_assume(old_array != NULL);
    for (size_t i = 0; i < len; i++) {
        old_array[i] = array[i];
    }

    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, len);

    assert(buf.buffer == array);
    assert(buf.len == 0);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    for (size_t i = 0; i < len; i++) {
        assert(array[i] == old_array[i]);
    }

    free(array);
    free(old_array);
}
