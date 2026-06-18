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
    /* Declare inputs */
    const void *bytes;
    size_t len;

    /* Non-deterministic inputs */
    bytes = nondet_void_ptr();
    len = nondet_size_t();

    /* Preconditions: if len > 0, bytes must be readable */
    __CPROVER_assume(len == 0 || AWS_MEM_IS_READABLE(bytes, len));

    /* Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* Postconditions */
    /* 1. Changed fields: ptr and len are set from inputs */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* 2. Unchanged fields: none (struct is created fresh, no old state) */

    /* 3. Both return paths: function returns struct by value, no error path */

    /* 4. Validity invariants */
    assert(aws_byte_cursor_is_valid(&cur));
}
