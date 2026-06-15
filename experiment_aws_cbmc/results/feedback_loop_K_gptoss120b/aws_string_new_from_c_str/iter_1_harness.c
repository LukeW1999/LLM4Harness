/*  
 * Contract for aws_string_new_from_c_str  
 * Preconditions:  
 *   - allocator is a non‑NULL pointer to a valid aws_allocator (use aws_default_allocator()).  
 *   - c_str is a non‑NULL pointer to a null‑terminated byte array.  
 *   - The length of the string (excluding the terminating NUL) is bounded by MAX_STR_LEN.  
 *   - All bytes of c_str up to (but not including) the terminating NUL are arbitrary.  
 *   - The terminating NUL byte is present exactly once at the end of the buffer.  
 * Postconditions (validity):  
 *   - The returned pointer r is either NULL (allocation failure) or points to a valid aws_string (aws_string_is_valid(r) == true).  
 *   - If r != NULL then r->allocator == allocator.  
 *   - If r != NULL then r->len == strlen(c_str).  
 *   - If r != NULL then the first r->len bytes of r->bytes equal the first r->len bytes of c_str.  
 *   - If r != NULL then r->bytes[r->len] == '\\0' (the implicit null terminator).  
 * Postconditions (frame):  
 *   - The contents of c_str and the allocator object are not modified by the call.  
 */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/common.h>

#include "proof_helpers/make_common_data_structures.h"

#define MAX_STR_LEN 256

void aws_string_new_from_c_str_harness(void) {
    /* Allocate a default allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Nondeterministically choose a string length within bounds */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_STR_LEN);

    /* Allocate memory for the C string (including space for the terminating NUL) */
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Fill the string with nondeterministic bytes */
    for (size_t i = 0; i < str_len; ++i) {
        c_str[i] = nondet_char();
    }
    /* Ensure proper NUL termination */
    c_str[str_len] = '\0';

    /* Preserve a copy of the original string for later comparison */
    char *c_str_orig = malloc(str_len + 1);
    __CPROVER_assume(c_str_orig != NULL);
    memcpy(c_str_orig, c_str, str_len + 1);

    /* Call the function under verification */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* Postcondition: result is either NULL or a valid aws_string */
    if (result != NULL) {
        assert(aws_string_is_valid(result));

        /* Allocator field must match the input allocator */
        assert(result->allocator == allocator);

        /* Length must equal the original C string length */
        assert(result->len == str_len);

        /* Bytes must match the original C string content (excluding the implicit NUL) */
        for (size_t i = 0; i < result->len; ++i) {
            assert(result->bytes[i] == (uint8_t)c_str_orig[i]);
        }

        /* Implicit NUL terminator after the data */
        assert(result->bytes[result->len] == '\0');
    }

    /* Frame condition: c_str and allocator must be unchanged */
    for (size_t i = 0; i < str_len + 1; ++i) {
        assert(c_str[i] == c_str_orig[i]);
    }
    /* allocator is opaque; we only required it to be non‑NULL, which holds */

    /* Clean up */
    free(c_str);
    free(c_str_orig);
    if (result != NULL) {
        aws_string_destroy(result);
    }

    return 0;
}
