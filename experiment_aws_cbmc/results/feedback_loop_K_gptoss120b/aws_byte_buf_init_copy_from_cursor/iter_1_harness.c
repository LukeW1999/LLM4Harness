/*  
 * Contract for aws_byte_buf_init_copy_from_cursor  
 * ------------------------------------------------  
 * Preconditions:  
 *   - allocator != NULL (aws_default_allocator())  
 *   - src.len is a nondeterministic size_t, bounded (e.g., 0 <= src.len <= MAX_LEN)  
 *   - if src.len > 0 then src.ptr points to a valid memory region of at least src.len bytes  
 *   - aws_byte_cursor_is_valid(&src) holds (i.e., src.ptr != NULL when src.len > 0)  
 *   - dest points to a valid, writable struct aws_byte_buf (stack‑allocated)  
 *   - No other global state is required.  
 *  
 * Postconditions (validity):  
 *   - The function returns either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1).  
 *   - dest.allocator == allocator.  
 *   - dest.len == src.len.  
 *   - dest.capacity == src.len.  
 *   - If src.len == 0 then dest.buffer == NULL.  
 *   - If src.len > 0 and the call returns AWS_OP_SUCCESS then dest.buffer != NULL and the memory region  
 *     dest.buffer[0..src.len-1] contains exactly the bytes from src.ptr[0..src.len-1].  
 *   - If src.len > 0 and the call returns AWS_OP_ERR then dest.buffer == NULL (allocation failed).  
 *  
 * Postconditions (frame):  
 *   - The source cursor (src) and its underlying memory are unchanged.  
 *   - The allocator object is unchanged.  
 *   - No other memory outside of dest and the newly allocated buffer (if any) is modified.  
 */  

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_LEN 1024

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* destination buffer (stack allocated) */
    struct aws_byte_buf dest;
    /* ensure dest is a valid writable object */
    __CPROVER_assume(&dest != NULL);

    /* source cursor */
    struct aws_byte_cursor src;
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= MAX_LEN);

    if (src.len > 0) {
        src.ptr = (uint8_t *)malloc(src.len);
        __CPROVER_assume(src.ptr != NULL);
        /* nondet contents */
        for (size_t i = 0; i < src.len; ++i) {
            src.ptr[i] = nondet_uint8_t();
        }
    } else {
        src.ptr = NULL;
    }

    /* Assume the cursor is valid */
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save copies of inputs for post‑condition checks */
    struct aws_byte_cursor src_old = src;
    struct aws_allocator *allocator_old = allocator;

    /* Call the function under test */
    int ret = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* ---- Postcondition checks ---- */

    /* Return value must be either success or error */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* Allocator must be propagated */
    assert(dest.allocator == allocator_old);

    /* Length and capacity must equal source length */
    assert(dest.len == src_old.len);
    assert(dest.capacity == src_old.len);

    if (src_old.len == 0) {
        /* Zero‑length source yields NULL buffer */
        assert(dest.buffer == NULL);
    } else {
        if (ret == AWS_OP_SUCCESS) {
            /* Successful allocation: buffer must be non‑NULL and contain a copy */
            assert(dest.buffer != NULL);
            for (size_t i = 0; i < src_old.len; ++i) {
                assert(dest.buffer[i] == src_old.ptr[i]);
            }
        } else {
            /* Allocation failed: buffer must be NULL */
            assert(dest.buffer == NULL);
        }
    }

    /* Source cursor and its memory must be unchanged */
    assert(src.len == src_old.len);
    assert(src.ptr == src_old.ptr);
    if (src.len > 0) {
        for (size_t i = 0; i < src.len; ++i) {
            assert(src.ptr[i] == src_old.ptr[i]);
        }
    }

    /* Allocator object must be unchanged */
    assert(allocator == allocator_old);

    return 0;
}
