#include <assert.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_CAPACITY 1024

/* Nondeterministic helpers */
size_t nondet_size_t(void);
struct aws_allocator *nondet_allocator_ptr(void);
bool nondet_bool(void);
void *nondet_pointer(void);
unsigned long nondet_unsigned_long(void);

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;

    /* Initialize buf with nondeterministic values to silence warnings */
    buf.buffer   = nondet_pointer();
    buf.len      = nondet_unsigned_long();
    buf.capacity = nondet_unsigned_long();
    buf.allocator = nondet_allocator_ptr();

    /* Choose allocator: either NULL or a valid mock allocator */
    struct aws_allocator *alloc;
    if (nondet_bool()) {
        alloc = NULL;
    } else {
        alloc = make_aws_allocator();   /* provided by proof_helpers */
    }

    /* Choose capacity */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    /* Call the function under test */
    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    /* Post‑condition checks */
    if (alloc == NULL) {
        assert(result != AWS_OP_SUCCESS);
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    } else {
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

    /* Invariant must always hold */
    assert(aws_byte_buf_is_valid(&buf));
}
