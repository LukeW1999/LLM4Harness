#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>
#include <assert.h>

void aws_ring_buffer_release_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Bound the aws_ring_buffer */
    __CPROVER_assume(aws_ring_buffer_is_bounded(&ring_buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Ensure ring_buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Ensure buf belongs to the pool */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Since the function does not return a status code, we assume it always succeeds */
    assert(AWS_MEM_IS_WRITABLE(ring_buffer.allocation, ring_buffer.allocation_end - ring_buffer.allocation));

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buffer));

    /* Additional assertions based on function behavior */
    assert(AWS_ATOMIC_LOAD(&ring_buffer.tail) == (buf.buffer + buf.capacity));
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
}
