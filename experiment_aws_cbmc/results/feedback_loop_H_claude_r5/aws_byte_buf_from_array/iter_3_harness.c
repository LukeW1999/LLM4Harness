#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_from_array_harness(void) {
    /* 1. Set up a nondet array and length */
    size_t len;
    uint8_t *array;

    /* Bound the length to keep verification tractable */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* array can be NULL or non-NULL; if len > 0, array must be non-NULL */
    if (len > 0) {
        array = malloc(len);
        __CPROVER_assume(array != NULL);
    } else {
        /* len == 0: array is NULL */
        array = NULL;
    }

    /* 2. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(array, len);

    /* 3. Assert postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.buffer == (uint8_t *)array);
    assert(buf.allocator == NULL);
}
