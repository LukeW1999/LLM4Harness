#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_harness(void) {
    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Byte buffer to be cleaned up */
    struct aws_byte_buf buf;

    /* Nondeterministically decide whether the buffer has allocated memory */
    if (nondet_bool()) {
        /* Allocate a backing buffer and set fields */
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(buf.len <= buf.capacity);
        buf.allocator = allocator;
    } else {
        /* No backing buffer */
        buf.buffer = NULL;
        buf.capacity = 0;
        buf.len = 0;
        buf.allocator = allocator;
    }

    /* Call the function under test */
    aws_byte_buf_clean_up(&buf);

    /* Post‑condition checks */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
