#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_CAPACITY 1024

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;

    /* initialize buf with nondeterministic values to avoid uninitialized warnings */
    buf.buffer = nondet_pointer();
    buf.len = nondet_unsigned_long();
    buf.capacity = nondet_unsigned_long();
    buf.allocator = nondet_pointer();

    /* nondeterministic capacity */
    size_t capacity = nondet_unsigned_long();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    /* nondeterministic allocator (may be NULL) */
    struct aws_allocator *alloc = nondet_pointer();

    /* call the function under test */
    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    /* postconditions */
    if (alloc == NULL) {
        __CPROVER_assert(result != AWS_OP_SUCCESS, "allocation with NULL allocator must fail");
        __CPROVER_assert(buf.buffer == NULL, "buf.buffer must be NULL when allocator is NULL");
        __CPROVER_assert(buf.len == 0, "buf.len must be 0 when allocator is NULL");
        __CPROVER_assert(buf.capacity == 0, "buf.capacity must be 0 when allocator is NULL");
        __CPROVER_assert(buf.allocator == NULL, "buf.allocator must be NULL when allocator is NULL");
    } else {
        __CPROVER_assert(result == AWS_OP_SUCCESS, "allocation with non‑NULL allocator must succeed");
        __CPROVER_assert(buf.allocator == alloc, "buf.allocator must be set to the provided allocator");
        if (capacity == 0) {
            __CPROVER_assert(buf.buffer == NULL, "buf.buffer must be NULL when capacity is 0");
            __CPROVER_assert(buf.len == 0, "buf.len must be 0 when capacity is 0");
            __CPROVER_assert(buf.capacity == 0, "buf.capacity must be 0 when capacity is 0");
        } else {
            __CPROVER_assert(buf.buffer != NULL, "buf.buffer must be non‑NULL when capacity > 0");
            __CPROVER_assert(buf.len == 0, "buf.len must be 0 after initialization");
            __CPROVER_assert(buf.capacity == capacity, "buf.capacity must equal the requested capacity");
        }
    }

    /* invariant must always hold */
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "aws_byte_buf_is_valid must hold");
}
