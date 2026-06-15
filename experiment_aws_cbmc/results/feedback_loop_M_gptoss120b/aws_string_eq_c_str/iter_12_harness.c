#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 256

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    const char *c_str;

    /* Stack‑allocated storage for the aws_string and its bytes */
    uint8_t str_storage[sizeof(struct aws_string) + MAX_LEN];
    /* Stack‑allocated buffer for the C string */
    char c_buf[MAX_LEN + 1];

    /* nondeterministic choice for str */
    if (__CPROVER_nondet_bool()) {
        str = NULL;
    } else {
        size_t len = (size_t)__CPROVER_nondet_uint();
        __CPROVER_assume(len <= MAX_LEN);
        str = (struct aws_string *)str_storage;
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;
        uint8_t *bytes = (uint8_t *)(str + 1);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = __CPROVER_nondet_uchar();
        }
    }

    /* nondeterministic choice for c_str */
    if (__CPROVER_nondet_bool()) {
        c_str = NULL;
    } else {
        size_t len = (size_t)__CPROVER_nondet_uint();
        __CPROVER_assume(len <= MAX_LEN);
        for (size_t i = 0; i < len; ++i) {
            c_buf[i] = (char)__CPROVER_nondet_uchar();
        }
        c_buf[len] = '\0';
        c_str = c_buf;
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        __CPROVER_assert(result == true, "both NULL should be equal");
    } else if (str == NULL || c_str == NULL) {
        __CPROVER_assert(result == false, "one NULL should be unequal");
    } else {
        __CPROVER_assert(result == true || result == false, "result is boolean");
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            c_len++;
        }
        if (str->len != c_len) {
            __CPROVER_assert(result == false, "different lengths imply false");
        }
    }

    if (str != NULL) {
        __CPROVER_assert(aws_string_is_valid(str), "aws_string must be valid");
    }
}
