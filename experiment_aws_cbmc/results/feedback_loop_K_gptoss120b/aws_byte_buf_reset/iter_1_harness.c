/*  
 * Contract for aws_byte_buf_reset  
 * --------------------------------  
 * Preconditions:  
 *   - __CPROVER_assume(buf != NULL);  
 *   - __CPROVER_assume(aws_byte_buf_is_valid(buf));   // buffer, len, capacity, allocator are well‑formed  
 *   - __CPROVER_assume(zero_contents == true || zero_contents == false);  
 *   - The memory pointed to by buf->buffer (if any) is readable/writable for buf->capacity bytes.  
 *   - The allocator stored in buf->allocator is a valid pointer (aws_default_allocator()).  
 *   
 * Postconditions (validity):  
 *   - aws_byte_buf_is_valid(buf) holds after the call.  
 *   - buf->allocator is unchanged.  
 *   - buf->capacity is unchanged.  
 *   
 * Postconditions (length):  
 *   - buf->len == 0.  
 *   
 * Postconditions (contents):  
 *   - If zero_contents is true and buf->capacity > 0, every byte in buf->buffer[0 .. buf->capacity‑1] is 0.  
 *   - If zero_contents is false, the contents of buf->buffer are unchanged.  
 *   
 * Frame conditions:  
 *   - No memory outside of buf and its internal buffer is modified.  
 */  

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_CAPACITY 64

void aws_byte_buf_reset_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    struct aws_byte_buf buf;
    size_t capacity;

    /* Choose a nondeterministic capacity within a reasonable bound */
    capacity = __CPROVER_nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    /* Allocate the internal buffer (or leave it NULL if capacity == 0) */
    if (capacity == 0) {
        buf.buffer = NULL;
    } else {
        buf.buffer = malloc(capacity);
        __CPROVER_assume(buf.buffer != NULL);
    }

    buf.capacity = capacity;

    /* Choose a nondeterministic length that does not exceed capacity */
    buf.len = __CPROVER_nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Fill the buffer with nondeterministic data */
    for (size_t i = 0; i < capacity; ++i) {
        ((uint8_t *)buf.buffer)[i] = __CPROVER_nondet_uint8_t();
    }

    buf.allocator = alloc;

    /* Ensure the pre‑condition holds */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Snapshot the original state */
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_contents = NULL;
    if (capacity > 0) {
        old_contents = malloc(capacity);
        __CPROVER_assume(old_contents != NULL);
        memcpy(old_contents, buf.buffer, capacity);
    }

    bool zero_contents = __CPROVER_nondet_bool();

    /* Call the function under verification */
    aws_byte_buf_reset(&buf, zero_contents);

    /* ---- Post‑condition checks ---- */

    /* Validity */
    assert(aws_byte_buf_is_valid(&buf));

    /* Length and capacity invariants */
    assert(buf.len == 0);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Content checks */
    if (zero_contents && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(((uint8_t *)buf.buffer)[i] == 0);
        }
    } else {
        if (buf.capacity > 0) {
            for (size_t i = 0; i < buf.capacity; ++i) {
                assert(((uint8_t *)buf.buffer)[i] == old_contents[i]);
            }
        }
    }

    /* No other memory should have been modified (allocator, capacity, len already checked) */
    /* The only other allocated object is old_contents, which is not touched by the function. */
    return 0;
}
