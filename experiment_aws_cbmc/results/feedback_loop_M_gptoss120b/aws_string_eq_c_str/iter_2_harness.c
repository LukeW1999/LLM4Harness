#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_LEN 256

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    const char *c_str;

    /* nondeterministic choice for str */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_LEN);
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        str->len = len;
        str->allocator = aws_default_allocator();
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
    }

    /* nondeterministic choice for c_str */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_LEN);
        char *buf = (char *)malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_char();
        }
        buf[len] = '\0';
        c_str = buf;
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        assert(result == true || result == false);
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            c_len++;
        }
        if (str->len != c_len) {
            assert(result == false);
        }
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
