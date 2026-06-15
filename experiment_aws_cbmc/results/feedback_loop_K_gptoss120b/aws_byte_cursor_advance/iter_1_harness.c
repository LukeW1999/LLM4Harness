/*  
 * Contract for aws_byte_cursor_advance  
 * Preconditions:  
 *   - cursor is a valid aws_byte_cursor (aws_byte_cursor_is_valid(cursor))  
 *   - cursor->len may be any size_t value (including 0) but must satisfy the validity condition  
 *   - len (the advance amount) is an arbitrary size_t (nondet)  
 * Postconditions (validity):  
 *   - The returned aws_byte_cursor (rv) is valid (aws_byte_cursor_is_valid(&rv))  
 *   - The input cursor remains valid after the call (aws_byte_cursor_is_valid(cursor))  
 * Postconditions (length and pointer):  
 *   - If cursor->len > (SIZE_MAX>>1) OR len > (SIZE_MAX>>1) OR len > cursor->len  
 *       then rv.ptr == NULL, rv.len == 0 and cursor is unchanged.  
 *   - Otherwise (successful advance)  
 *       rv.ptr == original cursor->ptr, rv.len == len,  
 *       cursor->ptr == original_ptr + len (or NULL if original_ptr was NULL),  
 *       cursor->len == original_len - len.  
 * Postconditions (frame):  
 *   - No memory other than the fields of *cursor and the returned struct is modified.  
 */

#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_advance_harness(void) {
    /* Set up a nondeterministic buffer length */
    size_t buf_len = nondet_size_t();
    __CPROVER_assume(buf_len <= 1024); /* limit for tractability */

    /* Allocate buffer if length > 0 */
    uint8_t *buf = NULL;
    if (buf_len > 0) {
        buf = malloc(buf_len);
        __CPROVER_assume(buf != NULL);
    }

    /* Initialize cursor */
    struct aws_byte_cursor cursor;
    cursor.ptr = buf;
    cursor.len = buf_len;
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Nondeterministic advance length */
    size_t adv_len = nondet_size_t();

    /* Save original state */
    uint8_t *old_ptr = cursor.ptr;
    size_t old_len = cursor.len;

    /* Call the function under test */
    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, adv_len);

    /* Postcondition: returned cursor is valid */
    __CPROVER_assert(aws_byte_cursor_is_valid(&rv), "Returned cursor must be valid");

    /* Postcondition: original cursor remains valid */
    __CPROVER_assert(aws_byte_cursor_is_valid(&cursor), "Original cursor must be valid after call");

    /* Determine expected outcome based on preconditions */
    if (old_len > (SIZE_MAX >> 1) || adv_len > (SIZE_MAX >> 1) || adv_len > old_len) {
        /* Failure case: no advancement */
        __CPROVER_assert(rv.ptr == NULL && rv.len == 0,
                         "Failure case: returned cursor should be empty");
        __CPROVER_assert(cursor.ptr == old_ptr && cursor.len == old_len,
                         "Failure case: original cursor must be unchanged");
    } else {
        /* Success case: advancement performed */
        __CPROVER_assert(rv.ptr == old_ptr,
                         "Success case: returned cursor ptr must equal original ptr");
        __CPROVER_assert(rv.len == adv_len,
                         "Success case: returned cursor len must equal advance length");
        __CPROVER_assert(cursor.ptr == (old_ptr == NULL ? NULL : old_ptr + adv_len),
                         "Success case: original cursor ptr must be advanced");
        __CPROVER_assert(cursor.len == old_len - adv_len,
                         "Success case: original cursor len must be decreased by advance length");
    }

    /* No other memory should be modified; the buffer contents remain unchanged */
    /* (The function does not write to the buffer, so we do not need explicit checks here) */

    return 0;
}
