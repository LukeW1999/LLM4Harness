#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        str = NULL;
    } else if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        uint8_t bytes[MAX_BUFFER_SIZE + 1];
        for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        str = aws_string_new_from_array(aws_default_allocator(), bytes, len);
        __CPROVER_assume(str != NULL);
    } else {
        AWS_STATIC_STRING_FROM_LITERAL(static_str, "abc");
        str = (struct aws_string *)static_str;
    }

    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    const uint8_t *old_bytes = NULL;
    uint8_t old_byte_0 = 0;
    uint8_t old_byte_1 = 0;
    uint8_t old_byte_2 = 0;
    uint8_t old_byte_3 = 0;

    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        old_bytes = str->bytes;
        old_byte_0 = str->bytes[0];

        if (str->len > 1) {
            old_byte_1 = str->bytes[1];
        }
        if (str->len > 2) {
            old_byte_2 = str->bytes[2];
        }
        if (str->len > 3) {
            old_byte_3 = str->bytes[3];
        }
    }

    aws_string_destroy(str);

    assert(str == old_str);

    if (old_str == NULL) {
        assert(str == NULL);
    } else if (old_allocator == NULL) {
        assert(str != NULL);
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert(str->bytes == old_bytes);
        assert(str->bytes[0] == old_byte_0);
        if (old_len > 1) {
            assert(str->bytes[1] == old_byte_1);
        }
        if (old_len > 2) {
            assert(str->bytes[2] == old_byte_2);
        }
        if (old_len > 3) {
            assert(str->bytes[3] == old_byte_3);
        }
    } else {
        assert(old_allocator != NULL);
        assert(!AWS_MEM_IS_READABLE(old_str, sizeof(struct aws_string)));
        assert(!AWS_MEM_IS_WRITABLE(old_str, sizeof(struct aws_string)));
    }
}
