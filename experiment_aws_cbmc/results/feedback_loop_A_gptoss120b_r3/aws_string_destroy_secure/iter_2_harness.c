#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* nondeterministically choose a length within bounds */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate a buffer on the stack large enough for the struct and bytes */
        uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)storage;

        /* set allocator to NULL to avoid deallocation of stack memory */
        str->allocator = NULL;

        /* set the const length field (cast away const for harness) */
        *((size_t *)&str->len) = len;

        /* fill the byte array with nondeterministic data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* save a copy of the original bytes for later comparison */
        uint8_t old_bytes[MAX_STRING_LEN];
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = bytes[i];
        }

        /* assume the string is initially valid */
        __CPROVER_assume(aws_string_is_valid(str));

        /* call the function under test */
        aws_string_destroy_secure(str);

        /* post‑conditions: the data bytes up to len must be zeroed */
        for (size_t i = 0; i < len; ++i) {
            assert(bytes[i] == 0);
        }

        /* the string should remain valid after zeroing (allocator is NULL) */
        assert(aws_string_is_valid(str));
    }
}
