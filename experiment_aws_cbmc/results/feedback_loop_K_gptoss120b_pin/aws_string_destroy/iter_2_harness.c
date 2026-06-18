#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_harness(void) {
    /* nondeterministic choice: whether str is NULL */
    struct aws_string *str;
    bool is_null = nondet_bool();
    __CPROVER_assume(is_null == true || is_null == false);

    struct aws_allocator *orig_allocator = NULL;
    size_t orig_len = 0;

    if (!is_null) {
        /* allocate memory for the string (including flexible array and null terminator) */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 256);               /* keep allocation bounded */
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);              /* allocation must succeed for this path */

        /* nondeterministically decide if allocator is NULL */
        bool allocator_is_null = nondet_bool();
        __CPROVER_assume(allocator_is_null == true || allocator_is_null == false);

        if (allocator_is_null) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }
        orig_allocator = str->allocator;
        str->len = len;
        orig_len = len;

        /* initialize the bytes (including the mandatory null terminator) */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        ((uint8_t *)str->bytes)[len] = 0; /* null terminator */

        /* ensure the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* Call the function under test */
    aws_string_destroy(str);

    /* Post‑condition checks */
    if (str != NULL && orig_allocator != NULL) {
        /* When allocator is non‑NULL, the memory is freed; no further checks needed. */
    } else {
        /* When no free occurs, the string (if any) must remain unchanged */
        if (str != NULL) {
            __CPROVER_assert(str->allocator == orig_allocator,
                             "allocator unchanged when not freed");
            __CPROVER_assert(str->len == orig_len,
                             "length unchanged when not freed");
            for (size_t i = 0; i < orig_len; ++i) {
                __CPROVER_assert(((uint8_t *)str->bytes)[i] == ((uint8_t *)str->bytes)[i],
                                 "bytes unchanged when not freed");
            }
        }
    }
}
