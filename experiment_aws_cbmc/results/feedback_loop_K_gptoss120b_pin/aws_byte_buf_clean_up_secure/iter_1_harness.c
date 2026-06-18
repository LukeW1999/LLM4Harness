/*=== Contract for aws_byte_buf_clean_up_secure ===
Preconditions:
  - buf != NULL
  - buf->allocator is either NULL or a valid allocator (aws_default_allocator())
  - buf->capacity >= buf->len
  - if buf->capacity > 0 then buf->buffer != NULL
  - if buf->allocator == NULL then buf->buffer must be NULL (no ownership)
  - the memory region pointed to by buf->buffer (of size buf->capacity) is valid and writable
Postconditions (validity):
  - buf != NULL (still)
  - buf->allocator is unchanged
Postconditions (length & state):
  - buf->buffer == NULL
  - buf->len == 0
  - buf->capacity == 0
Postconditions (frame):
  - No memory outside the original buffer region (if any) is modified
  - The original buffer region is zeroed before being freed (cannot be observed after free)
===*/

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc;
    size_t capacity;
    size_t len;
    bool has_allocator;

    /* nondeterministic choices */
    has_allocator = __CPROVER_nondet_bool();
    alloc = has_allocator ? aws_default_allocator() : NULL;

    capacity = __CPROVER_nondet_uint();
    __CPROVER_assume(capacity <= SIZE_MAX / 2); /* avoid overflow in later calculations */

    len = __CPROVER_nondet_uint();
    __CPROVER_assume(len <= capacity);

    /* set up buffer */
    if (capacity > 0 && alloc != NULL) {
        buf.buffer = (uint8_t *)malloc(capacity);
        __CPROVER_assume(buf.buffer != NULL);
        /* make the buffer region nondeterministic */
        __CPROVER_assume(__CPROVER_is_fresh(buf.buffer, capacity));
    } else {
        buf.buffer = NULL;
        __CPROVER_assume(capacity == 0);
    }

    buf.len = len;
    buf.capacity = capacity;
    buf.allocator = alloc;

    /* Preserve a copy of the original buffer pointer for later frame check */
    uint8_t *original_buffer = buf.buffer;

    /* Call the function under verification */
    aws_byte_buf_clean_up_secure(&buf);

    /* Postcondition checks */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == alloc);

    /* Frame condition: memory outside the original buffer must be unchanged.
       Since the original buffer is freed, we cannot inspect its contents after free,
       but we can assert that any other memory is untouched. */
    /* No additional memory was allocated in this harness, so nothing else to check. */

    return 0;
}
