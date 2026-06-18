#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

static bool expected_belongs(const struct aws_ring_buffer *ring_buffer,
                             const struct aws_byte_buf *buf) {
    if (buf->buffer == NULL) {
        return false;
    }
    const uint8_t *alloc_start = ring_buffer->allocation;
    const uint8_t *alloc_end   = ring_buffer->allocation_end;

    if (buf->buffer < alloc_start || buf->buffer >= alloc_end) {
        return false;
    }

    if ((size_t)(alloc_end - buf->buffer) < buf->capacity) {
        return false;
    }

    return true;
}

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    size_t ring_buffer_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(ring_buffer_size > 0);
    __CPROVER_assume(ring_buffer_size < (1ULL << 30));

    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, ring_buffer_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf = make_aws_byte_buf();

    struct aws_ring_buffer ring_buffer_before = ring_buffer;
    struct aws_byte_buf   buf_before = buf;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    bool expected = expected_belongs(&ring_buffer_before, &buf_before);
    __CPROVER_assert(result == expected, "result matches expected");

    __CPROVER_assert(memcmp(&ring_buffer, &ring_buffer_before, sizeof(struct aws_ring_buffer)) == 0,
                     "ring_buffer unchanged");
    __CPROVER_assert(memcmp(&buf, &buf_before, sizeof(struct aws_byte_buf)) == 0,
                     "buf unchanged");

    aws_ring_buffer_clean_up(&ring_buffer);
}
