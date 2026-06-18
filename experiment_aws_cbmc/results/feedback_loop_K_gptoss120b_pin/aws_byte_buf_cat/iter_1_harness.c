/*  
Preconditions:  
- dest != NULL and points to a valid aws_byte_buf (aws_byte_buf_is_valid(dest) == true).  
- number_of_args >= 2 && number_of_args <= 5.  
- For each i in [0, number_of_args): the i‑th argument is a non‑NULL pointer to a valid aws_byte_buf (aws_byte_buf_is_valid(buf_i) == true).  
- All buffers (dest and the source buffers) are allocated with aws_default_allocator() and their internal buffer pointers are either NULL (when capacity == 0) or point to a memory region of size at least capacity bytes.  

Postconditions (validity):  
- The function returns either AWS_OP_SUCCESS or AWS_OP_ERR.  
- dest remains a valid aws_byte_buf after the call (aws_byte_buf_is_valid(dest) == true).  

Postconditions (length):  
- If the return value is AWS_OP_SUCCESS then dest->len == old_dest_len + Σ_i src_i->len and dest->len <= dest->capacity.  
- If the return value is AWS_OP_ERR then dest->len is unchanged (still old_dest_len) and dest->capacity is unchanged.  

Postconditions (frame):  
- The internal buffers of the source aws_byte_buf objects are not modified.  
- No memory outside the allocated buffers (including allocator structures) is modified.  
*/

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_cat_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* destination buffer */
    struct aws_byte_buf dest;
    size_t dest_capacity = nondet_size_t();
    __CPROVER_assume(dest_capacity <= 1024);
    __CPROVER_assume(aws_byte_buf_init(&dest, alloc, dest_capacity) == AWS_OP_SUCCESS);
    /* remember old state */
    size_t old_dest_len = dest.len;
    size_t old_dest_capacity = dest.capacity;
    uint8_t *old_dest_buffer = dest.buffer;

    /* number of source arguments */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 2 && number_of_args <= 5);

    /* allocate array of source buffers */
    struct aws_byte_buf *src_bufs = malloc(number_of_args * sizeof(struct aws_byte_buf));
    __CPROVER_assume(src_bufs != NULL);

    /* initialize each source buffer */
    for (size_t i = 0; i < number_of_args; ++i) {
        size_t src_capacity = nondet_size_t();
        __CPROVER_assume(src_capacity <= 1024);
        __CPROVER_assume(aws_byte_buf_init(&src_bufs[i], alloc, src_capacity) == AWS_OP_SUCCESS);
        /* optionally set length to a value <= capacity */
        src_bufs[i].len = nondet_size_t();
        __CPROVER_assume(src_bufs[i].len <= src_bufs[i].capacity);
    }

    /* remember source lengths for post‑condition checks */
    size_t *src_lengths = malloc(number_of_args * sizeof(size_t));
    __CPROVER_assume(src_lengths != NULL);
    for (size_t i = 0; i < number_of_args; ++i) {
        src_lengths[i] = src_bufs[i].len;
    }

    /* call the function under test */
    int rc;
    {
        va_list ap;
        va_start(ap, number_of_args);
        /* Build a call using a helper macro to avoid varargs in CBMC */
        /* Instead we invoke the function directly with varargs via a wrapper */
        rc = aws_byte_buf_cat(&dest, number_of_args,
            &src_bufs[0],
            (number_of_args > 1 ? &src_bufs[1] : NULL),
            (number_of_args > 2 ? &src_bufs[2] : NULL),
            (number_of_args > 3 ? &src_bufs[3] : NULL),
            (number_of_args > 4 ? &src_bufs[4] : NULL));
        va_end(ap);
    }

    /* post‑condition: return value */
    assert(rc == AWS_OP_SUCCESS || rc == AWS_OP_ERR);

    /* post‑condition: dest validity */
    assert(aws_byte_buf_is_valid(&dest));

    /* post‑condition: length and capacity invariants */
    if (rc == AWS_OP_SUCCESS) {
        size_t total_src_len = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            total_src_len += src_lengths[i];
        }
        assert(dest.len == old_dest_len + total_src_len);
        assert(dest.len <= dest.capacity);
    } else {
        assert(dest.len == old_dest_len);
        assert(dest.capacity == old_dest_capacity);
        assert(dest.buffer == old_dest_buffer);
    }

    /* frame: source buffers unchanged */
    for (size_t i = 0; i < number_of_args; ++i) {
        assert(src_bufs[i].len == src_lengths[i]);
        assert(aws_byte_buf_is_valid(&src_bufs[i]));
    }

    /* clean up */
    aws_byte_buf_clean_up(&dest);
    for (size_t i = 0; i < number_of_args; ++i) {
        aws_byte_buf_clean_up(&src_bufs[i]);
    }
    free(src_bufs);
    free(src_lengths);

    return 0;
}
