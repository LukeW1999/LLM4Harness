#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness() {
    /* Input: non-deterministic array and length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    uint8_t *bytes;
    if (len == 0) {
        bytes = NULL;
    } else {
        bytes = (uint8_t *)malloc(len);
        __CPROVER_assume(bytes != NULL);
        /* Ensure the memory is readable for the specified length */
        __CPROVER_assume(__CPROVER_r_ok(bytes, len));
    }

    /* Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_array(bytes, len);

    /* Postcondition: returned cursor exactly matches the inputs */
    assert(cursor.ptr == bytes);
    assert(cursor.len == len);

    /* Invariant: the newly created cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cursor));
}
