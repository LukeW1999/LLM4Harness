#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <stdbool.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_harness(void) {
    /* nondeterministic choice: whether str is NULL */
    struct aws_string *str;
    bool is_null = nondet_bool();
    __CPROVER_assume(is_null == true || is_null == false);

    struct aws_allocator *orig_allocator = NULL;
    size_t orig_len = 0;

    if (!is_null) {
        /* nondeterministic length, bounded */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 256);

        /* allocate a buffer for the string contents */
        uint8_t *buf = (uint8_t *)malloc(len);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_uint8_t();
        }

        /* create a valid aws_string using the default allocator */
        str = aws_string_new_from_array(aws_default_allocator(), buf, len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        /* nondeterministically decide if the allocator field is NULL */
        bool allocator_is_null = nondet_bool();
        __CPROVER_assume(allocator_is_null == true || allocator_is_null == false);
        if (allocator_is_null) {
            str->allocator = NULL;
        }

        orig_allocator = str->allocator;
        orig_len = str->len;

        free(buf);
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
