Looking at the function `aws_byte_buf_clean_up_secure`, I need to analyze:

1. **What it does**: Calls `aws_byte_buf_secure_zero` (zeros all bytes and resets len to 0) then `aws_byte_buf_clean_up` (frees the buffer memory).

2. **What changes on success**: 
   - `buf->len` becomes 0 (from secure_zero)
   - `buf->buffer` becomes NULL (from clean_up)
   - `buf->capacity` becomes 0 (from clean_up)
   - `buf->allocator` becomes NULL (from clean_up)

3. **What's unchanged**: Nothing - all fields are modified.

4. **Failure**: This function returns void - no failure path.

5. **Validity invariants**: After clean_up, the buffer should still be valid (a zeroed/null buffer is valid).

Looking at `aws_byte_buf_clean_up` behavior: it frees the buffer and zeros out the struct fields. After `aws_byte_buf_secure_zero`, len=0 and buffer contents are zeroed. After `aws_byte_buf_clean_up`, buffer=NULL, capacity=0, len=0, allocator=NULL.

```c
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and set up the byte buffer */
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep verification tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state before calling */
    struct aws_byte_buf old = buf;

    /* 3. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Assert postconditions */
    /* After secure_zero + clean_up:
     * - len should be 0
     * - buffer should be NULL (freed and zeroed)
     * - capacity should be 0
     * - allocator should be NULL
     */
    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 5. Assert validity invariant - a null/zeroed buffer is valid */
    assert(aws_byte_buf_is_valid(&buf));
}
