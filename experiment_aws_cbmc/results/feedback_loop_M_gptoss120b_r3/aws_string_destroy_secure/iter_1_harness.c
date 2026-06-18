#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_destroy_secure_harness(void) {
    /* 1. Non‑deterministically decide whether the pointer is NULL */
    bool is_null = nondet_bool();
    struct aws_string *str = NULL;

    if (!is_null) {
        /* 2. Non‑deterministically choose a length and bound it */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* 3. Non‑deterministically choose an allocator (NULL or default) */
        struct aws_allocator *alloc = nondet_bool() ? NULL : aws_default_allocator();

        /* 4. Allocate the aws_string object with space for the flexible array */
        size_t alloc_size = sizeof(struct aws_string) + (len > 0 ? len - 1 : 0);
        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        /* 5. Initialise the (const) fields – cast away constness for the harness */
        *((struct aws_allocator * const *)&str->allocator) = alloc;
        *((size_t const *)&str->len) = len;

        /* 6. Fill the byte payload with nondet data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* 7. Assume the freshly created string is valid */
        __CPROVER_assume(aws_string_is_valid(str));

        /* 8. Save a copy of the old state (allocator, len, and bytes) */
        struct aws_allocator *old_allocator = str->allocator;
        size_t old_len = str->len;
        uint8_t *old_bytes = malloc(len);
        __CPROVER_assume(old_bytes != NULL);
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = bytes[i];
        }

        /* 9. Call the function under test */
        aws_string_destroy_secure(str);

        /* 10. Post‑conditions */
        if (old_allocator == NULL) {
            /* No deallocation happened – the string object is still alive */
            assert(aws_string_is_valid(str));
            /* The length must be unchanged */
            assert(str->len == old_len);
            /* The allocator pointer must be unchanged */
            assert(str->allocator == old_allocator);
            /* All data bytes must have been zeroed */
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);
            }
        } else {
            /* The string was freed; we must not dereference it after the call.
               The harness therefore makes no further assertions about its contents. */
        }

        /* Clean up the saved copy of the old bytes */
        free(old_bytes);
    } else {
        /* str is NULL – the function should be a no‑op */
        aws_string_destroy_secure(str);
        /* No state to check; just ensure the call does not crash */
        assert(true);
    }
}
