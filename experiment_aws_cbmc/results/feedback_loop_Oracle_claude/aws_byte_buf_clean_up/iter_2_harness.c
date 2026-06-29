#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;

    bool has_allocator;
    bool has_buffer;
    size_t capacity;
    size_t len;

    __CPROVER_assume(capacity <= 10);
    __CPROVER_assume(len <= capacity);

    buf.capacity = capacity;
    buf.len = len;

    if (has_allocator) {
        buf.allocator = aws_default_allocator();
    } else {
        buf.allocator = NULL;
    }

    if (has_buffer && capacity > 0) {
        buf.buffer = malloc(capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
        buf.capacity = 0;
        buf.len = 0;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    aws_byte_buf_clean_up(&buf);

    assert(buf.buffer == NULL);
    assert(buf.allocator == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(aws_byte_buf_is_valid(&buf));
}
