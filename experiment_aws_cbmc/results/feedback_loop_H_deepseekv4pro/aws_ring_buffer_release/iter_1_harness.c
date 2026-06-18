#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    /* allocate ring buffer's internal memory */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= MAX_BUFFER_SIZE);
    uint8_t *allocation = malloc(capacity);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + capacity;

    /* set up the byte buffer to be released, ensuring it is within the ring buffer's allocation */
    size_t buf_start = nondet_size_t();
    __CPROVER_assume(buf_start < capacity);
    size_t buf_cap = nondet_size_t();
    __CPROVER_assume(buf_cap > 0 && buf_cap <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf_start + buf_cap <= capacity);

    buf.buffer = allocation + buf_start;
    buf.capacity = buf_cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf_cap);
    buf.allocator = aws_default_
