#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256
#define IMPLIES(a,b) (!(a) || (b))

void aws_string_eq_c_str_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *str = NULL;
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STRING_LEN);
    if (__CPROVER_nondet_bool()) {
        uint8_t str_buf[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)str_buf;
        str->allocator = allocator;
        str->len = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            str->bytes[i] = __CPROVER_nondet_uchar();
        }
    }

    char *c_str = NULL;
    size_t c_len;
    __CPROVER_assume(c_len <= MAX_STRING_LEN);
    if (__CPROVER_nondet_bool()) {
        char c_buf[MAX_STRING_LEN + 1];
        c_str = c_buf;
        for (size_t i = 0; i < c_len; ++i) {
            c_buf[i] = __CPROVER_nondet_char();
        }
        c_buf[c_len] = '\0';
    }

    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(IMPLIES(c_str != NULL, aws_c_string_is_valid(c_str)));

    /* make copies for frame condition checks */
    struct aws_string *str_copy = NULL;
    size_t str_copy_size = 0;
    if (str) {
        str_copy_size = sizeof(struct aws_string) + str->len;
        uint8_t copy_buf[sizeof(struct aws_string) + MAX_STRING_LEN];
        str_copy = (struct aws_string *)copy_buf;
        memcpy(str_copy, str, str_copy_size);
    }

    char *c_str_copy = NULL;
    size_t c_str_copy_len = 0;
    if (c_str) {
        c_str_copy_len = c_len + 1;
        char c_copy_buf[MAX_STRING_LEN + 1];
        memcpy(c_copy_buf, c_str, c_str_copy_len);
        c_str_copy = c_copy_buf;
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result);
    } else if (str == NULL || c_str == NULL) {
        assert(!result);
    } else {
        size_t c_strlen = strlen(c_str);
        bool expected = (str->len == c_strlen) && (memcmp(str->bytes, c_str, str->len) == 0);
        assert(result == expected);
    }

    if (result && str && c_str) {
        assert(str->len == strlen(c_str));
        assert(memcmp(str->bytes, c_str, str->len) == 0);
    }

    if (str && str_copy) {
        assert(memcmp(str, str_copy, str_copy_size) == 0);
    }

    if (c_str && c_str_copy) {
        assert(memcmp(c_str, c_str_copy, c_str_copy_len) == 0);
    }
}
