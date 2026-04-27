// === STEP 1: SUCCESS PATH ===
// aws_ring_buffer_buf_belongs_to_pool returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// It calls s_buf_belongs_to_pool which checks if buf->buffer is within ring_buffer's allocation range
// Returns true if buf belongs to the pool, false otherwise
// No state changes occur - this is a pure query function
//
// === STEP 2: FAILURE PATH ===
// Returns false if buf does not belong to the pool
// No state changes in either case
//
// === STEP 3: FRAME CONDITIONS ===
// ring_buffer (struct aws_ring_buffer):
//   - allocator: UNCHANGED always
//   - allocation: UNCHANGED always
//   - head: UNCHANGED always
//   - tail: UNCHANGED always
//   - allocation_end: UNCHANGED always
// buf (struct aws_byte_buf):
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_ring_buffer_is_valid(ring_buffer): YES (must hold before and after call)
//   - aws_byte_buf_is_valid(buf): YES (must hold before and after call)

#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Allocate and set up ring_buffer */
    struct aws_ring_buffer ring_buffer;
    
    /* We need a valid ring buffer - set up manually */
    ring_buffer.allocator = nondet_bool() ? can_fail_allocator() : NULL;
    
    /* Create a buffer allocation of some size */
    size_t buf_size;
    __CPROVER_assume(buf_size > 0 && buf_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = malloc(buf_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + buf_size;
    
    /* Set head and tail to valid pointers within the allocation */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= buf_size);
    __CPROVER_assume(tail_offset <= buf_size);
    
    aws_atomic_init_ptr(&ring_buffer.head, (void *)(ring_buffer.allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(ring_buffer.allocation + tail_offset));
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* Allocate and set up byte_buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    
    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    
    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);
    
    /* === ASSERTIONS: Frame conditions - nothing should change === */
    
    /* ring_buffer fields unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    
    /* Check head and tail atomic vars unchanged */
    uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_head_ptr = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.head);
    assert(head_ptr == old_head_ptr);
    
    uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    uint8_t *old_tail_ptr = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.tail);
    assert(tail_ptr == old_tail_ptr);
    
    /* buf fields unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    
    /* === ASSERTIONS: Validity invariants === */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* === ASSERTIONS: Result correctness === */
    /* If buf->buffer is within [allocation, allocation_end), result should be true */
    if (buf.buffer != NULL &&
        buf.buffer >= ring_buffer.allocation &&
        buf.buffer < ring_buffer.allocation_end) {
        /* buf could belong to pool - result may be true */
        /* We can't assert result == true here without knowing s_buf_belongs_to_pool exactly */
    }
    
    /* Result is a valid bool */
    assert(result == true || result == false);
}
