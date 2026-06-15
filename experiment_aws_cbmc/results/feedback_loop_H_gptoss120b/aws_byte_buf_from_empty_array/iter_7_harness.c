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
struct aws_allocator *nondet_allocator_ptr(void);
bool nondet_bool(void);
void *nondet_pointer(void);
unsigned long nondet_unsigned_long(void);

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;

    /* allocator may be NULL or a valid allocator */
    struct aws_allocator *alloc = nondet_bool() ? NULL : make_aws_allocator();

    /* capacity is nondeterministic but bounded */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    if (alloc == NULL) {
        if (capacity == 0) {
            /* With no allocator and zero capacity the call should succeed */
            assert(result == AWS_OP_SUCCESS);
            assert(buf.buffer == NULL);
            assert(buf.len == 0);
            assert(buf.capacity == 0);
            assert(buf.allocator == NULL);
        } else {
            /* With no allocator and non‑zero capacity the call must fail */
            assert(result != AWS_OP_SUCCESS);
            /* The buffer fields are unspecified on failure; we only require they are not
               mistakenly marked as valid. */
            assert(!aws_byte_buf_is_valid(&buf));
        }
    } else {
        /* Allocator is present */
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

    /* Final validity check for the successful case */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&buf));
    }
}
