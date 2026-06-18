#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf src;
    struct aws_byte_cursor cursor;
    struct aws_byte_buf dest;

    /* nondet capacity for source buffer, bounded to avoid huge allocations */
    size_t capacity = nondet_uint64_t();
    __CPROVER_assume(capacity <= 1024);

    if (aws_byte_buf_init(&src, allocator, capacity) != AWS_OP_SUCCESS) {
        return;
    }

    /* nondet length for source buffer, must be within capacity */
    src.len = nondet_uint64_t();
    __CPROVER_assume(src.len <= src.capacity);

    /* create a cursor that points to the source buffer */
    cursor = aws_byte_cursor_from_buf(&src);

    /* call the function under test */
    aws_byte_buf_init_copy_from_cursor(&dest, allocator, cursor);

    /* clean up allocated resources */
    aws_byte_buf_clean_up(&src);
    aws_byte_buf_clean_up(&dest);
}
