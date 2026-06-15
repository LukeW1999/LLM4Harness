#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness(void) {
    /* 1. Declare the byte buffer */
    struct aws_byte_buf buf;

    /* 2. Non‑deterministically choose an allocator (NULL or default) */
    if (nondet_bool()) {
        buf.allocator = NULL;
    } else {
        buf.allocator = aws_default_allocator();
    }

    /* 3. Impose a bound on the buffer and allocate its internal storage if needed */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 4. Optionally turn the buffer into a null buffer */
    if (nondet_bool()) {
        buf.buffer = NULL;
        buf.capacity = 0;
        buf.len = 0;
    }

    /* 5. Save the original state for later checks */
    struct aws_byte_buf old_buf = buf;

    /* 6. Call the function under test */
    aws_byte_buf_reset(&buf);

    /* 7. Post‑condition checks */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* 8. If the original buffer held memory, its allocator must have been the default */
    if (old_buf.buffer != NULL) {
        assert(old_buf.allocator == aws_default_allocator());
    }

    /* 9. The resulting buffer must satisfy its invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
