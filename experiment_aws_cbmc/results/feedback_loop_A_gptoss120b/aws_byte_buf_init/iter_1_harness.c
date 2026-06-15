#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "aws/common/byte_buf.h"
#include "aws/common/allocator.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_harness() {
    /* 1. Declare inputs and bound them */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = nondet_uint8_t() ? (struct aws_allocator *)malloc(sizeof(struct aws_allocator)) : NULL;
    size_t capacity = nondet_size_t();

    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Save old state for immutability checks */
    struct aws_byte_buf old = buf;

    /* 2. Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 3. Answer the specification questions via asserts */

    /* 1. Fields that change on success */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);                     /* len is set to 0 */
        assert(buf.capacity == capacity);         /* capacity is set to the requested capacity */
        assert(buf.allocator == allocator);       /* allocator is stored */
        if (capacity == 0) {
            assert(buf.buffer == NULL);           /* buffer is NULL when capacity is 0 */
        } else {
            assert(buf.buffer != NULL);           /* buffer is non‑NULL when allocation succeeds */
        }
    }

    /* 2. Fields that remain unchanged (none are left untouched on success) */
    /* No additional unchanged‑field asserts needed because every field is explicitly set. */

    /* 3. Behaviour on failure */
    if (result != AWS_OP_SUCCESS) {
        /* Failure occurs only when capacity != 0 and allocation fails, causing the struct to be zeroed */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    /* 4. Validity invariant must hold after the call */
    assert(aws_byte_buf_is_valid(&buf));
}
