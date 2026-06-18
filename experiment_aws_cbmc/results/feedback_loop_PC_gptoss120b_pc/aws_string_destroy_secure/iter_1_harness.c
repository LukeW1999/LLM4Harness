#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    /* 1. Declare the string pointer (may be NULL) */
    struct aws_string *str;
    bool is_null = nondet_bool();
    if (is_null) {
        str = NULL;
    } else {
        /* 2. Non‑deterministically choose a length and bound it */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* 3. Allocate memory for the struct plus its flexible array */
        /* struct aws_string contains one byte in the flexible array, so allocate len‑1 extra */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* 4. Non‑deterministically decide whether the string has an allocator */
        bool has_allocator = nondet_bool();
        if (has_allocator) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* 5. Set the length */
        *((size_t *)&str->len) = len; /* const field – cast away const for initialization */

        /* 6. Fill the byte payload with nondet data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* 7. Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 8. Save old state for later comparison */
    struct aws_string old = {0};
    uint8_t *old_bytes = NULL;
    if (str != NULL) {
        old.allocator = str->allocator;
        old.len = str->len;
        if (str->len > 0) {
            old_bytes = (uint8_t *)malloc(str->len);
            __CPROVER_assume(old_bytes != NULL);
            for (size_t i = 0; i < str->len; ++i) {
                old_bytes[i] = ((uint8_t *)str->bytes)[i];
            }
        }
    }

    /* 9. Call the function under test */
    aws_string_destroy_secure(str);

    /* 10. Post‑condition checks */
    if (str != NULL) {
        /* 10a. All data bytes must be zeroed */
        for (size_t i = 0; i < str->len; ++i) {
            assert(((uint8_t *)str->bytes)[i] == 0);
        }

        /* 10b. Length must remain unchanged */
        assert(str->len == old.len);

        /* 10c. Allocator pointer must remain unchanged */
        assert(str->allocator == old.allocator);
    } else {
        /* When the input is NULL the function does nothing; nothing to assert */
        assert(str == NULL);
    }

    /* 11. Clean up any auxiliary allocations */
    if (old_bytes != NULL) {
        free(old_bytes);
    }
    if (str != NULL) {
        free(str);
    }

    return 0;
}
