#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_array_harness(void) {
    /* 1. Set up a nondet array and length */
    size_t len;
    uint8_t *array = nondet_voidp();

    /* 2. Destination buffer on stack */
    struct aws_byte_buf buf;

    /* 3. Call function under test */
    aws_byte_buf_from_array(&buf, array, len);

    /* 4. Assert postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.buffer == (len == 0 ? NULL : array));
    assert(buf.allocator == NULL);
}
