#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 256);

        if (nondet_bool()) {
            static uint8_t static_buf[sizeof(struct aws_string) + 256];
            str = (struct aws_string *)static_buf;
            str->allocator = NULL;
        } else {
            struct aws_allocator *alloc = aws_default_allocator();
            str = (struct aws_string *)alloc->mem_acquire(
                alloc,
                sizeof(struct aws_string) + (len ? len - 1 : 0));
            __CPROVER_assume(str != NULL);
            str->allocator = alloc;
        }

        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old = *str;
        save_byte_from_array(aws_string_bytes(str), str->len, &old_bytes);
    }

    aws_string_destroy_secure(str);

    if (str == NULL) {
        assert(str == NULL);
    } else {
        assert(str->allocator == old.allocator);
        assert(str->len == old.len);

        if (str->allocator == NULL) {
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < str->len; ++i) {
                assert(bytes[i] == 0);
            }
            assert(aws_string_is_valid(str));
        } else {
            (void)old_bytes;
        }
    }
}
