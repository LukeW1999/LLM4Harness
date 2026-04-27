#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.buffer == (capacity == 0 ? NULL : (uint8_t *)aws_mem_acquire(allocator, capacity)));
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
    } else {
        assert(buf.buffer == old.buffer);
        assert(buf.len == old.len);
        assert(buf.capacity == old.capacity);
        assert(buf.allocator == old.allocator);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(buf.allocator == old.allocator);
    assert(buf.capacity == old.capacity);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
