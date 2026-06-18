#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_LEN 64
#define MAX_CSTR_LEN   64

void aws_string_eq_c_str_harness(void) {
    /* nondeterministic choice of NULL / non-NULL for inputs */
    bool str_is_null = nondet_bool();
    bool cstr_is_null = nondet_bool();

    struct aws_string *str = NULL;
    char *c_str = NULL;

    /* ----- set up str ----- */
    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate space for struct + (len-1) extra bytes (bytes[1] already accounts for 1) */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* fields */
        str->allocator = NULL;               /* static string – allocator may be NULL */
        str->len = len;

        /* nondet fill of the byte array */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* validity */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* ----- set up c_str ----- */
    if (!cstr_is_null) {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_CSTR_LEN);

        c_str = malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);

        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';

        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* ----- save old state for immutability checks ----- */
    struct aws_string *old_str = NULL;
    if (str != NULL) {
        old_str = malloc(sizeof(struct aws_string) + (str->len > 0 ? str->len - 1 : 0));
        __CPROVER_assume(old_str != NULL);
        *old_str = *str; /* copies allocator, len, and first byte of the flexible array */
        for (size_t i = 0; i < str->len; ++i) {
            ((uint8_t *)old_str->bytes)[i] = ((uint8_t *)str->bytes)[i];
        }
    }

    char *old_c_str = NULL;
    size_t old_c_len = 0;
    if (c_str != NULL) {
        /* compute length (excluding terminating NUL) */
        while (c_str[old_c_len] != '\0') {
            ++old_c_len;
        }
        old_c_str = malloc(old_c_len + 1);
        __CPROVER_assume(old_c_str != NULL);
        for (size_t i = 0; i <= old_c_len; ++i) { /* copy NUL as well */
            old_c_str[i] = c_str[i];
        }
    }

    /* ----- call function under test ----- */
    bool result = aws_string_eq_c_str(str, c_str);

    /* ----- post‑condition: functional correctness ----- */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
    }
    assert(result == expected);

    /* ----- post‑condition: immutability of inputs ----- */
    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        for (size_t i = 0; i < str->len; ++i) {
            assert(((uint8_t *)str->bytes)[i] == ((uint8_t *)old_str->bytes)[i]);
        }
    }
    if (c_str != NULL) {
        for (size_t i = 0; i <= old_c_len; ++i) {
            assert(c_str[i] == old_c_str[i]);
        }
    }

    /* ----- validity invariants ----- */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }

    /* free allocated memory (not required for CBMC but keeps the model tidy) */
    if (str != NULL) {
        free((void *)str);
        free(old_str);
    }
    if (c_str != NULL) {
        free(c_str);
        free(old_c_str);
    }
}
