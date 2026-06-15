/* Contract for aws_byte_buf_clean_up_secure
 *
 * Preconditions:
 *   - buf is a non‑null pointer.
 *   - aws_byte_buf_is_valid(buf) holds before the call.
 *   - buf->allocator may be NULL or a valid allocator (default allocator used in harness).
 *
 * Postconditions (validity):
 *   - After the call, buf remains a valid aws_byte_buf structure.
 *
 * Postconditions (length & capacity):
 *   - buf->len == 0
 *   - buf->capacity == 0
 *
 * Postconditions (buffer pointer):
 *   - buf->buffer == NULL
 *
 * Postconditions (allocator):
 *   - buf->allocator is unchanged (still the same pointer as before the call).
 *
 * Frame conditions:
 *   - No memory outside of the buffer allocated for buf->buffer is modified.
 *   - The allocator object (if non‑NULL) is not modified.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic size_t generator for CBMC */
size_t nondet_size_t(void);
uint8_t *nondet_uint8_ptr(void);

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Allocate a nondeterministic capacity for the buffer */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= SIZE_MAX / 2); /* avoid overflow in internal calculations */

    /* Initialize the buffer with the default allocator */
    int init_result = aws_byte_buf_init(&buf, alloc, capacity);
    __CPROVER_assume(init_result == 0); /* assume successful allocation */

    /* Set a nondeterministic length that does not exceed capacity */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Ensure the buffer is valid before invoking the function under test */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Preserve the original allocator pointer for post‑condition checking */
    struct aws_allocator *old_allocator = buf.allocator;

    /* Call the function under verification */
    aws_byte_buf_clean_up_secure(&buf);

    /* Verify post‑conditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == old_allocator);
    assert(aws_byte_buf_is_valid(&buf));

    return 0;
}
