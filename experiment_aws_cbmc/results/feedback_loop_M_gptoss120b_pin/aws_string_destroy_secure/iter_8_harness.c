#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        struct {
            struct aws_string s;
            uint8_t bytes[MAX_BUFFER_SIZE];
        } storage;

        str = &storage.s;

        struct aws_allocator *alloc = nondet_bool() ? aws_default_allocator() : NULL;
        str->allocator = alloc;
        str->len = len;

        uint8_t *b = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            b[i] = nondet_uint8_t();
        }

        if (str->allocator) {
            __CPROVER_assume(aws_string_is_valid(str));
        }
    } else {
        str = NULL;
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str) {
        old_allocator = str->allocator;
        old_len = str->len;
    }

    aws_string_destroy_secure(str);

    if (str) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);

        if (old_allocator == NULL) {
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < str->len; ++i) {
                assert(bytes[i] == 0);
            }
            assert((void *)bytes == (void *)aws_string_bytes(str));
        }

        if (str->allocator) {
            assert(aws_string_is_valid(str));
        }
    }
}
