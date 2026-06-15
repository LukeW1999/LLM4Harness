#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_clean_up_harness() {
    struct aws_byte_buf buf;
    /* Non-deterministic but valid buf */
    buf.allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    /* buffer may be NULL or allocated */
    if (nondet_bool()) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct aws_byte_buf old = buf;

    aws_byte_buf_clean_up(&buf);

    /* Postconditions: buffer freed and zeroed, allocator unchanged */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == old.allocator);
    /* Note: If old.buffer was non-NULL, memory has been freed; cannot assert directly */
}
