#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Declare inputs */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t *bytes;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    } else {
        /* NULL may be passed if length is 0 */
        bytes = nondet_bool() ? NULL : malloc(1);
    }

    /* 2. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* 3. Assert postconditions */
    /* Changed fields: ptr and len are set from inputs */
    assert(result.ptr == (uint8_t *)bytes);
    assert(result.len == len);

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
