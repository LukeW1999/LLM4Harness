#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cbmc_proof/cbmc_utils.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_byte_buf buf;
    struct aws_allocator allocator;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded((struct aws_byte_buf *)&allocator, MAX_BUFFER_SIZE));

    /* 3. Allocate pointer members */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 5. Save old state (for checking immutability) */
    struct aws_byte_buf old = buf;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)buf.buffer, buf.capacity, &old_byte);

    /* 6. Assume function-specific preconditions */
    __CPROVER_assume(allocator.mem_acquire != NULL);
    __CPROVER_assume(allocator.mem_release != NULL);
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 7. Call function under test */
    int result = aws_byte_buf_init(&buf, &allocator, capacity);

    /* 8. Assert postconditions (both branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == &allocator);
        assert(AWS_MEM_IS_WRITABLE(buf.buffer, buf.capacity));
    } else {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == &allocator);
    }
    assert(aws_byte_buf_is_valid(&buf));  // invariant always holds
    assert_byte_from_buffer_matches((uint8_t *)old.buffer, &old_byte);  // if read-only
}
