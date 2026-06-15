#include <assert.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_clean_up_harness(void) {
    /* allocator – nondeterministic but non‑NULL */
    struct aws_allocator *alloc;
    __CPROVER_assume(alloc != NULL);

    /* byte buffer – may or may not have an allocated buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* force a non‑NULL allocator in the buffer */
    buf.allocator = alloc;
    __CPROVER_assume(buf.allocator != NULL);

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Remember the original state for later checks */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_byte_buf_clean_up(&buf);

    /* Post‑conditions: the buffer must be cleared */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));

    /* The original allocator (if any) must not be stored in the cleared buffer */
    assert(buf.allocator != old_buf.allocator);
}
