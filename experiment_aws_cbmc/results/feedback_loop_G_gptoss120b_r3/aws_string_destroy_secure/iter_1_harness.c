#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    /* nondeterministically decide whether to have a string or NULL */
    if (nondet_bool()) {
        /* allocate a string with a bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_SIZE);

        /* allocate enough memory for the struct plus the flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL); /* for this branch we need a valid allocation */

        /* nondeterministically choose an allocator (NULL = static string) */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* set length */
        *((size_t *)&str->len) = len; /* cast away const for initialization */

        /* fill the bytes with nondeterministic data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* save old state for later comparison */
        uint8_t *old_bytes = malloc(len);
        __CPROVER_assume(old_bytes != NULL);
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = bytes[i];
        }
        size_t old_len = len;
        struct aws_allocator *old_allocator = str->allocator;

        /* call the function under test */
        aws_string_destroy_secure(str);

        /* post‑conditions */
        if (old_allocator == NULL) {
            /* string was not freed – bytes must be zeroed */
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);
            }
            /* length and allocator must be unchanged */
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
            /* validity invariant must still hold */
            assert(aws_string_is_valid(str));
        } else {
            /* string was freed – we must not dereference it any more.
               The only thing we can assert is that we did not corrupt memory
               outside the freed object, which is ensured by the absence of
               further accesses. */
        }

        free(old_bytes);
    } else {
        /* str is NULL */
        str = NULL;
        aws_string_destroy_secure(str);
        /* nothing to assert – function should be a no‑op */
    }
}
