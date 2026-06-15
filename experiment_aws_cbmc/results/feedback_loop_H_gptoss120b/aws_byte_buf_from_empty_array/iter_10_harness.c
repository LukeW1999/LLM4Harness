#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_CAPACITY 1024

/* Nondeterministic helpers */
size_t nondet_size_t(void);
bool nondet_bool(void);
struct aws_allocator *make_aws_allocator(void);

/* Harness */
void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = nondet_bool() ? NULL : make_aws_allocator();

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    if (alloc == NULL) {
        if (capacity == 0) {
            /* No allocator, zero capacity: should succeed */
            assert(result == AWS_OP_SUCCESS);
            assert(buf.buffer == NULL);
            assert(buf.len == 0);
            assert(buf.capacity == 0);
            assert(buf.allocator == NULL);
        } else {
            /* No allocator, non‑zero capacity: must fail */
            assert(result != AWS_OP_SUCCESS);
            /* On failure the buffer is not required to be valid */
            assert(!aws_byte_buf_is_valid(&buf));
        }
    } else {
        /* Allocator is present: call must succeed */
        assert(result == AWS_OP_SUCCESS);
        assert(buf.allocator == alloc);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
            assert(buf.len == 0);
            assert(buf.capacity == 0);
        } else {
            assert(buf.buffer != NULL);
            assert(buf.len == 0);
            assert(buf.capacity == capacity);
        }
    }

    /* If the call succeeded, the buffer must be valid */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&buf));
    }
}

/* Entry point for CBMC */
int main(void) {
    aws_byte_buf_from_empty_array_harness();
    return 0;
}
