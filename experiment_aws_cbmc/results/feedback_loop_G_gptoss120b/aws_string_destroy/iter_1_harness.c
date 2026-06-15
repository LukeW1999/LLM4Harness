#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "aws/common/string.h"
#include "aws/common/byte_buf.h"
#include "aws/common/byte_cursor.h"
#include "aws/common/allocator.h"
#include "proof_helpers/make_common_data_structures.h"

/* Bounding constant for the length of the string data */
#define MAX_STRING_LEN 64

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    /* 1. Non‑deterministically decide whether we have a string */
    if (nondet_bool()) {
        /* Allocate a string with a flexible array member.
         * The allocation size is sizeof(struct aws_string) + (len-1) because
         * the struct already contains one byte in the flexible array. */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* 2. Non‑deterministically decide whether the allocator is present */
        if (nondet_bool()) {
            struct aws_allocator *alloc = malloc(sizeof(struct aws_allocator));
            __CPROVER_assume(alloc != NULL);
            /* allocator field is const, so we need a cast to assign */
            *((struct aws_allocator * const *) &str->allocator) = alloc;
        } else {
            *((struct aws_allocator * const *) &str->allocator) = NULL;
        }

        /* 3. Initialise the length field (also const) */
        *((size_t const *) &str->len) = len;

        /* 4. Initialise the byte data */
        uint8_t *bytes = (uint8_t *)str->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* 5. Assume the string is valid before the call */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* 6. Save old state for comparison when the string is not freed */
    struct aws_string *old_str = str;

    /* 7. Call the function under test */
    aws_string_destroy(str);

    /* 8. Post‑conditions */

    /* If the string was NULL or its allocator was NULL, the function must not
     * modify any observable fields. */
    if (str == NULL || (str != NULL && str->allocator == NULL)) {
        if (str != NULL) {
            /* allocator unchanged */
            assert(str->allocator == old_str->allocator);
            /* length unchanged */
            assert(str->len == old_str->len);
            /* bytes unchanged */
            assert_bytes_match(str->bytes, old_str->bytes, str->len);
        }
        /* The string must still satisfy its validity predicate */
        if (str != NULL) {
            assert(aws_string_is_valid(str));
        }
    } else {
        /* When str != NULL && str->allocator != NULL the memory is released.
         * The pointer may now be dangling; we must not dereference it.
         * No further assertions are required because the function does not
         * modify any other observable state. */
    }
}
