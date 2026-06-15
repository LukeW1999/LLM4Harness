#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_CAPACITY 1024

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;

    /* nondeterministic capacity */
    size_t capacity = nondet_unsigned_long();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    /* nondeterministic allocator (may be NULL) */
    struct aws_allocator *alloc = nondet_pointer();
    __CPROVER_assume(alloc == NULL || alloc != NULL);

    /* call the function under test */
    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    if (alloc == NULL) {
        /* allocation should fail */
        assert(result != AWS_OP_SUCCESS);
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    } else {
        /* allocation should succeed */
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

    /* invariant must always hold */
    assert(aws_byte_buf_is_valid(&buf));
}
