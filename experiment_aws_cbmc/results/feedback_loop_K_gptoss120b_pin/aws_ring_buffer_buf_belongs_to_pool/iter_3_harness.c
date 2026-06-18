#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/* Forward declaration for nondeterministic uint64 generator */
uint64_t nondet_uint64_t(void);

/* Helper to compute the expected result of aws_ring_buffer_buf_belongs_to_pool */
static bool expected_belongs(const struct aws_ring_buffer *ring_buffer,
                             const struct aws_byte_buf *buf) {
    if (buf->buffer == NULL) {
        return false;
    }
    const uint8_t *alloc_start = ring_buffer->allocation;
    const uint8_t *alloc_end   = ring_buffer->allocation_end;

    /* buf must start within the allocation */
    if (buf->buffer < alloc_start || buf->buffer >= alloc_end) {
        return false;
    }

    /* buf must not extend past the allocation */
    if ((size_t)(alloc_end - buf->buffer) < buf->capacity) {
        return false;
    }

    return true;
}

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Non‑deterministic size for the ring buffer allocation (bounded to avoid overflow) */
    size_t ring_buffer_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(ring_buffer_size > 0);
    __CPROVER_assume(ring_buffer_size < (1ULL << 30)); /* reasonable upper bound */

    /* Allocate and initialize the ring buffer */
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, ring_buffer_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Make a nondeterministic byte buffer */
    struct aws_byte_buf buf = make_aws_byte_buf();

    /* Preserve copies of the inputs for frame condition checks */
    struct aws_ring_buffer ring_buffer_before = ring_buffer;
    struct aws_byte_buf   buf_before = buf;

    /* Call the function under verification */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Postcondition: result matches the expected logical condition */
    bool expected = expected_belongs(&ring_buffer_before, &buf_before);
    assert(result == expected);

    /* Frame condition: ring buffer must be unchanged */
    assert(memcmp(&ring_buffer, &ring_buffer_before, sizeof(struct aws_ring_buffer)) == 0);

    /* Frame condition: byte buffer must be unchanged */
    assert(memcmp(&buf, &buf_before, sizeof(struct aws_byte_buf)) == 0);

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
