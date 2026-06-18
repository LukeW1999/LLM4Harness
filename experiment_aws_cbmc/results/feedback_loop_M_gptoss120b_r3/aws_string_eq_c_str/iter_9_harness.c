#include <aws/common/string.h>
#include <stdbool.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    const char *c_str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        struct aws_string *tmp = (struct aws_string *)storage;
        tmp->allocator = aws_default_allocator();
        tmp->len = len;

        for (size_t i = 0; i < len; ++i) {
            tmp->bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(tmp));
        str = tmp;
    }

    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
        char buf[MAX_BUFFER_SIZE + 1];
        for (size_t i = 0; i < c_len; ++i) {
            buf[i] = (char)nondet_uint8_t();
        }
        buf[c_len] = '\0';
        c_str = buf;
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes_storage;
    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_bytes_storage);
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    }

    if (str != NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes_storage);
        assert(aws_string_is_valid(str));
    }
}
