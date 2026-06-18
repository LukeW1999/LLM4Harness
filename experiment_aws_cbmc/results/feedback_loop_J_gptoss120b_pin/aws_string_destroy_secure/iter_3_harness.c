#include <stdbool.h>
#include <stddef.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
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

        /* 5. No allocator (NULL) to avoid freeing stack memory */
        str->allocator = NULL;

        /* 6. Initialise length */
        str->len = len;

        /* 7. Fill the byte array with nondeterministic data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* 8. Save old state for later comparison (only if non‑NULL) */
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old_len = str->len;
        save_byte_from_array(aws_string_bytes(str), str->len, &old_bytes);
    }

    /* 9. Call the function under test */
    aws_string_destroy_secure(str);

    /* 10. Post‑condition checks */
    if (str) {
        /* No allocator: memory is not freed, bytes must be zeroed */
        const uint8_t *bytes = aws_string_bytes(str);
        for (size_t i = 0; i < old_len; ++i) {
            assert(bytes[i] == 0);
        }

        /* Unchanged fields */
        assert(str->allocator == NULL);
        assert(str->len == old_len);

        /* Validity invariant must still hold */
        assert(aws_string_is_valid(str));
    } else {
        /* str was NULL: nothing should happen */
        assert(str == NULL);
    }
}
