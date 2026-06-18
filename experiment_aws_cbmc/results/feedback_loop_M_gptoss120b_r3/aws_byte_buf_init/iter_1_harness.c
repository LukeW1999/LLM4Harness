#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_buf_init_harness(void) {
    /* 1. Declare the byte buffer */
    struct aws_byte_buf buf;

    /* Save old state for failure case comparison */
    struct aws_byte_buf old = buf;

    /* 2. Non‑deterministic capacity bounded by MAX_BUFFER_SIZE */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 3. Use the default allocator (non‑null as required) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 5. Post‑condition checks */
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
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
        /* The failure can only happen when capacity != 0 and allocation fails */
        assert(capacity != 0);
    }

    /* 6. Invariant: the buffer must always be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
