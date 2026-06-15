#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;

    /* nondet capacity bounded */
    buf.capacity = nondet_uint();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);

    /* nondet length bounded by capacity */
    buf.len = nondet_uint();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* nondet allocator: either NULL or the default allocator */
    struct aws_allocator *alloc = nondet_pointer();
    __CPROVER_assume(alloc == NULL || alloc == aws_default_allocator());
    buf.allocator = alloc;

    /* allocate buffer if capacity > 0 */
    if (buf.capacity > 0) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    /* ensure the byte buffer is initially valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state */
    struct aws_byte_buf old = buf;

    /* call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* post‑condition checks */
    assert(buf.len == 0);
    assert(buf.allocator == old.allocator);

    if (old.allocator != NULL) {
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
    }

    /* validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));
}
