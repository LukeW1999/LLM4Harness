/*  
 * Contract for aws_byte_buf_init_copy  
 * Preconditions:  
 *   - src points to a valid aws_byte_buf (aws_byte_buf_is_valid(src) == true)  
 *   - allocator is a valid pointer (may be NULL, but aws_default_allocator() will be used in harness)  
 *   - dest points to allocated memory for struct aws_byte_buf (but its contents are unrestricted)  
 * Postconditions (validity):  
 *   - The function returns either AWS_OP_SUCCESS or AWS_OP_ERR.  
 *   - If src->buffer == NULL, the function returns AWS_OP_SUCCESS, dest is zero‑initialized (all fields 0) and dest->allocator == allocator.  
 *   - If src->buffer != NULL and memory allocation succeeds, the function returns AWS_OP_SUCCESS, dest->buffer points to a newly allocated region of size src->capacity, dest->len == src->len, dest->capacity == src->capacity, dest->allocator == allocator, and the first src->len bytes of dest->buffer equal src->buffer.  
 *   - If src->buffer != NULL and memory allocation fails, the function returns AWS_OP_ERR and dest is zero‑initialized (all fields 0).  
 * Postconditions (frame):  
 *   - src and its buffer contents are unchanged.  
 *   - allocator is unchanged.  
 *   - No memory other than dest and the newly allocated dest->buffer (if any) is modified.  
 */

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* Allocate and nondeterministically initialize src */
    struct aws_byte_buf src;
    /* nondet allocator for src (not used by the function) */
    src.allocator = NULL;

    /* nondet capacity */
    size_t src_capacity = nondet_size_t();
    __CPROVER_assume(src_capacity <= SIZE_MAX / 2); /* avoid overflow in later ops */

    /* nondet buffer pointer */
    bool src_has_buffer = nondet_bool();
    if (src_has_buffer) {
        src.buffer = malloc(src_capacity);
        __CPROVER_assume(src.buffer != NULL);
        src.capacity = src_capacity;
        /* nondet length bounded by capacity */
        src.len = nondet_size_t();
        __CPROVER_assume(src.len <= src.capacity);
        /* initialize buffer contents */
        for (size_t i = 0; i < src.capacity; ++i) {
            src.buffer[i] = nondet_uint8_t();
        }
    } else {
        src.buffer = NULL;
        src.capacity = 0;
        src.len = 0;
    }

    /* Ensure src satisfies the library's validity predicate */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Backup src for frame condition */
    struct aws_byte_buf src_old = src;
    uint8_t *src_buffer_old = NULL;
    if (src.buffer) {
        src_buffer_old = malloc(src.capacity);
        __CPROVER_assume(src_buffer_old != NULL);
        memcpy(src_buffer_old, src.buffer, src.capacity);
    }

    /* Allocate dest (uninitialized) */
    struct aws_byte_buf dest;

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Call the function under verification */
    int ret = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postcondition: return value */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (src.buffer == NULL) {
        /* When source buffer is NULL, function must succeed and zero dest */
        assert(ret == AWS_OP_SUCCESS);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
    } else {
        if (ret == AWS_OP_SUCCESS) {
            /* Allocation succeeded */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == allocator);
            /* Verify contents are copied */
            for (size_t i = 0; i < src.len; ++i) {
                assert(dest.buffer[i] == src.buffer[i]);
            }
        } else {
            /* Allocation failed */
            assert(ret == AWS_OP_ERR);
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == NULL);
        }
    }

    /* Frame condition: src unchanged */
    assert(src.buffer == src_old.buffer);
    assert(src.len == src_old.len);
    assert(src.capacity == src_old.capacity);
    assert(src.allocator == src_old.allocator);
    if (src.buffer) {
        for (size_t i = 0; i < src.capacity; ++i) {
            assert(src.buffer[i] == src_buffer_old[i]);
        }
    }

    /* Clean up allocated memory */
    if (src.buffer) {
        free(src.buffer);
    }
    if (src_buffer_old) {
        free(src_buffer_old);
    }
    if (dest.buffer) {
        /* The function allocated this buffer with the allocator; free via allocator */
        aws_mem_release(allocator, dest.buffer);
    }

    return 0;
}
