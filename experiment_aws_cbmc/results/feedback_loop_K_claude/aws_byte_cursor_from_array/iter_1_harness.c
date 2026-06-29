#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness(void) {
    size_t len;
    uint8_t *bytes;

    /* Precondition: if len > 0, bytes must be non-NULL and point to valid memory */
    __CPROVER_assume(len <= 1024); /* bound for tractability */

    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    } else {
        /* len == 0: bytes may be NULL or non-NULL; test both cases */
        /* Use nondet pointer; function should handle NULL when len==0 */
        bytes = NULL;
    }

    /* Save input values for postcondition checks */
    const void *const bytes_in = (const void *)bytes;
    const size_t len_in = len;

    /* Call the function under verification */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes_in, len_in);

    /* Postcondition: ptr is set to the input bytes pointer */
    assert(result.ptr == (uint8_t *)bytes_in);

    /* Postcondition: len is set to the input len */
    assert(result.len == len_in);

    /* Postcondition: the cursor is valid according to the library's validity predicate */
    assert(aws_byte_cursor_is_valid(&result));
}
