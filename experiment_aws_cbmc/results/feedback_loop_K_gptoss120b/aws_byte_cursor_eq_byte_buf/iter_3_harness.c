#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf b;
    struct aws_byte_cursor a;

    size_t capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(capacity <= SIZE_MAX);
    if (!aws_byte_buf_init(&b, allocator, capacity)) {
        b.buffer = NULL;
        b.capacity = 0;
    }

    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= b.capacity);
    b.len = len;

    a.ptr = b.buffer;
    a.len = len;

    __CPROVER_assume(aws_byte_buf_is_valid(&b));
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    (void)aws_byte_cursor_eq_byte_buf(&a, &b);
}
