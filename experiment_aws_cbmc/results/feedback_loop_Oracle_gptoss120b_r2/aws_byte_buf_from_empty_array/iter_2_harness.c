#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();

    /* Limit capacity to a reasonable size for verification */
    __CPROVER_assume(capacity <= 1024);

    int result = aws_byte_buf_from_empty_array(&buf, allocator, capacity);

    if (result == 0) {
        assert(buf.capacity == capacity);
        assert(buf.len == 0);
        assert(buf.buffer != NULL);
    } else {
        /* On failure the buffer should not be usable */
        assert(buf.buffer == NULL || buf.capacity == 0);
    }

    return 0;
}
