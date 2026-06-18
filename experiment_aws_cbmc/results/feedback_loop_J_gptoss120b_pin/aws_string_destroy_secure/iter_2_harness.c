#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_secure_harness() {
    /* 1. Decide if the pointer is NULL */
    bool is_null = nondet_bool();
    struct aws_string *str = NULL;

    /* 2. Buffer for a non‑NULL string */
    uint8_t buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE];

    if (!is_null) {
        /* 3. Choose a length bounded by MAX_BUFFER_SIZE */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* 4. Use the buffer as the string object */
        str = (struct aws_string *)buffer;

        /* 5. Choose an allocator (NULL or default) */
        bool has_allocator = nondet_bool();
        if (has_allocator) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* 6. Initialise length */
        str->len = len;

        /* 7. Fill the byte array with nondeterministic data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* 8. Save old state for later comparison (only if non‑NULL) */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old = *str; /* copies allocator and len (bytes are not copied) */
        save_byte_from_array(aws_string_bytes(str), str->len, &old_bytes);
    }

    /* 9. Call the function under test */
    aws_string_destroy_secure(str);

    /* 10. Post‑condition checks */
    if (str) {
        if (old.allocator) {
            /* When an allocator is present the string memory is released.
               The pointer may now be dangling, so we only check that the
               allocator was non‑NULL before. */
            assert(old.allocator != NULL);
        } else {
            /* No allocator: memory is not freed, bytes must be zeroed */
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old.len; ++i) {
                assert(bytes[i] == 0);
            }

            /* Unchanged fields */
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);

            /* Validity invariant must still hold */
            assert(aws_string_is_valid(str));
        }
    } else {
        /* str was NULL: nothing should happen */
        assert(str == NULL);
    }
}
