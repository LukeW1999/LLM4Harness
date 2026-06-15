#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_CAPACITY 1024

/* Nondeterministic helpers provided by CBMC */
size_t nondet_size_t(void);
struct aws_allocator *make_aws_allocator(void);

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = make_aws_allocator();
    __CPROVER_assume(alloc != NULL);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    /* The function must return either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* Successful initialization */
        assert(buf.allocator == alloc);
        assert(buf.len == 0);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
            assert(buf.capacity == 0);
        } else {
            assert(buf.buffer != NULL);
            assert(buf.capacity == capacity);
        }
        assert(aws_byte_buf_is_valid(&buf));
    } else {
        /* On failure, the allocator field must still be set */
        assert(buf.allocator == alloc);
        /* Other fields are unspecified in the error case */
    }
}

/* Entry point for CBMC */
int main(void) {
    aws_byte_buf_from_empty_array_harness();
    return 0;
}
