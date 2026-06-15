#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = nondet_bool() ? malloc(sizeof(struct aws_allocator)) : NULL;
    size_t capacity;
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(capacity < MAX_BUFFER_SIZE);

    int result = aws_byte_buf_init(&buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
            assert(AWS_MEM_IS_WRITABLE(buf.buffer, capacity));
        }
        assert(aws_byte_buf_is_valid(&buf));
    } else {
        assert(result == AWS_OP_ERR);
        assert(buf.len == 0);
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
        assert(aws_byte_buf_is_valid(&buf));
    }
}
