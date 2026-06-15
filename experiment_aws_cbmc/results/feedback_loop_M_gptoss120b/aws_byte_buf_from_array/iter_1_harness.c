#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* CBMC harness for aws_byte_buf_init */
void aws_byte_buf_init_harness(void) {
    /* 1. Declare the buffer and nondeterministic capacity */
    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();

    /* Bound the capacity to avoid unbounded state space */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Use the default allocator (always non‑NULL) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Save the old state (for completeness, even though it is not
       required for the failure path) */
    struct aws_byte_buf old = buf;

    /* 3. Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 4. Post‑condition checks for both success and error paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: fields set according to the specification */
        assert(buf.allocator == allocator);
        assert(buf.capacity == capacity);
        assert(buf.len == 0);

        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        /* Error: the function zeroes the entire structure */
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }

    /* 5. Fields that must remain unchanged regardless of the result.
       In this function the only field that could change on error is the
       allocator pointer, which is explicitly set to NULL on failure, so we
       only need to assert the invariant that the buffer is valid. */

    /* 6. The validity invariant must always hold */
    assert(aws_byte_buf_is_valid(&buf));
}
