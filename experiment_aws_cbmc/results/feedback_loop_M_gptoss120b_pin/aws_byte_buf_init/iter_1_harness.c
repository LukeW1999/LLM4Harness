#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_harness(void) {
    /* 1. Declare the byte buffer and bound it */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old = buf;

    /* 3. Non‑deterministic capacity, bounded */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 4. Use the default allocator (non‑null) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 5. Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 6. Result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* 7. Post‑conditions for the success path */
    if (result == AWS_OP_SUCCESS) {
        /* len is set to zero */
        assert(buf.len == 0);
        /* capacity is set to the requested capacity */
        assert(buf.capacity == capacity);
        /* allocator is stored */
        assert(buf.allocator == allocator);
        /* buffer pointer follows the allocation rule */
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        /* 8. Post‑conditions for the failure path (allocation failure) */
        /* The struct is zeroed */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    /* 9. Invariant: the byte buffer must always be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
