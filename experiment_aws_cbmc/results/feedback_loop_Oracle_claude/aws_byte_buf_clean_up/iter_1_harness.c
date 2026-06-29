#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;

    /* Non-deterministically choose whether the buffer has an allocator and data */
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

    /* Ensure the buffer is valid before calling clean_up */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under test */
    aws_byte_buf_clean_up(&buf);

    /* Postconditions */
    /* 1. The buffer pointer should be NULL after clean_up */
    assert(buf.buffer == NULL);

    /* 2. The allocator should be NULL after clean_up */
    assert(buf.allocator == NULL);

    /* 3. The length should be 0 after clean_up */
    assert(buf.len == 0);

    /* 4. The capacity should be 0 after clean_up */
    assert(buf.capacity == 0);

    /* 5. The resulting buffer should be valid (zeroed out state is valid) */
    assert(aws_byte_buf_is_valid(&buf));
}

void aws_byte_buf_clean_up_harness(void) {
    aws_byte_buf_clean_up_harness();
    return 0;
}
