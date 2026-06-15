#include <assert.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/assertions.h"

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_clean_up_secure_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* byte buffer */
    struct aws_byte_buf buf;
    buf.allocator = alloc;

    /* nondet capacity bounded by MAX_BUFFER_SIZE */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    buf.capacity = capacity;

    /* allocate internal buffer if capacity > 0 */
    if (capacity > 0) {
        buf.buffer = (uint8_t *)alloc->mem_acquire(alloc, capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    /* nondet length bounded by capacity */
    size_t len;
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    /* ensure the buffer satisfies its invariant before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state */
    struct aws_byte_buf old = buf;

    /* capture a byte inside the buffer (if any) */
    struct store_byte_from_buffer storage = {0};
    if (buf.buffer != NULL && buf.capacity > 0) {
        save_byte_from_array(buf.buffer, buf.capacity, &storage);
    }

    /* call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* post‑condition checks */

    /* allocator must be unchanged */
    assert(buf.allocator == old.allocator);

    /* buffer fields must be cleared */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* the captured byte must have been zeroed before the memory was released */
    if (old.buffer != NULL && old.capacity > 0) {
        assert(storage.ptr != NULL);
        assert(*storage.ptr == 0);
    }

    /* the resulting buffer must still satisfy its invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
