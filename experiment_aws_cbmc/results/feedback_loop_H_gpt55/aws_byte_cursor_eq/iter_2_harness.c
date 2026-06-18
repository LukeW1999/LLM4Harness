#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 10
#endif

void aws_byte_cursor_eq_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t allocation_size = MAX_BUFFER_SIZE;
    if (allocation_size == 0) {
        allocation_size = 1;
    }

    uint8_t *a_buffer = aws_mem_acquire(allocator, allocation_size);
    uint8_t *b_buffer = aws_mem_acquire(allocator, allocation_size);

    __CPROVER_assume(a_buffer != NULL);
    __CPROVER_assume(b_buffer != NULL);

    size_t a_len;
    size_t b_len;

    __CPROVER_assume(a_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(b_len <= MAX_BUFFER_SIZE);

    struct aws_byte_cursor a = {
        .len = a_len,
        .ptr = a_buffer,
    };

    struct aws_byte_cursor b = {
        .len = b_len,
        .ptr = b_buffer,
    };

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    bool result = aws_byte_cursor_eq(&a, &b);

    if (result) {
        assert(a.len == b.len);
    }

    if (old_a.len != old_b.len) {
        assert(!result);
    }

    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
