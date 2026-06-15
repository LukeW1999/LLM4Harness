#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    /* 1. Declare and nondeterministically allocate a string (or NULL) */
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate space for the struct plus flexible array */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* nondeterministically choose an allocator (or NULL) */
        if (nondet_bool()) {
            str->allocator = malloc(sizeof(struct aws_allocator));
            __CPROVER_assume(str->allocator != NULL);
        } else {
            str->allocator = NULL;
        }

        /* set length (const field) */
        *((size_t *)&str->len) = len;

        /* initialize the byte payload with nondet values */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Save old state before the call */
    struct aws_string *old_str = str;
    size_t old_len = (str) ? str->len : 0;
    uint8_t *old_bytes = NULL;
    if (str) {
        old_bytes = malloc(old_len);
        __CPROVER_assume(old_bytes != NULL || old_len == 0);
        for (size_t i = 0; i < old_len; ++i) {
            old_bytes[i] = ((uint8_t *)aws_string_bytes(str))[i];
        }
    }

    /* 3. Call the function under test */
    aws_string_destroy_secure(str);

    /* 4. Post‑condition checks */

    /* The pointer itself must remain unchanged */
    assert(str == old_str);

    if (old_str) {
        if (old_str->allocator == NULL) {
            /* Memory must be zeroed when no allocator is present */
            const uint8_t *bytes = aws_string_bytes(old_str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);
            }
            /* The string must still satisfy its validity predicate */
            assert(aws_string_is_valid(old_str));
        } else {
            /* When an allocator is present the memory may have been released.
               We cannot safely dereference it, but the pointer value is unchanged. */
            /* No additional dereference assertions here. */
        }
    } else {
        /* When str is NULL the function does nothing; nothing else to assert. */
    }
}
