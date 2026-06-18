#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* Set up inputs */
    size_t len;
    void *array;

    /* Nondet array pointer - can be NULL or non-NULL */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate array if len > 0 */
    if (len > 0) {
        array = malloc(len);
        __CPROVER_assume(array != NULL);
    } else {
        array = NULL;
    }

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, len);

    /* Assert postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    /* When capacity > 0, buffer must point to the provided array */
    if (len > 0) {
        assert(buf.buffer == (uint8_t *)array);
    } else {
        /* When capacity == 0, buffer must be NULL regardless of input */
        assert(buf.buffer == NULL);
    }

    /* Explicit check: buffer is non-NULL iff capacity > 0 */
    assert((buf.buffer == NULL) == (len == 0));

    /* Explicit check: buffer equals array pointer only when capacity > 0 */
    assert(len > 0 ? buf.buffer == (uint8_t *)array : buf.buffer == NULL);
}
