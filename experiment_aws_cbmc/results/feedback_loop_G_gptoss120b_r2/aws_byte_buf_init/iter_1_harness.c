#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Save old state for potential immutability checks */
    struct aws_byte_buf old = buf;

    /* Use the default allocator as required */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Non‑deterministic capacity, bounded by the maximum buffer size */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 2. Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 3. Post‑condition checks for both success and failure paths */
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
        /* Failure: the buffer is zeroed */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    /* 4. Validity invariant must always hold */
    assert(aws_byte_buf_is_valid(&buf));
}
