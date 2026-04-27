#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;

    /* Use a valid allocator or NULL */
    bool use_allocator = nondet_bool();
    if (use_allocator) {
        buf.allocator = aws_default_allocator();
    } else {
        buf.allocator = NULL;
    }

    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();

    if (buf.capacity == 0) {
        buf.len = 0;
        buf.buffer = NULL;
    } else {
        __CPROVER_assume(buf.len <= buf.capacity);
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    }

    /* Precondition: buf must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under test */
    aws_byte_buf_clean_up(&buf);

    /* === Post-condition assertions === */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    assert(aws_byte_buf_is_valid(&buf));
}
