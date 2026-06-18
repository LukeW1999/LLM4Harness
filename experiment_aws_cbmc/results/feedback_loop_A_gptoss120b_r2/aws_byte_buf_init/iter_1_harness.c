#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old = buf;

    /* 3. Nondeterministic capacity (bounded) */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 4. Use the default allocator (always non‑NULL) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 5. Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 6. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: fields are set according to the specification */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        /* Failure: the struct is zeroed */
        assert(result == AWS_OP_ERR);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    /* 7. Fields that must remain unchanged regardless of the result */
    /* (All fields are overwritten by the function, so no unchanged fields to assert) */

    /* 8. Validity invariant must always hold */
    assert(aws_byte_buf_is_valid(&buf));
}
