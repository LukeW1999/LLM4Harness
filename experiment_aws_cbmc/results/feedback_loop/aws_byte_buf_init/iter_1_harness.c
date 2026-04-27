#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_byte_buf buf;
    struct aws_allocator allocator;

    /* 2. Bound the structure (limits CBMC state space) */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 3. Allocate pointer members */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 5. Save old state (for checking immutability) */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(buf.buffer, buf.capacity, &old_byte);

    /* 6. Assume function-specific preconditions */
    __CPROVER_assume(&buf != NULL);
    __CPROVER_assume(&allocator != NULL);

    /* 7. Call function under test */
    int result = aws_byte_buf_init(&buf, &allocator, capacity);

    /* 8. Assert postconditions (both branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == &allocator);
        assert(buf.buffer != NULL || capacity == 0);
        assert(AWS_MEM_IS_WRITABLE(buf.buffer, buf.capacity));
    } else {
        assert(result == AWS_OP_ERR);
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }
    assert(aws_byte_buf_is_valid(&buf));  // invariant always holds
    assert_byte_from_buffer_matches(buf.buffer, &old_byte);  // if read-only
}
