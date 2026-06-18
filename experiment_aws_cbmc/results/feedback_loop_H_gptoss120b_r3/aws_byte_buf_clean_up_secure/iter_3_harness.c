#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;

    /* Use the default allocator */
    buf.allocator = aws_default_allocator();

    /* Nondeterministically choose a capacity within the allowed bound */
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);

    /* Allocate the buffer if capacity is non‑zero */
    if (buf.capacity > 0) {
        buf.buffer = (uint8_t *)aws_mem_acquire(buf.allocator, buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    /* Nondeterministically choose a length that does not exceed capacity */
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Ensure the byte buffer satisfies its validity invariant */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save the original state */
    struct aws_byte_buf old = buf;

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Post‑condition checks */
    assert(buf.allocator == NULL);          /* allocator is cleared by clean_up_secure */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
