#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>

#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/assertions.h"

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Non‑deterministic allocator (must be non‑NULL) */
    struct aws_allocator *alloc;
    __CPROVER_assume(alloc != NULL);

    /* 3. Destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* 4. Preserve old source state */
    struct aws_byte_buf old_src = src;

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Post‑conditions expressed with implications so both branches are reachable */
    /* Success case */
    assert((result == AWS_OP_SUCCESS) ==> (dest.allocator == alloc));
    assert((result == AWS_OP_SUCCESS) ==> (dest.capacity == src.capacity));
    assert((result == AWS_OP_SUCCESS) ==> (dest.len == src.len));

    assert((result == AWS_OP_SUCCESS && src.capacity == 0) ==> (dest.buffer == NULL));
    assert((result == AWS_OP_SUCCESS && src.capacity != 0) ==> (dest.buffer != NULL));
    assert((result == AWS_OP_SUCCESS && src.capacity != 0) ==> (assert_bytes_match(dest.buffer, src.buffer, src.len)));

    /* Failure case */
    assert((result != AWS_OP_SUCCESS) ==> (dest.buffer == NULL));
    assert((result != AWS_OP_SUCCESS) ==> (dest.len == 0));
    assert((result != AWS_OP_SUCCESS) ==> (dest.capacity == 0));
    assert((result != AWS_OP_SUCCESS) ==> (dest.allocator == NULL));

    /* 7. Source must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* 8. Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
