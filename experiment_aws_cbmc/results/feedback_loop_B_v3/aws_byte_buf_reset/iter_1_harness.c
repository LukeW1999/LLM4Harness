#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_byte_buf buf;
    struct aws_allocator allocator;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 3. Allocate pointer members */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 4. Assume validity precondition (infer from how function uses the struct) */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 5. Save old state (for checking immutability) */
    struct aws_byte_buf old = buf;

    /* 6. Assume function-specific preconditions (infer from code logic) */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 7. Call function under test */
    int result = aws_byte_buf_init(&buf, &allocator, capacity);

    /* 8. Assert postconditions (infer from what the function guarantees) */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == &allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == &allocator);
    }

    assert(aws_byte_buf_is_valid(&buf));  // invariant always holds
}
