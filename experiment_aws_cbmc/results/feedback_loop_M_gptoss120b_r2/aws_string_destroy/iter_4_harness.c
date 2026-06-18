#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str;
    bool have_str = nondet_bool();

    if (have_str) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)storage;

        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old = *str;
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    aws_string_destroy(str);

    if (str) {
        assert(str->allocator == old.allocator);
        assert(str->len == old.len);
        assert_byte_from_buffer(str->bytes, str->len, &old_bytes);
        assert(aws_string_is_valid(str));
    }
}
