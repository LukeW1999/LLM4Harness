#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf;

    /* Ensure the buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    old_buf = buf;

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Postconditions: */
    /* 1. Changed fields: len is zeroed, buffer is freed */
    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 2. Unchanged fields: none (all fields are modified) */

    /* 3. Both return paths: no return value, so no success/failure distinction */

    /* 4. Validity invariants: after cleanup, the buffer should be valid (empty) */
    assert(aws_byte_buf_is_valid(&buf));
}
