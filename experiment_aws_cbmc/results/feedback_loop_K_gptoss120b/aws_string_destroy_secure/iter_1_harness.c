/*  
Preconditions:  
- `str` may be NULL or a pointer to a valid `struct aws_string`.  
- If `str != NULL` then `aws_string_is_valid(str)` holds.  
- If `str != NULL` then `str->len` is a size_t value and the allocation for the flexible array member provides at least `str->len` bytes.  
- `str->allocator` may be NULL or a pointer to a valid allocator (we use `aws_default_allocator()` when non‑NULL).  

Postconditions (validity):  
- The function returns `void`.  
- If `str != NULL` and `str->allocator != NULL` the memory for the string is released (cannot be directly observed, but the pointer must not be dereferenced after the call).  
- If `str != NULL` and `str->allocator == NULL` the string memory is not freed and remains a valid static string.  

Postconditions (frame):  
- No memory outside the `aws_string` object (including the allocator) is modified.  
*/

#include <aws/common/string.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    /* nondeterministic length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* nondeterministic choice of having an allocator */
    bool has_allocator = nondet_bool();
    struct aws_allocator *allocator = has_allocator ? aws_default_allocator() : NULL;

    /* allocate memory for the string structure plus flexible array */
    struct aws_string *str = malloc(sizeof(struct aws_string) + (len == 0 ? 0 : len - 1));
    __CPROVER_assume(str != NULL);

    /* initialize the string fields */
    str->allocator = allocator;
    str->len = len;
    /* the bytes array is uninitialized; its content is irrelevant for the proof */

    /* assume the string is valid according to the library's invariant */
    __CPROVER_assume(aws_string_is_valid(str));

    /* call the function under verification */
    aws_string_destroy_secure(str);

    /* post‑condition checks */
    if (str != NULL) {
        if (allocator != NULL) {
            /* When an allocator is present the memory is released.
               We cannot dereference `str` after the call, so we only
               check that we do not access it. */
            /* No further assertions needed; CBMC will catch any illegal
               accesses if the implementation incorrectly uses `str` after
               freeing it. */
        } else {
            /* When allocator is NULL the string is static; it must remain
               valid and unchanged. */
            assert(str->allocator == NULL);
            assert(str->len == len);
        }
    }

    return 0;
}
