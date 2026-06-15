#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_LEN 256

void aws_string_eq_c_str_harness(void) {
    /* nondeterministic allocation of aws_string */
    struct aws_string *str = NULL;
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STRING_LEN);
    if (__CPROVER_nondet_bool()) {
        /* allocate space for flexible array member */
        str = malloc(sizeof(struct aws_string) + (str_len ? str_len - 1 : 0));
        if (str) {
            str->allocator = NULL;               /* allocator may be NULL for static strings */
            str->len = str_len;
            for (size_t i = 0; i < str_len; ++i) {
                str->bytes[i] = __CPROVER_nondet_uchar();
            }
        }
    }

    /* nondeterministic allocation of C string */
    char *c_str = NULL;
    size_t c_len;
    __CPROVER_assume(c_len <= MAX_STRING_LEN);
    if (__CPROVER_nondet_bool()) {
        c_str = malloc(c_len + 1);
        if (c_str) {
            for (size_t i = 0; i < c_len; ++i) {
                c_str[i] = __CPROVER_nondet_char();
            }
            c_str[c_len] = '\0';
        }
    }

    /* structural validity assumptions */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(IMPLIES(c_str != NULL, aws_c_string_is_valid(c_str)));

    /* make copies for frame condition checks */
    struct aws_string *str_copy = NULL;
    size_t str_copy_size = 0;
    if (str) {
        str_copy_size = sizeof(struct aws_string) + (str->len ? str->len - 1 : 0);
        str_copy = malloc(str_copy_size);
        if (str_copy) {
            memcpy(str_copy, str, str_copy_size);
        }
    }

    char *c_str_copy = NULL;
    size_t c_str_copy_len = 0;
    if (c_str) {
        c_str_copy_len = strlen(c_str) + 1;
        c_str_copy = malloc(c_str_copy_len);
        if (c_str_copy) {
            memcpy(c_str_copy, c_str, c_str_copy_len);
        }
    }

    /* call the function under verification */
    bool result = aws_string_eq_c_str(str, c_str);

    /* postcondition: return value correctness */
    if (str == NULL && c_str == NULL) {
        assert(result);
    } else if (str == NULL || c_str == NULL) {
        assert(!result);
    } else {
        bool expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
        assert(result == expected);
    }

    /* optional stronger postcondition when result is true */
    if (result && str && c_str) {
        assert(str->len == strlen(c_str));
        assert(memcmp(str->bytes, c_str, str->len) == 0);
    }

    /* frame condition: str unchanged */
    if (str && str_copy) {
        assert(memcmp(str, str_copy, str_copy_size) == 0);
    }

    /* frame condition: c_str unchanged */
    if (c_str && c_str_copy) {
        assert(memcmp(c_str, c_str_copy, c_str_copy_len) == 0);
    }

    return 0;
}
