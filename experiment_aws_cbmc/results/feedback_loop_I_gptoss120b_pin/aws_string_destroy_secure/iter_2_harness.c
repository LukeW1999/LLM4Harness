#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* stack‑allocated buffer for the string structure and its bytes */
        static uint8_t buffer[sizeof(struct aws_string) + MAX_STRING_LEN - 1];
        str = (struct aws_string *)buffer;

        /* static allocation (no allocator) */
        str->allocator = NULL;
        str->len = len;

        /* fill the byte array with nondeterministic data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Save old state for immutability checks */
    struct aws_string old;
    uint8_t *old_bytes = NULL;
    if (str) {
        old = *str; /* copy allocator and len */
        if (str->len > 0) {
            old_bytes = malloc(str->len);
            __CPROVER_assume(old_bytes != NULL);
            for (size_t i = 0; i < str->len; ++i) {
                old_bytes[i] = ((uint8_t *)aws_string_bytes(str))[i];
            }
        }
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Post‑condition checks */
    if (str) {
        /* len and allocator must remain unchanged */
        assert(str->len == old.len);
        assert(str->allocator == old.allocator);

        /* all data bytes must be zeroed */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < str->len; ++i) {
            assert(bytes[i] == 0);
        }

        /* if the string was statically allocated (no allocator), it must still be valid */
        if (str->allocator == NULL) {
            assert(aws_string_is_valid(str));
        }
    } else {
        /* when str is NULL, nothing changes – no additional assertions needed */
    }
}
