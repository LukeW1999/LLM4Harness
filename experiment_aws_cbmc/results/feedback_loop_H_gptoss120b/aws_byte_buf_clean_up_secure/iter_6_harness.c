#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

int main(void) {
    /* Obtain a non‑null allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* Set up a byte buffer with nondeterministic but bounded fields */
    struct aws_byte_buf buf;
    buf.allocator = alloc;

    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    buf.capacity = capacity;

    if (capacity > 0) {
        /* Allocate a nondeterministic buffer; may be NULL */
        buf.buffer = (uint8_t *)alloc->mem_acquire(alloc, capacity);
        /* For the zero‑isation check we need a non‑NULL buffer */
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    size_t len;
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    /* Preserve the original state for later checks */
    struct aws_byte_buf old = buf;

    /* Save a byte from the original buffer (if any) to verify zeroisation */
    struct store_byte_from_buffer storage = {0};
    if (old.buffer != NULL && old.capacity > 0) {
        save_byte_from_array(old.buffer, old.capacity, &storage);
    }

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Post‑conditions */
    assert(buf.allocator == old.allocator);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    if (old.buffer != NULL && old.capacity > 0) {
        assert(storage.ptr != NULL);
        assert(*storage.ptr == 0);
    }

    /* The resulting buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    return 0;
}
