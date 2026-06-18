#include <proof_helpers/make_common_data_structures.h>

#define MAX_CAPACITY 1024

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t capacity;
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    struct aws_byte_buf buf = {0};
    struct aws_byte_buf old_buf = buf;

    int result = aws_byte_buf_from_empty_array(&buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(buf.allocator == allocator, "allocator set on success");
        __CPROVER_assert(buf.capacity == capacity, "capacity set on success");
        __CPROVER_assert(buf.len == 0, "len zero on success");
        if (capacity == 0) {
            __CPROVER_assert(buf.buffer == NULL, "buffer NULL when capacity zero");
        } else {
            __CPROVER_assert(buf.buffer != NULL, "buffer non‑NULL when capacity > 0");
        }
        __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf valid after success");
    } else {
        __CPROVER_assert(buf.buffer == old_buf.buffer, "buffer unchanged on failure");
        __CPROVER_assert(buf.len == old_buf.len, "len unchanged on failure");
        __CPROVER_assert(buf.capacity == old_buf.capacity, "capacity unchanged on failure");
        __CPROVER_assert(buf.allocator == old_buf.allocator, "allocator unchanged on failure");
        __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf valid after failure");
    }
}
