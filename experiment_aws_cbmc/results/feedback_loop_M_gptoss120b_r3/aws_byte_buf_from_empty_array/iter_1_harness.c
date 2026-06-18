#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* CBMC harness for aws_byte_buf_init */
void aws_byte_buf_init_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 2. Save old state */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    int result = aws_byte_buf_init(&buf, alloc, capacity);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* fields that must change on success */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == alloc);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        /* on failure the struct is zeroed */
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }

    /* 5. Invariant: the buffer must always be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
