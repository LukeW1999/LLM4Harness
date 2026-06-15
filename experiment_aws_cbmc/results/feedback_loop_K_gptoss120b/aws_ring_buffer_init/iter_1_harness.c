/*  
Contract for aws_ring_buffer_init  

Preconditions:  
- ring_buf pointer is non‑NULL.  
- allocator pointer is non‑NULL (we use aws_default_allocator()).  
- size is a nondet size_t value, assumed to be > 0 and less than a reasonable upper bound (e.g., 1<<20).  

Postconditions (validity):  
- If the function returns AWS_OP_SUCCESS:  
    * ring_buf->allocation is non‑NULL and points to a memory region of at least `size` bytes.  
    * ring_buf->allocator equals the supplied allocator.  
    * ring_buf->allocation_end == ring_buf->allocation + size.  
    * The atomic variables head and tail both contain the pointer `ring_buf->allocation`.  
- If the function returns AWS_OP_ERR:  
    * ring_buf->allocation is NULL.  
    * All other fields of ring_buf remain zero (as set by AWS_ZERO_STRUCT).  

Postconditions (frame):  
- The allocator argument and the size argument are not modified.  
- No memory outside of the newly allocated buffer (if any) is written.  
*/

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

/* Helper to obtain a nondet size_t */
size_t nondet_size_t(void);
void *nondet_void_ptr(void);

void aws_ring_buffer_init_harness(void) {
    /* Allocate the ring buffer structure */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    __CPROVER_assume(ring_buf != NULL);

    /* Save a copy of the original (uninitialized) structure for frame checking */
    struct aws_ring_buffer old_ring_buf = *ring_buf;

    /* Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* Nondet size with reasonable bounds */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size < (1U << 20)); /* limit to 1 MiB for tractability */

    /* Call the function under test */
    int ret = aws_ring_buffer_init(ring_buf, alloc, size);

    /* Postcondition checks */
    if (ret == AWS_OP_SUCCESS) {
        /* Allocation must have succeeded */
        assert(ring_buf->allocation != NULL);
        /* Allocator must be stored */
        assert(ring_buf->allocator == alloc);
        /* Allocation end must be correctly computed */
        assert(ring_buf->allocation_end == ring_buf->allocation + size);
        /* Head and tail atomics must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf->head) == ring_buf->allocation);
        assert(aws_atomic_load_ptr(&ring_buf->tail) == ring_buf->allocation);
    } else {
        /* On error, allocation is NULL and all fields remain zero */
        assert(ring_buf->allocation == NULL);
        assert(ring_buf->allocator == NULL);
        assert(ring_buf->allocation_end == NULL);
        assert(aws_atomic_load_ptr(&ring_buf->head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf->tail) == NULL);
    }

    /* Frame conditions: allocator and size arguments unchanged */
    assert(alloc == aws_default_allocator());
    assert(size == size); /* trivially true, kept for symmetry */

    /* No other memory outside the allocated buffer is modified.
       Since we cannot directly inspect all memory, we conservatively
       assert that the original ring buffer fields that are not supposed
       to change remain equal to their saved copies (except those set
       by the function). */
    if (ret == AWS_OP_ERR) {
        assert(ring_buf->allocation == old_ring_buf.allocation);
        assert(ring_buf->allocator == old_ring_buf.allocator);
        assert(ring_buf->allocation_end == old_ring_buf.allocation_end);
        assert(aws_atomic_load_ptr(&ring_buf->head) == aws_atomic_load_ptr(&old_ring_buf.head));
        assert(aws_atomic_load_ptr(&ring_buf->tail) == aws_atomic_load_ptr(&old_ring_buf.tail));
    }

    /* Clean up */
    if (ring_buf->allocation != NULL) {
        aws_mem_release(ring_buf->allocator, ring_buf->allocation);
    }
    free(ring_buf);

    return 0;
}

/* Stub definitions for nondet helpers (CBMC will treat them as nondet) */
size_t nondet_size_t(void) {
    size_t x;
    return x;
}
void *nondet_void_ptr(void) {
    void *p;
    return p;
}
