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

        /* Allocate a stack object with enough space for the flexible array */
        struct {
            struct aws_string s;
            uint8_t bytes[1024];
        } storage;

        str = &storage.s;

        /* Set allocator to NULL to avoid freeing stack memory */
        ((struct aws_allocator **)&str->allocator)[0] = NULL;
        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));

        /* Preserve a copy of the original bytes */
        uint8_t old_bytes[1024];
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = bytes[i];
        }

        aws_string_destroy_secure(str);

        /* After secure destroy with NULL allocator, the string should remain valid,
           its length unchanged, and its contents zeroed. */
        assert(aws_string_is_valid(str));
        assert(str->len == len);
        assert(str->allocator == NULL);
        for (size_t i = 0; i < len; ++i) {
            assert(bytes[i] == 0);
        }
    } else {
        aws_string_destroy_secure(str);
        assert(1);
    }
}
