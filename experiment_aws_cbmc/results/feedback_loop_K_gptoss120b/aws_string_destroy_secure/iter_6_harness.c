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
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    /* nondeterministic length */
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* nondeterministic choice of having an allocator */
    bool has_allocator = nondet_bool();
    const struct aws_allocator *allocator = has_allocator ? aws_default_allocator()
                                                          : NULL;

    /* stack‑allocated wrapper to provide space for the flexible array member */
    struct {
        struct aws_string s;
        char bytes[MAX_STRING_LEN];
    } wrapper;

    /* initialize the string fields */
    wrapper.s.allocator = allocator;
    wrapper.s.len = len;

    /* assume the string is valid according to the library's invariant */
    __CPROVER_assume(aws_string_is_valid(&wrapper.s));

    /* call the function under verification */
    aws_string_destroy_secure(&wrapper.s);

    /* post‑condition checks */
    if (allocator != NULL) {
        /* When an allocator is present the memory is released.
           The object must not be accessed after this point. */
    } else {
        /* When allocator is NULL the string is static; it must remain valid and unchanged. */
        assert(wrapper.s.allocator == NULL);
        assert(wrapper.s.len == len);
    }
}
