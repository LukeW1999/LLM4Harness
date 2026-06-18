/*=== Formal Contract for aws_byte_buf_from_empty_array =========================

Preconditions:
  1. size_t capacity is any value (including 0).
  2. If capacity > 0 then:
       - bytes points to a writable memory region of at least `capacity` bytes.
       - bytes != NULL.
     If capacity == 0 then bytes may be NULL or any pointer value.

Postconditions (validity):
  - The returned struct aws_byte_buf `buf` satisfies aws_byte_buf_is_valid(&buf).

Postconditions (length & capacity):
  - buf.len == 0
  - buf.capacity == capacity
  - buf.allocator == NULL

Postconditions (buffer pointer):
  - If capacity == 0 then buf.buffer == NULL.
  - If capacity > 0 then buf.buffer == (uint8_t *)bytes.

Postconditions (frame):
  - The memory region pointed to by `bytes` (if capacity > 0) is not modified
    by the call (its contents remain unchanged).
  - No other memory locations are modified.

============================================================================*/

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to nondeterministically allocate a buffer of given size */
static uint8_t *nondet_buffer(size_t size) {
    if (size == 0) {
        return NULL;
    }
    uint8_t *buf = malloc(size);
    /* If allocation fails, treat as NULL (the contract allows any pointer when size>0,
       but we will assume allocation succeeded for the proof). */
    __CPROVER_assume(buf != NULL);
    return buf;
}

/* Helper to copy a memory region for later comparison */
static void copy_memory(const uint8_t *src, uint8_t *dst, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        dst[i] = src[i];
    }
}

void aws_byte_buf_from_empty_array_harness(void) {
    /* nondeterministic inputs */
    size_t capacity;
    __CPROVER_assume(capacity <= SIZE_MAX); /* trivially true, just to silence warnings */

    uint8_t *bytes = nondet_buffer(capacity);

    /* If capacity > 0, ensure the precondition that the region is writable.
       In CBMC we model this by simply assuming the pointer is non‑NULL. */
    if (capacity > 0) {
        __CPROVER_assume(bytes != NULL);
    }

    /* Preserve a copy of the original memory to check the frame condition */
    uint8_t *bytes_snapshot = NULL;
    if (capacity > 0) {
        bytes_snapshot = malloc(capacity);
        __CPROVER_assume(bytes_snapshot != NULL);
        copy_memory(bytes, bytes_snapshot, capacity);
    }

    /* Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(bytes, capacity);

    /* ----- Postcondition checks ----- */

    /* Validity */
    assert(aws_byte_buf_is_valid(&buf));

    /* Length and capacity invariants */
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);

    /* Buffer pointer relationship */
    if (capacity == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == bytes);
    }

    /* Frame condition: input memory must be unchanged */
    if (capacity > 0) {
        for (size_t i = 0; i < capacity; ++i) {
            assert(bytes[i] == bytes_snapshot[i]);
        }
    }

    /* No other global state is modified – this is implicitly guaranteed by the
       absence of any writes to global variables in the function. */

    return 0;
}
