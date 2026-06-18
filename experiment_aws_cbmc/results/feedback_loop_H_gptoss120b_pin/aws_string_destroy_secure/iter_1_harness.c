#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate a string (or leave it NULL) */
    if (nondet_bool()) {
        /* Allocate enough space for the struct plus the flexible array */
        size_t total = sizeof(struct aws_string) + (len > 0 ? len - 1 : 0);
        str = (struct aws_string *)malloc(total);
        if (str) {
            /* nondet allocator: either NULL (static) or the default allocator */
            str->allocator = nondet_bool() ? NULL : aws_default_allocator();
            str->len = len;

            /* Fill the byte array with nondet data */
            uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
            for (size_t i = 0; i < len; ++i) {
                bytes[i] = nondet_uint8_t();
            }

            /* Assume the string is valid according to the library predicate */
            __CPROVER_assume(aws_string_is_valid(str));
        }
    } else {
        str = NULL;
    }

    /* Save old state when the pointer is non‑NULL */
    struct aws_string old;
    if (str) {
        old = *str; /* copies allocator and len (bytes are flexible) */
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Post‑conditions */
    if (str) {
        /* If the string was statically allocated (allocator == NULL) the
         * memory is not released, so we can observe the zeroing. */
        if (str->allocator == NULL) {
            /* All data bytes must be zero after the call */
            uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
            for (size_t i = 0; i < old.len; ++i) {
                assert(bytes[i] == 0);
            }

            /* Unchanged fields */
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
        } else {
            /* When allocator is non‑NULL the memory may have been released.
             * The function does not modify any observable fields before the
             * release, so we assert that the allocator field (read before the
             * free) is unchanged.  Accessing the struct after free would be
             * undefined, so we avoid further dereferencing. */
            assert(str->allocator == old.allocator);
        }

        /* Validity invariant must still hold (for the static case) */
        if (str->allocator == NULL) {
            assert(aws_string_is_valid(str));
        }
    } else {
        /* str == NULL: nothing to assert, the function is a no‑op */
    }
}
