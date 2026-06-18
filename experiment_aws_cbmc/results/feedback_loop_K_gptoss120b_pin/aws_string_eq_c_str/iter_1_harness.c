#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256
#define MAX_CSTR_LEN   256

void aws_string_eq_c_str_harness(void) {
    /* Allocate and nondeterministically initialize an aws_string */
    struct aws_string *str;
    bool str_is_null = nondet_bool();
    if (str_is_null) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        /* allocate space for struct + (len-1) extra bytes for flexible array */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + (len ? len - 1 : 0));
        __CPROVER_assume(str != NULL);
        /* allocator may be NULL (static string) or a valid allocator */
        bool allocator_is_null = nondet_bool();
        if (allocator_is_null) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }
        str->len = len;
        /* nondet initialize the bytes */
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        /* ensure a null terminator after the logical bytes (as required by aws_string) */
        if (len < MAX_STRING_LEN) {
            str->bytes[len] = 0;
        }
        /* assume the string satisfies the library invariant */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Allocate and nondeterministically initialize a C string */
    char *c_str;
    bool cstr_is_null = nondet_bool();
    if (cstr_is_null) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_CSTR_LEN);
        c_str = (char *)malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);
        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* Snapshot memory for frame condition checks */
    struct aws_string *str_snapshot = NULL;
    if (str != NULL) {
        size_t snap_len = str->len;
        str_snapshot = (struct aws_string *)malloc(sizeof(struct aws_string) + (snap_len ? snap_len - 1 : 0));
        __CPROVER_assume(str_snapshot != NULL);
        *str_snapshot = *str;
        for (size_t i = 0; i < snap_len; ++i) {
            str_snapshot->bytes[i] = str->bytes[i];
        }
        if (snap_len < MAX_STRING_LEN) {
            str_snapshot->bytes[snap_len] = str->bytes[snap_len];
        }
    }
    char *cstr_snapshot = NULL;
    if (c_str != NULL) {
        size_t c_len = 0;
        while (c_str[c_len] != '\0') ++c_len;
        cstr_snapshot = (char *)malloc(c_len + 1);
        __CPROVER_assume(cstr_snapshot != NULL);
        for (size_t i = 0; i <= c_len; ++i) {
            cstr_snapshot[i] = c_str[i];
        }
    }

    /* Call the function under verification */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Compute the expected result */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        /* both non‑NULL */
        size_t i = 0;
        expected = true;
        while (i < str->len && c_str[i] != '\0') {
            if (str->bytes[i] != (uint8_t)c_str[i]) {
                expected = false;
                break;
            }
            ++i;
        }
        if (expected) {
            /* lengths must match */
            if (i != str->len || c_str[i] != '\0') {
                expected = false;
            }
        }
    }

    /* Assert postconditions */
    assert(result == expected);

    /* Frame condition: str unchanged */
    if (str != NULL) {
        assert(str->allocator == str_snapshot->allocator);
        assert(str->len == str_snapshot->len);
        for (size_t i = 0; i < str->len; ++i) {
            assert(str->bytes[i] == str_snapshot->bytes[i]);
        }
        /* also check the byte after the logical length (null terminator) */
        if (str->len < MAX_STRING_LEN) {
            assert(str->bytes[str->len] == str_snapshot->bytes[str->len]);
        }
    }

    /* Frame condition: c_str unchanged */
    if (c_str != NULL) {
        size_t i = 0;
        while (c_str[i] != '\0') ++i;
        for (size_t j = 0; j <= i; ++j) {
            assert(c_str[j] == cstr_snapshot[j]);
        }
    }

    /* Clean up */
    if (str != NULL) free(str);
    if (c_str != NULL) free(c_str);
    if (str_snapshot != NULL) free(str_snapshot);
    if (cstr_snapshot != NULL) free(cstr_snapshot);

    return 0;
}
