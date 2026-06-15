#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_SIZE 256

int main(void) {
    /* 1. Declare and allocate a possibly NULL aws_string */
    struct aws_string *str;
    if (nondet_bool()) {
        /* Allocate a string with a nondeterministic length bounded by MAX_STRING_SIZE */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_SIZE);
        /* Allocate enough space for the flexible array member */
        str = malloc(sizeof(struct aws_string) + (len == 0 ? 0 : len - 1));
        __CPROVER_assume(str != NULL);

        /* Nondeterministically choose an allocator (NULL or default) */
        struct aws_allocator *alloc = nondet_bool() ? aws_default_allocator() : NULL;
        /* Initialize fields */
        *((struct aws_allocator *const *)(&str->allocator)) = alloc;   /* const field */
        *((size_t const *)(&str->len)) = len;                         /* const field */

        /* Fill the byte array with nondeterministic data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* 2. Save old state for the case where the string is not freed */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (str != NULL && str->allocator == NULL) {
        old = *str; /* copies allocator, len, and the first byte of the flexible array */
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    /* 3. Call function under test */
    aws_string_destroy_secure(str);

    /* 4. Post‑condition checks */
    if (str != NULL) {
        if (str->allocator == NULL) {
            /* The string was not freed; its bytes must be zeroed */
            for (size_t i = 0; i < old.len; ++i) {
                assert(((uint8_t *)aws_string_bytes(str))[i] == 0);
            }
            /* Unchanged fields */
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            /* Validity must still hold (zeroed bytes are still a valid string) */
            assert(aws_string_is_valid(str));
        } else {
            /* The string was freed; we cannot dereference it. No further assertions. */
        }
    } else {
        /* str was NULL – nothing to check */
    }

    return 0;
}
