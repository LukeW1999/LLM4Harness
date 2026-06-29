#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_array_harness(void) {
    size_t len;
    __CPROVER_assume(len <= 8); /* bound for tractability */

    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* Precondition: bytes must be writable up to len bytes */
    __CPROVER_assume(len == 0 || __CPROVER_w_ok(bytes, len));

    struct aws_byte_buf result = aws_byte_buf_from_array(bytes, len);

    /* Postcondition: result must be a valid aws_byte_buf */
    assert(aws_byte_buf_is_valid(&result));

    /* Postcondition: allocator must be NULL (no allocator used) */
    assert(result.allocator == NULL);

    /* Postcondition: len and capacity must equal the input len */
    assert(result.len == len);
    assert(result.capacity == len);

    /* Postcondition: buffer pointer correctness */
    if (len == 0) {
        assert(result.buffer == NULL);
    } else {
        assert(result.buffer == (uint8_t *)bytes);
    }
}
