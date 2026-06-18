#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)storage;

        /* Use a NULL allocator to avoid freeing stack memory */
        str->allocator = NULL;

        /* Set the (const) length field */
        *((size_t *)&str->len) = len;

        /* Initialize the string bytes with nondeterministic data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* Keep a copy of the original bytes (optional, not used in assertions) */
        uint8_t old_bytes[MAX_STRING_LEN];
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = bytes[i];
        }

        __CPROVER_assume(aws_string_is_valid(str));

        aws_string_destroy_secure(str);

        for (size_t i = 0; i < len; ++i) {
            assert(bytes[i] == 0);
        }

        assert(aws_string_is_valid(str));
    }
}
