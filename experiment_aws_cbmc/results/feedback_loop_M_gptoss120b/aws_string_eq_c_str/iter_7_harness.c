#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

#define MAX_LEN 256

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    const char *c_str;

    /* Stack‑allocated storage for the aws_string and its bytes */
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_LEN];
    } str_storage;

    /* Stack‑allocated buffer for the C string */
    char c_buf[MAX_LEN + 1];

    /* nondeterministic choice for str */
    if (__CPROVER_nondet_bool()) {
        str = NULL;
    } else {
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= MAX_LEN);
        str = &str_storage.s;
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            str_storage.bytes[i] = __CPROVER_nondet_uint8_t();
        }
    }

    /* nondeterministic choice for c_str */
    if (__CPROVER_nondet_bool()) {
        c_str = NULL;
    } else {
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= MAX_LEN);
        for (size_t i = 0; i < len; ++i) {
            c_buf[i] = (char)__CPROVER_nondet_uint8_t();
        }
        c_buf[len] = '\0';
        c_str = c_buf;
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
