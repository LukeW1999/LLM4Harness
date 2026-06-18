#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness(void) {
    /* Symbolic inputs */
    struct aws_string *str;
    const char *c_str;

    /* nondet choice for NULL or non-NULL string */
    bool str_is_null = nondet_bool();
    if (str_is_null) {
        str = NULL;
    } else {
        /* allocate a string with nondet length */
        size_t max_len = nondet_uint();
        __CPROVER_assume(max_len <= 256);
        /* allocate enough space for the struct plus flexible array */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + max_len);
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        str->len = max_len;
        /* nondet bytes */
        for (size_t i = 0; i < max_len; ++i) {
            str->bytes[i] = nondet_uint8();
        }
        /* ensure a null terminator after the data (as required by aws_string) */
        str->bytes[max_len] = 0;
    }

    /* nondet choice for NULL or non-NULL c string */
    bool c_str_is_null = nondet_bool();
    if (c_str_is_null) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_uint();
        __CPROVER_assume(c_len <= 256);
        char *tmp = (char *)malloc(c_len + 1);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < c_len; ++i) {
            tmp[i] = nondet_uint8();
        }
        tmp[c_len] = '\0';
        c_str = tmp;
    }

    /* Preconditions */
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* Pre‑call snapshot */
    size_t orig_str_len = 0;
    const uint8_t *orig_str_bytes = NULL;
    if (str != NULL) {
        orig_str_len = str->len;
        orig_str_bytes = str->bytes;
    }
    const char *orig_c_str = c_str;

    /* Call the function under verification */
    bool result = aws_string_eq_c_str(str, c_str);

    /* ASSERT_POSTCONDITIONS_HERE */
}
