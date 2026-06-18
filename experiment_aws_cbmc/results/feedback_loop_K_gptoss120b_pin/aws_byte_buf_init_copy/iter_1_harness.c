/*  
 * Contract for aws_byte_buf_init_copy  
 * ------------------------------------------------------------  
 * Preconditions:  
 *   - dest != NULL (the pointer passed to the function)  
 *   - allocator != NULL (a valid aws_allocator, here aws_default_allocator())  
 *   - src != NULL and aws_byte_buf_is_valid(src) holds  
 *   - If src->buffer != NULL then src->capacity >= src->len  
 *   - All memory referenced by src (including src->buffer when non‑NULL) is
 *     initialized.  
 *   
 * Postconditions (validity):  
 *   - The function returns either AWS_OP_SUCCESS (0) or AWS_OP_ERR (non‑zero).  
 *   - On success: dest is a valid aws_byte_buf (aws_byte_buf_is_valid(&dest) == true).  
 *   - On error: dest is zero‑initialized (all fields 0) and dest->allocator == NULL.  
 *   
 * Postconditions (length & capacity):  
 *   - If src->buffer == NULL: dest->len == 0, dest->capacity == 0, dest->buffer == NULL.  
 *   - If src->buffer != NULL and allocation succeeds:  
 *       dest->len == src->len, dest->capacity == src->capacity,  
 *       dest->buffer points to a newly allocated region of size src->capacity,  
 *       and the first src->len bytes of dest->buffer equal src->buffer.  
 *   - In all cases dest->allocator == allocator.  
 *   
 * Postconditions (frame):  
 *   - No memory outside of dest, the newly allocated buffer (if any), and
 *     the allocator is modified.  
 *   - src and its buffer remain unchanged.  
 */  

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
bool nondet_bool(void);

void aws_byte_buf_init_copy_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Allocate and nondeterministically initialize src */
    struct aws_byte_buf *src = malloc(sizeof(*src));
    __CPROVER_assume(src != NULL);

    src->len = nondet_size_t();
    src->capacity = nondet_size_t();
    __CPROVER_assume(src->capacity >= src->len);
    src->allocator = allocator;

    if (nondet_bool()) {
        src->buffer = NULL;
    } else {
        src->buffer = malloc(src->capacity);
        __CPROVER_assume(src->buffer != NULL);
        for (size_t i = 0; i < src->capacity; ++i) {
            src->buffer[i] = nondet_uint8_t();
        }
    }

    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* Call the function under test */
    int ret = aws_byte_buf_init_copy(&dest, allocator, src);

    /* Postcondition: return value */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* Postcondition: allocator field */
    assert(dest.allocator == allocator);

    if (src->buffer == NULL) {
        /* When source buffer is NULL the function must succeed and zero dest */
        assert(ret == AWS_OP_SUCCESS);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
    } else {
        if (ret == AWS_OP_SUCCESS) {
            /* Successful allocation */
            assert(aws_byte_buf_is_valid(&dest));
            assert(dest.len == src->len);
            assert(dest.capacity == src->capacity);
            assert(dest.buffer != NULL);
            /* Verify content copy */
            for (size_t i = 0; i < src->len; ++i) {
                assert(dest.buffer[i] == src->buffer[i]);
            }
        } else {
            /* Allocation failed: dest must be zeroed */
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
        }
    }

    /* Frame condition: src must remain unchanged */
    assert(src->len == src->len);
    assert(src->capacity == src->capacity);
    assert(src->allocator == allocator);
    if (src->buffer != NULL) {
        for (size_t i = 0; i < src->capacity; ++i) {
            assert(src->buffer[i] == src->buffer[i]);
        }
    }

    return 0;
}
