#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness(void) {
    /* Set up a nondet array and length */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t *array = len > 0 ? malloc(len) : NULL;
    if (len > 0) {
        __CPROVER_assume(array != NULL);
    }

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(array, len);

    /* Assert postconditions */
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    /* Strictly check buffer pointer assignment:
       - When len > 0, buffer must be exactly the passed pointer (catches mutant_0016 where len>=0 changes behavior for len==0)
       - When len == 0, buffer must be NULL (catches mutant_0016 where len>=0 would set buffer to array instead of NULL)
    */
    if (len > 0) {
        assert(buf.buffer == (uint8_t *)array);
    } else {
        assert(buf.buffer == NULL);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
