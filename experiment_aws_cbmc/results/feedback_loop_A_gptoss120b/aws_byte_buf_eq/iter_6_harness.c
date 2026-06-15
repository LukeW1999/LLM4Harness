#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>

#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_eq_harness(void) {
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    bool result = aws_byte_buf_eq(&a, &b);

    bool expected = false;
    if (a.len == b.len) {
        bool match = true;
        for (size_t i = 0; i < a.len; ++i) {
            if (a.buffer[i] != b.buffer[i]) {
                match = false;
                break;
            }
        }
        expected = match;
    }

    assert(result == expected);
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
