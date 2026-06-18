/*  
 * Contract for aws_byte_cursor_from_array  
 * Preconditions:  
 *   - len is any size_t value.  
 *   - If len > 0, bytes must point to a valid memory region of at least len bytes.  
 *   - If len == 0, bytes may be NULL.  
 * Postconditions (validity):  
 *   - The returned aws_byte_cursor has ptr equal to (uint8_t *)bytes.  
 *   - The returned aws_byte_cursor has len equal to the input len.  
 *   - No error codes are produced (function cannot fail).  
 * Postconditions (frame):  
 *   - The function does not modify any memory; the contents of the buffer pointed to by bytes remain unchanged.  
 */

#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness(void) {
    /* nondeterministic length */
    size_t len = nondet_size_t();
    /* limit length to avoid excessive allocation in CBMC */
    __CPROVER_assume(len <= 1024);

    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
        /* make the allocated region fresh */
        __CPROVER_assume(__CPROVER_is_fresh(bytes, len));
        /* optionally initialize with nondet data */
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    } else {
        /* when len == 0, bytes may be NULL */
        bytes = NULL;
    }

    /* Preserve a copy of the original buffer to check the frame condition */
    uint8_t *bytes_copy = NULL;
    if (len > 0) {
        bytes_copy = malloc(len);
        __CPROVER_assume(bytes_copy != NULL);
        memcpy(bytes_copy, bytes, len);
    }

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* Postcondition checks */
    assert(cur.len == len);
    assert(cur.ptr == (uint8_t *)bytes);

    /* Frame condition: input buffer must be unchanged */
    if (len > 0) {
        assert(memcmp(bytes, bytes_copy, len) == 0);
    }

    /* Clean up */
    if (bytes) free(bytes);
    if (bytes_copy) free(bytes_copy);

    return 0;
}
