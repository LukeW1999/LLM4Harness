#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

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

        /* 3. Allocate storage on the stack for the struct plus its flexible array */
        uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)storage;

        /* 4. No allocator (to avoid freeing stack memory) */
        str->allocator = NULL;

        /* 5. Set the length (const field) */
        *((size_t *)&str->len) = len;

        /* 6. Fill the byte payload with nondet data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* 7. Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 8. Save old state for later comparison */
    struct aws_string old = {0};
    uint8_t old_bytes[MAX_STRING_LEN];
    bool have_old_bytes = false;
    if (str != NULL) {
        old.allocator = str->allocator;
        old.len = str->len;
        if (str->len > 0) {
            have_old_bytes = true;
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
        /* When the input is NULL the function does nothing */
        assert(str == NULL);
    }
}
