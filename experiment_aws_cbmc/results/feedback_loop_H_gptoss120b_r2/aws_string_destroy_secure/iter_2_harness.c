#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

union aws_string_storage {
    struct aws_string s;
    uint8_t raw[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
};

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        if (nondet_bool()) {
            /* allocator is NULL – use stack‑allocated storage */
            union aws_string_storage storage;
            str = &storage.s;
            str->allocator = NULL;
        } else {
            /* allocator is default – allocate on the heap */
            str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
            __CPROVER_assume(str != NULL);
            str->allocator = aws_default_allocator();
        }

        str->len = len;

        /* initialise the bytes with nondeterministic values */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    if (str) {
        const struct aws_allocator *old_allocator = str->allocator;
        const size_t old_len = str->len;
        const bool old_allocator_is_null = (old_allocator == NULL);

        uint8_t *old_bytes = NULL;
        if (old_len > 0) {
            old_bytes = (uint8_t *)malloc(old_len);
            __CPROVER_assume(old_bytes != NULL);
            const uint8_t *cur_bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                old_bytes[i] = cur_bytes[i];
            }
        }

        aws_string_destroy_secure(str);

        if (old_allocator_is_null) {
            const uint8_t *cur_bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(cur_bytes[i] == 0);
            }
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            assert(aws_string_is_valid(str));
        } else {
            /* memory may have been freed; no further dereference of str */
        }

        if (old_bytes) {
            free(old_bytes);
        }
    } else {
        /* str is NULL – the function should be a no‑op. */
        aws_string_destroy_secure(str);
    }
}
