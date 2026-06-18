#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_array_harness(void) {
    /* 1. Set up nondet array and length */
    size_t len;
    uint8_t *array = malloc(len);

    /* 2. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(array, len);

    /* 3. Assert postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    if (len == 0) {
        /* buffer may be null or non-null when len is 0 */
    } else {
        assert(buf.buffer == (uint8_t *)array);
    }
}
