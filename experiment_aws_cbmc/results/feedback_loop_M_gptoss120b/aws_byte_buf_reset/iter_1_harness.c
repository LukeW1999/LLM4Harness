#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* CBMC harness for aws_byte_buf_init */
void aws_byte_buf_init_harness(void) {
    /* Allocate the buffer struct on the stack */
    struct aws_byte_buf buf;

    /* Use the default allocator (always non‑NULL) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Nondeterministic capacity, bounded to avoid state explosion */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Call the function under test */
    int result = aws_byte_buf_init(&buf, alloc, capacity);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success guarantees */
        assert(buf.allocator == alloc);
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        /* Failure path: the function zeroes the struct */
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }

    /* Invariant: the buffer must always be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
