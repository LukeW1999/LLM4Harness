#include <aws/common/byte_buf.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_array_harness(void) {
    /* nondeterministic length, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate buffer if length is non‑zero */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* postconditions: returned cursor reflects the inputs */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* validity invariant must hold for the returned cursor */
    assert(aws_byte_cursor_is_valid(&cur));
}
