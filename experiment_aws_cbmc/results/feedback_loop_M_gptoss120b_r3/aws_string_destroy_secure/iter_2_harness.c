#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
_Bool nondet_bool(void);

void aws_string_destroy_secure_harness(void) {
    _Bool is_null = nondet_bool();
    struct aws_string *str = NULL;

    if (!is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 1024);

        struct aws_allocator *alloc = nondet_bool() ? NULL : aws_default_allocator();

        size_t alloc_size = sizeof(struct aws_string) + (len > 0 ? len - 1 : 0);
        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        *((struct aws_allocator **) &str->allocator) = alloc;
        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));

        struct aws_allocator *old_allocator = (struct aws_allocator *)str->allocator;
        size_t old_len = str->len;
        uint8_t *old_bytes = (uint8_t *)malloc(old_len);
        __CPROVER_assume(old_bytes != NULL);
        for (size_t i = 0; i < old_len; ++i) {
            old_bytes[i] = bytes[i];
        }

        aws_string_destroy_secure(str);

        if (old_allocator == NULL) {
            assert(aws_string_is_valid(str));
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);
            }
        } else {
            /* string may have been freed; no further checks */
        }

        free(old_bytes);
    } else {
        aws_string_destroy_secure(str);
        assert(1);
    }
}
