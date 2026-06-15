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
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Non‑deterministically decide whether the buffer is NULL */
    if (nondet_bool()) {
        /* Make it a null buffer */
        buf.buffer = NULL;
        buf.capacity = 0;
        buf.len = 0;
    }

    /* 3. Save old state for later comparison */
    struct aws_byte_buf old_buf = buf;

    /* 4. Call the function under test */
    aws_byte_buf_reset(&buf);

    /* 5. Post‑condition checks */
    /* After reset the buffer must be empty and have no allocator */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* 6. The original buffer's fields (except allocator) must not have been altered
       before they were cleared – we only check that the allocator pointer was
       either unchanged (if it was NULL) or was the default allocator. */
    if (old_buf.buffer != NULL) {
        /* The allocator used to free the memory must be the one stored in the
           original buffer. */
        assert(old_buf.allocator == aws_default_allocator());
    }

    /* 7. Invariant must hold for the resulting buffer */
    assert(aws_byte_buf_is_valid(&buf));
}
