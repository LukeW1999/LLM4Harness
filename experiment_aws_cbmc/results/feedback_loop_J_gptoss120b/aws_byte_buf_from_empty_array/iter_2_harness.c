#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum size for bounding the capacity – defined in the Makefile */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_byte_buf_from_empty_array_harness(void) {
    /* 1. Allocate allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Nondeterministically choose a capacity within bounds */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 3. Declare the byte buffer (uninitialized) */
    struct aws_byte_buf buf;

    /* 4. Call the function under test */
    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Buffer should be empty but have the requested capacity */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == alloc);

        if (capacity > 0) {
            assert(buf.buffer != NULL);
        } else {
            assert(buf.buffer == NULL);
        }
    } else {
        /* On failure the buffer fields must be zeroed */
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }

    /* 6. Validity invariant must always hold */
    assert(aws_byte_buf_is_valid(&buf));
}
