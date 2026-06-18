#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness(void) {
    /* allocator is not used by aws_byte_buf_secure_zero, but we set it to a valid value */
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic capacity, bounded to keep the proof tractable */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 64);               /* reasonable bound */
    __CPROVER_assume(capacity > 0);                 /* non‑zero to test zeroing */

    /* allocate a buffer with guard bytes before and after the actual region */
    uint8_t *raw = (uint8_t *)malloc(capacity + 2);
    __CPROVER_assume(raw != NULL);                 /* assume allocation succeeds */

    /* set guard bytes */
    raw[0] = 0xAA;
    raw[capacity + 1] = 0xBB;

    /* point buf.buffer to the interior region */
    struct aws_byte_buf buf;
    buf.buffer   = raw + 1;
    buf.capacity = capacity;

    /* nondeterministic length, must be <= capacity */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* fill the buffer with nondeterministic data */
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf.buffer[i] = nondet_uint8_t();
    }

    /* set allocator field (not used by secure_zero) */
    buf.allocator = alloc;

    /* pre‑condition: the buffer must be valid */
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "precondition: aws_byte_buf_is_valid");

    /* call the function under test */
    aws_byte_buf_secure_zero(&buf);

    /* post‑condition: buffer remains valid */
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "postcondition: aws_byte_buf_is_valid after secure_zero");

    /* post‑condition: length reset to zero, capacity unchanged */
    __CPROVER_assert(buf.len == 0, "postcondition: len reset to zero");
    __CPROVER_assert(buf.capacity == capacity, "postcondition: capacity unchanged");

    /* post‑condition: memory zeroed only within the buffer */
    for (size_t i = 0; i < buf.capacity; ++i) {
        __CPROVER_assert(buf.buffer[i] == 0, "postcondition: buffer zeroed");
    }

    /* guard bytes must remain unchanged */
    __CPROVER_assert(raw[0] == 0xAA, "guard byte before buffer unchanged");
    __CPROVER_assert(raw[capacity + 1] == 0xBB, "guard byte after buffer unchanged");

    /* clean up */
    free(raw);
}
