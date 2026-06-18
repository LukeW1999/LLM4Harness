/*  
 * Contract for aws_string_bytes  
 * Preconditions:  
 *   - __CPROVER_assume(str != NULL);  
 *   - __CPROVER_assume(aws_string_is_valid(str));  
 *   - __CPROVER_assume(str->len <= MAX_STRING_LEN);  // optional bound for verification  
 * Postconditions (validity):  
 *   - The returned pointer is non‑NULL.  
 *   - The returned pointer is equal to str->bytes.  
 *   - The memory region [bytes, bytes + str->len) is readable.  
 * Postconditions (frame):  
 *   - No memory locations other than those possibly read by the function are modified.  
 *   - The fields of *str (allocator, len, bytes) remain unchanged.  
 */

#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* Allocate a default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Create a nondeterministic length for the string */
    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate a nondeterministic buffer for the string data */
    uint8_t *data = (uint8_t *)malloc(len);
    __CPROVER_assume(data != NULL);
    /* Fill the buffer with nondeterministic bytes */
    for (size_t i = 0; i < len; ++i) {
        data[i] = (uint8_t)__CPROVER_nondet_uint();
    }

    /* Construct an aws_string from the buffer */
    struct aws_string *str = aws_string_new_from_array(allocator, data, len);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Preserve original values for frame checking */
    const struct aws_allocator *orig_allocator = str->allocator;
    const size_t orig_len = str->len;
    const uint8_t *orig_bytes = str->bytes;

    /* Call the function under test */
    const uint8_t *bytes = aws_string_bytes(str);

    /* Postcondition checks */
    assert(bytes != NULL);
    assert(bytes == orig_bytes);
    /* The bytes region must be readable for str->len bytes */
    for (size_t i = 0; i < str->len; ++i) {
        (void)bytes[i];
    }

    /* Frame checks: the string structure must remain unchanged */
    assert(str->allocator == orig_allocator);
    assert(str->len == orig_len);
    assert(str->bytes == orig_bytes);

    /* Clean up */
    aws_string_destroy(str);
    free(data);

    return 0;
}
