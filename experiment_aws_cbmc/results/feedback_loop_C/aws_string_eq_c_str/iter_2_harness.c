#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = malloc(sizeof(struct aws_string) + sizeof(uint8_t));
    if (str) {
        str->allocator = (struct aws_allocator *)nondet_ptr();
        str->len = nondet_size_t();
        str->bytes[0] = nondet_uint8_t();
    }

    char *c_str = (char *)nondet_ptr();
    if (c_str) {
        size_t c_str_len = nondet_size_t();
        __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
        c_str = (char *)malloc(c_str_len + 1);
        if (c_str) {
            for (size_t i = 0; i < c_str_len; i++) {
                c_str[i] = nondet_uint8_t();
            }
            c_str[c_str_len] = '\0';
        }
    }

    struct aws_string old_str;
    if (str) {
        old_str = *str;
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str) {
        assert(aws_string_is_valid(str));
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(str->bytes[0] == old_str.bytes[0]);
    }

    if (result) {
        if (c_str) {
            assert_bytes_match(str->bytes, (const uint8_t *)c_str, str->len);
        }
    } else {
        if (c_str) {
            assert(!bytes_match(str->bytes, (const uint8_t *)c_str, str->len));
        }
    }

    free(str);
    free(c_str);
}
