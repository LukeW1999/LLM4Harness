#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_acquire_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_ring_buffer ring_buf;
    ensure_ring_buffer_has_allocated_members(&ring_buf);
    ring_buf.allocator = allocator;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(ring_buf.allocation != NULL);
    __CPROVER_assume(ring_buf.allocation_end > ring_buf.allocation);

    size_t capacity = (size_t)(ring_buf.allocation_end - ring_buf.allocation);
    __CPROVER_assume(capacity > 1);
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    size_t requested_size;
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size < capacity);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    struct aws_byte_buf dest;

    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR, "result is a valid aws status");
    __CPROVER_assert(ring_buf.allocator == old_allocator, "allocator is unchanged");
    __CPROVER_assert(ring_buf.allocation == old_allocation, "allocation is unchanged");
    __CPROVER_assert(ring_buf.allocation_end == old_allocation_end, "allocation_end is unchanged");
    __CPROVER_assert(new_tail == old_tail, "tail is unchanged");
    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buf), "ring buffer remains valid");

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(dest.len == 0, "acquired buffer length is zero");
        __CPROVER_assert(dest.capacity == requested_size, "acquired buffer capacity is requested size");
        __CPROVER_assert(dest.allocator == NULL, "acquired buffer allocator is NULL");
        __CPROVER_assert(dest.buffer != NULL, "acquired buffer is non-NULL");
        __CPROVER_assert(aws_byte_buf_is_valid(&dest), "acquired byte buffer is valid");

        __CPROVER_assert(dest.buffer >= old_allocation, "acquired buffer starts within ring buffer allocation");
        __CPROVER_assert(dest.buffer + dest.capacity <= old_allocation_end, "acquired buffer ends within ring buffer allocation");

        if (old_tail > old_head) {
            size_t available_space = (size_t)(old_tail - old_head - 1);
            __CPROVER_assert(available_space >= requested_size, "space between head and tail is sufficient");
            __CPROVER_assert(dest.buffer == old_head, "buffer starts at old head");
            __CPROVER_assert(new_head == old_head + requested_size, "head advances by requested size");
        } else {
            size_t head_space = (size_t)(old_allocation_end - old_head);

            if (dest.buffer == old_head) {
                __CPROVER_assert(head_space >= requested_size, "head space is sufficient");
                if (head_space == requested_size) {
                    __CPROVER_assert(new_head == old_allocation, "head wraps to allocation");
                } else {
                    __CPROVER_assert(new_head == old_head + requested_size, "head advances by requested size");
                }
            } else {
                size_t tail_space = (size_t)(old_tail - old_allocation);
                __CPROVER_assert(dest.buffer == old_allocation, "buffer starts at allocation");
                __CPROVER_assert(tail_space > requested_size, "tail space is sufficient");
                __CPROVER_assert(new_head == old_allocation + requested_size, "head advances from allocation");
            }
        }
    }
}
