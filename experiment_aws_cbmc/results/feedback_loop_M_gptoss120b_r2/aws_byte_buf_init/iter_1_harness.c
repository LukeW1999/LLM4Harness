#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_byte_buf_init_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Bound the byte buffer structure to keep state space finite */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 2. Save old state */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 4. Post‑condition checks */
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
        /* Failure: the structure is zeroed */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    /* 5. Invariant: the byte buffer must always be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
