#include <assert.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 256);

        struct aws_allocator *alloc = aws_default_allocator();
        str = (struct aws_string *)alloc->mem_acquire(
            alloc,
            sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondet decide whether the string uses an allocator */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = alloc;
        }

        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old_allocator = str->allocator;
        old_len = str->len;
        save_byte_from_array(aws_string_bytes(str), str->len, &old_bytes);
    }

    aws_string_destroy_secure(str);

    if (str == NULL) {
        assert(str == NULL);
    } else {
        if (old_allocator == NULL) {
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);
            }
            assert(str->allocator == NULL);
            assert(str->len == 0);
            assert(aws_string_is_valid(str));
        } else {
            (void)old_bytes;
        }
    }
}
