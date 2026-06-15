#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    if (__CPROVER_nondet_bool()) {
        size_t cap = __CPROVER_nondet_size_t();
        __CPROVER_assume(cap > 0 && cap <= MAX_BUFFER_SIZE);
        uint8_t *mem = (uint8_t *)malloc(cap);
        __CPROVER_assume(mem != NULL);
        buf.buffer = mem;
        buf.capacity = cap;
        buf.len = __CPROVER_nondet_size_t();
        __CPROVER_assume(buf.len <= buf.capacity);
        buf.allocator = allocator;
    } else {
        buf.buffer = NULL;
        buf.capacity = 0;
        buf.len = 0;
        buf.allocator = allocator;
    }

    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer buf_storage;
    if (buf.buffer != NULL && buf.capacity > 0) {
        save_byte_from_array(buf.buffer, buf.capacity, &buf_storage);
    }

    aws_byte_buf_reset(&buf);

    /* Post‑conditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.allocator == old_buf.allocator);

    if (old_buf.buffer != NULL) {
        assert(buf.buffer == old_buf.buffer);
        assert(buf.capacity == old_buf.capacity);
        if (buf.buffer != NULL && buf.capacity > 0) {
            assert_byte_from_buffer_matches(buf.buffer, &buf_storage);
        }
    } else {
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
    }
}

int main(void) {
    aws_byte_buf_reset_harness();
    return 0;
}
