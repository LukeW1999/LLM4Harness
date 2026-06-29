#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* Bound capacity to keep CBMC tractable */
    size_t capacity;
    __CPROVER_assume(capacity <= 10);

    /* Allocate a writable array of the given capacity */
    uint8_t *bytes = NULL;
    if (capacity > 0) {
        bytes = malloc(capacity);
        __CPROVER_assume(bytes != NULL);
    } else {
        /* capacity == 0: bytes can be anything (even NULL), but the
           implementation only uses it when capacity > 0.
           Use a non-NULL pointer to satisfy AWS_MEM_IS_WRITABLE(bytes, 0). */
        uint8_t dummy;
        bytes = &dummy;
    }

    /* Precondition: bytes must be writable up to capacity bytes */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(bytes, capacity));

    /* Call the function under verification */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(bytes, capacity);

    /* Postcondition: result must be a valid aws_byte_buf */
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition: allocator is NULL (no allocator used) */
    assert(buf.allocator == NULL);

    /* Postcondition: len is 0 (empty array) */
    assert(buf.len == 0);

    /* Postcondition: capacity matches input */
    assert(buf.capacity == capacity);

    /* Postcondition: buffer pointer is set correctly */
    if (capacity > 0) {
        assert(buf.buffer == (uint8_t *)bytes);
    } else {
        assert(buf.buffer == NULL);
    }

    /* Postcondition: len <= capacity */
    assert(buf.len <= buf.capacity);
}
