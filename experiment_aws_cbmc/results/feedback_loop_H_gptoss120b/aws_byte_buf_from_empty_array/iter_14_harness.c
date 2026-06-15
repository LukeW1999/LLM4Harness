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
bool nondet_bool(void);
struct aws_allocator *make_aws_allocator(void);

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = nondet_bool() ? NULL : make_aws_allocator();

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    /* General post‑condition: result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* If the call succeeded, the buffer must be correctly initialized */
    assert((result == AWS_OP_SUCCESS) ==> (
        buf.allocator == alloc &&
        buf.len == 0 &&
        ((capacity == 0) ?
            (buf.buffer == NULL && buf.capacity == 0) :
            (buf.buffer != NULL && buf.capacity == capacity)) &&
        aws_byte_buf_is_valid(&buf)
    ));

    /* If the call failed, the allocator field must at least reflect the input allocator */
    assert((result != AWS_OP_SUCCESS) ==> (
        (alloc == NULL) ? (buf.allocator == NULL) : (buf.allocator == alloc)
    ));
}

/* Entry point for CBMC */
int main(void) {
    aws_byte_buf_from_empty_array_harness();
    return 0;
}
