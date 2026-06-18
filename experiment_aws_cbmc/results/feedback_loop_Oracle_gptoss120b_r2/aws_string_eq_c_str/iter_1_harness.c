#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 16
#define MAX_CSTR_LEN   16

void aws_string_eq_c_str_harness(void) {
    /* ---------- nondet allocation of aws_string ---------- */
    bool str_is_null = __CPROVER_nondet_bool();
    struct aws_string *str = NULL;
    size_t str_len = 0;
    size_t str_alloc_size = 0;

    if (!str_is_null) {
        str_len = __CPROVER_nondet_uint();
        __CPROVER_assume(str_len <= MAX_STRING_LEN);

        str_alloc_size = sizeof(struct aws_string) + str_len;
        str = malloc(str_alloc_size);
        __CPROVER_assume(str != NULL);

        /* static string – allocator is NULL */
        str->allocator = NULL;
        str->len = str_len;

        /* fill the flexible array member with nondet bytes */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = __CPROVER_nondet_uint8();
        }
    }

    /* ---------- nondet allocation of C string ---------- */
    bool c_is_null = __CPROVER_nondet_bool();
    char *c_str = NULL;
    size_t c_len = 0;

    if (!c_is_null) {
        c_len = __CPROVER_nondet_uint();
        __CPROVER_assume(c_len <= MAX_CSTR_LEN);

        c_str = malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);

        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = __CPROVER_nondet_char();
            __CPROVER_assume(c_str[i] != '\0'); /* ensure no early terminator */
        }
        c_str[c_len] = '\0';
    }

    /* ---------- precondition assumptions ---------- */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(IMPLIES(c_str != NULL, aws_c_string_is_valid(c_str)));

    /* ---------- snapshot memory for frame condition ---------- */
    unsigned char *str_snapshot = NULL;
    size_t str_snapshot_len = 0;
    if (str != NULL) {
        str_snapshot_len = str_alloc_size;
        str_snapshot = malloc(str_snapshot_len);
        __CPROVER_assume(str_snapshot != NULL);
        memcpy(str_snapshot, str, str_snapshot_len);
    }

    unsigned char *c_snapshot = NULL;
    size_t c_snapshot_len = 0;
    if (c_str != NULL) {
        c_snapshot_len = c_len + 1;
        c_snapshot = malloc(c_snapshot_len);
        __CPROVER_assume(c_snapshot != NULL);
        memcpy(c_snapshot, c_str, c_snapshot_len);
    }

    /* ---------- call the function under verification ---------- */
    bool result = aws_string_eq_c_str(str, c_str);

    /* ---------- postcondition: return value correctness ---------- */
    if (str == NULL && c_str == NULL) {
        __CPROVER_assert(result == true,
                         "aws_string_eq_c_str returns true when both arguments are NULL");
    } else if (str == NULL || c_str == NULL) {
        __CPROVER_assert(result == false,
                         "aws_string_eq_c_str returns false when exactly one argument is NULL");
    } else {
        size_t c_actual_len = strlen(c_str);
        bool expected = (c_actual_len == str->len) &&
                        (memcmp(str->bytes, c_str, str->len) == 0);
        __CPROVER_assert(result == expected,
                         "aws_string_eq_c_str returns correct comparison result");
    }

    /* ---------- frame condition: no memory modification ---------- */
    if (str != NULL) {
        __CPROVER_assert(memcmp(str_snapshot, str, str_snapshot_len) == 0,
                         "aws_string_eq_c_str does not modify the aws_string object");
    }
    if (c_str != NULL) {
        __CPROVER_assert(memcmp(c_snapshot, c_str, c_snapshot_len) == 0,
                         "aws_string_eq_c_str does not modify the C string");
    }

    return 0;
}
