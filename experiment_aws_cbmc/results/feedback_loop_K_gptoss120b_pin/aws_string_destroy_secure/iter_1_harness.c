/*  
Preconditions:  
- str is either NULL or points to a valid aws_string structure.  
- If str != NULL, then aws_string_is_valid(str) holds.  
- The allocator field of the string may be NULL (to avoid deallocation in the proof) or a valid allocator.  
- The length field (str->len) is a nondeterministic size_t value, and the bytes array contains nondeterministic data.  

Postconditions (validity):  
- The function returns void; no return value to check.  

Postconditions (length / content):  
- If str != NULL and str->allocator == NULL, then all data bytes (str->bytes[0] … str->bytes[str->len‑1]) are zero after the call.  
- The struct fields (len, allocator) remain unchanged.  

Postconditions (frame):  
- No memory locations other than the bytes of the string (and the struct itself when allocator == NULL) are modified.  
- If str->allocator != NULL, the memory may be released; the proof does not inspect that case.  
*/

#include <aws/common/string.h>
#include <aws/common/memory.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    /* nondet pointer */
    str = (struct aws_string *)malloc(sizeof(struct aws_string));
    __CPROVER_assume(str != NULL);

    /* nondet length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 1024); /* bound for proof */

    /* allocate full object with flexible array */
    size_t total_size = sizeof(struct aws_string) + (len > 0 ? len - 1 : 0);
    str = (struct aws_string *)malloc(total_size);
    __CPROVER_assume(str != NULL);

    /* set fields */
    str->allocator = NULL;               /* choose NULL to avoid free */
    str->len = len;

    /* initialize bytes with nondet data */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    /* keep a copy of original bytes for later comparison */
    uint8_t *orig_bytes = malloc(len);
    __CPROVER_assume(orig_bytes != NULL);
    for (size_t i = 0; i < len; ++i) {
        orig_bytes[i] = ((uint8_t *)str->bytes)[i];
    }

    /* assume the string is valid according to the library's predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* call the function under test */
    aws_string_destroy_secure(str);

    /* post‑conditions */
    if (str != NULL && str->allocator == NULL) {
        /* bytes must be zeroed */
        for (size_t i = 0; i < len; ++i) {
            assert(((uint8_t *)str->bytes)[i] == 0);
        }
        /* struct fields unchanged */
        assert(str->len == len);
        assert(str->allocator == NULL);
    }

    /* clean up auxiliary memory */
    free(orig_bytes);
    free(str);
    return 0;
}
