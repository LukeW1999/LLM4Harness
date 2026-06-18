/*  
Preconditions:  
- `dest` is a non‑NULL pointer to an `aws_byte_buf` (the function zeroes it).  
- `allocator` is a valid pointer (we use `aws_default_allocator()`).  
- `src` satisfies `aws_byte_cursor_is_valid(&src)`.  
  * If `src.len == 0` then `src.ptr` may be NULL.  
  * If `src.len > 0` then `src.ptr` is a non‑NULL pointer to at least `src.len` bytes.  

Postconditions (validity):  
- The function returns `AWS_OP_SUCCESS` (0) on success, otherwise `AWS_OP_ERR` (non‑zero).  
- On success:  
  * `dest->allocator == allocator`.  
  * `dest->len == src.len`.  
  * `dest->capacity == src.len`.  
  * If `src.len == 0` then `dest->buffer == NULL`.  
  * If `src.len > 0` then `dest->buffer` points to a newly allocated region of exactly `src.len` bytes and the contents are identical to the source (`memcmp(dest->buffer, src.ptr, src.len) == 0`).  
- On error (allocation failure when `src.len > 0`):  
  * The function returns `AWS_OP_ERR`.  
  * `dest` remains zero‑initialized (all fields zero or NULL).  

Postconditions (frame):  
- The caller‑provided `src` and `allocator` are not modified by the function.  
- No memory outside the newly allocated buffer is written.  
*/

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* Allocate destination on the stack */
    struct aws_byte_buf dest;
    /* Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Nondeterministic source cursor */
    struct aws_byte_cursor src;
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= 1024); /* bound to keep allocation reasonable */

    if (src.len > 0) {
        src.ptr = malloc(src.len);
        __CPROVER_assume(src.ptr != NULL);
        /* Fill with nondeterministic data */
        for (size_t i = 0; i < src.len; ++i) {
            ((uint8_t *)src.ptr)[i] = nondet_uint8_t();
        }
    } else {
        src.ptr = NULL;
    }

    /* Assume the cursor is valid */
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Preserve a copy of src for frame checking */
    struct aws_byte_cursor src_old = src;

    /* Call the function under test */
    int ret = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* Postcondition checks */
    if (src.len == 0) {
        assert(ret == AWS_OP_SUCCESS);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == alloc);
    } else {
        /* In this harness we assume allocation succeeds, therefore success is expected */
        assert(ret == AWS_OP_SUCCESS);
        assert(dest.buffer != NULL);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == alloc);
        /* Verify the copy */
        assert(memcmp(dest.buffer, src.ptr, src.len) == 0);
    }

    /* Frame conditions: src and allocator must be unchanged */
    assert(src.ptr == src_old.ptr);
    assert(src.len == src_old.len);
    assert(alloc == aws_default_allocator());

    /* Clean up allocated memory */
    if (dest.buffer != NULL) {
        aws_mem_release(alloc, dest.buffer);
    }
    if (src.ptr != NULL) {
        free(src.ptr);
    }

    return 0;
}
