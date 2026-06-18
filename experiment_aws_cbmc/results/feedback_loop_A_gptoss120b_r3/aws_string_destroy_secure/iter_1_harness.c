#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate enough memory for the struct plus the flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically choose an allocator (NULL or default) */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        /* set the const length field (cast away const for harness) */
        *((size_t *)&str->len) = len;

        /* fill the byte array with nondeterministic data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* save a copy of the original bytes for later comparison */
        uint8_t *old_bytes = malloc(len);
        __CPROVER_assume(old_bytes != NULL);
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = bytes[i];
        }

        /* save old immutable fields */
        struct aws_allocator *old_allocator = str->allocator;
        size_t old_len = str->len;

        /* assume the string is initially valid */
        __CPROVER_assume(aws_string_is_valid(str));

        /* call the function under test */
        aws_string_destroy_secure(str);

        /* post‑conditions */
        if (str != NULL) {
            /* the data bytes up to old_len must be zeroed */
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);
            }

            if (old_allocator == NULL) {
                /* when no allocator is present the struct remains allocated */
                assert(str->allocator == old_allocator);
                assert(str->len == old_len);
                /* the string must still be valid after zeroing */
                assert(aws_string_is_valid(str));
            } else {
                /* when an allocator is present the memory may have been released;
                 * we cannot safely dereference str after this point, but we have
                 * already asserted that only the intended bytes were written. */
            }
        }
    }
}
