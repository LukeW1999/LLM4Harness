#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256
#define MAX_CSTR_LEN   256

void aws_string_eq_c_str_harness(void) {
    /* 1. Declare and bound the aws_string */
    struct aws_string *str;
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_STRING_LEN);

    /* allocate memory for the struct plus flexible array */
    str = (struct aws_string *)malloc(sizeof(struct aws_string) + (str_len == 0 ? 0 : str_len - 1));
    __CPROVER_assume(str != NULL);

    /* nondeterministically set allocator (NULL is allowed for static strings) */
    str->allocator = aws_default_allocator();

    /* set length */
    *((size_t *)&str->len) = str_len;   /* const cast for initialization */

    /* fill bytes with nondet data */
    for (size_t i = 0; i < str_len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    /* assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Declare and bound the C string */
    char *c_str;
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len < MAX_CSTR_LEN);   /* length before null terminator */

    c_str = (char *)malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill with nondet data and ensure null termination */
    for (size_t i = 0; i < c_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_len] = '\0';

    /* assume the C string is valid */
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    /* 3. Save old state */
    struct aws_string old_str = *str;   /* copies allocator and len */
    uint8_t *old_bytes = (uint8_t *)malloc(str_len);
    __CPROVER_assume(old_bytes != NULL);
    for (size_t i = 0; i < str_len; ++i) {
        old_bytes[i] = ((uint8_t *)str->bytes)[i];
    }

    char *old_c_str = (char *)malloc(c_len + 1);
    __CPROVER_assume(old_c_str != NULL);
    for (size_t i = 0; i <= c_len; ++i) {
        old_c_str[i] = c_str[i];
    }

    /* 4. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 5. Assert unchanged fields */
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    assert_bytes_match(str->bytes, old_bytes, str_len);

    assert_bytes_match((uint8_t *)c_str, (uint8_t *)old_c_str, c_len + 1);

    /* 6. Assert validity invariants */
    assert(aws_string_is_valid(str));
    assert(aws_c_string_is_valid(c_str));

    /* 7. Clean up */
    free((void *)old_bytes);
    free((void *)old_c_str);
    free((void *)str);
    free((void *)c_str);
}
