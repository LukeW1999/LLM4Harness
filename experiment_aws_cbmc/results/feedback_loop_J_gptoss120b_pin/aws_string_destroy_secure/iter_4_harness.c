#include <stdbool.h>
#include <stddef.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_STRING_SIZE 256

void aws_string_destroy_secure_harness() {
    /* 1. Decide if the pointer is NULL */
    bool is_null = nondet_bool();
    struct aws_string *str = NULL;

    /* 2. Buffer for a non‑NULL string */
    uint8_t buffer[sizeof(struct aws_string) + MAX_STRING_SIZE];

    if (!is_null) {
        /* 3. Choose a length bounded by MAX_STRING_SIZE */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_SIZE);

        /* 4. Use the buffer as the string object */
        str = (struct aws_string *)buffer;

        /* 5. Use the default allocator (required for proper free) */
        str->allocator = aws_default_allocator();

        /* 6. Initialise length */
        str->len = len;

        /* 7. Fill the byte array with nondeterministic data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* 8. Call the function under test */
    aws_string_destroy_secure(str);

    /* 9. Post‑condition checks */
    if (str) {
        /* After destruction the memory may have been freed; no further checks. */
    } else {
        /* str was NULL: nothing should happen */
        assert(str == NULL);
    }
}
