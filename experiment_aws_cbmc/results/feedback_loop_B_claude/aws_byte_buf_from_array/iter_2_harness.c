#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness(void) {
    /* 1. Declare a nondet array and length */
    size_t len;
    uint8_t *array;

    /* Bound the length for tractability */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* If len > 0, array must be non-NULL */
    if (len > 0) {
        array = malloc(len);
        __CPROVER_assume(array != NULL);
    } else {
        /* len == 0: array can be NULL or non-NULL */
        /* leave array nondet */
    }

    /* 2. Declare dest buffer */
    struct aws_byte_buf buf;

    /* 3. Call function under test */
    aws_byte_buf_from_array(&buf, array, len);

    /* 4. Assert postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    if (len > 0) {
        assert(buf.buffer == array);
    }
}
