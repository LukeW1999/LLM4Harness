#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* Bounding constants – adjust as needed for the proof */
#define MAX_STR_LEN 32U

void aws_string_eq_c_str_harness(void) {
    /* ---------- nondeterministic inputs ---------- */
    struct aws_string *str;
    const char *c_str;

    bool str_is_null = nondet_bool();
    bool c_is_null   = nondet_bool();

    /* ----- allocate and initialize str when not NULL ----- */
    if (!str_is_null) {
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STR_LEN);

        /* allocate space for struct + flexible array */
        str = malloc(sizeof(struct aws_string) + (str_len == 0 ? 0 : str_len - 1));
        __CPROVER_assume(str != NULL);

        /* allocator may be NULL (static string) or a default allocator */
        str->allocator = nondet_bool() ? NULL : aws_default_allocator();

        /* set length */
        *((size_t *)&str->len) = str_len;   /* const cast for initialization */

        /* nondeterministically fill the bytes */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* ----- allocate and initialize c_str when not NULL ----- */
    if (!c_is_null) {
        size_t c_len;
        __CPROVER_assume(c_len <= MAX_STR_LEN);

        c_str = malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);

        for (size_t i = 0; i < c_len; ++i) {
            ((char *)c_str)[i] = (char)nondet_uint8_t();
        }
        ((char *)c_str)[c_len] = '\0'; /* ensure null‑termination */
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    } else {
        c_str = NULL;
    }

    /* ---------- save old state for immutability checks ---------- */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes = {0};

    if (str != NULL) {
        old_str = *str; /* copies allocator and len (bytes are not copied) */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct store_byte_from_buffer old_c_str_bytes = {0};
    size_t old_c_len = 0;
    if (c_str != NULL) {
        /* compute length of c_str (bounded) */
        while (old_c_len < MAX_STR_LEN && ((char *)c_str)[old_c_len] != '\0') {
            ++old_c_len;
        }
        save_byte_from_array((const uint8_t *)c_str, old_c_len, &old_c_str_bytes);
    }

    /* ---------- call function under test ---------- */
    bool result = aws_string_eq_c_str(str, c_str);

    /* ---------- postcondition: result matches specification ---------- */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        /* both non‑NULL: compare lengths and contents */
        /* compute length of c_str (bounded) */
        size_t c_len = 0;
        while (c_len < MAX_STR_LEN && ((char *)c_str)[c_len] != '\0') {
            ++c_len;
        }
        bool same_len = (c_len == str->len);
        bool same_bytes = true;
        for (size_t i = 0; i < str->len && same_bytes; ++i) {
            if (str->bytes[i] != (uint8_t)((char *)c_str)[i]) {
                same_bytes = false;
            }
        }
        expected = same_len && same_bytes;
    }
    assert(result == expected);

    /* ---------- immutability: inputs must not change ---------- */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_bytes_match(str->bytes, old_str_bytes.arr, str->len);
    }

    if (c_str != NULL) {
        /* length of c_str unchanged */
        size_t c_len = 0;
        while (c_len < MAX_STR_LEN && ((char *)c_str)[c_len] != '\0') {
            ++c_len;
        }
        assert(c_len == old_c_len);
        assert_bytes_match((const uint8_t *)c_str, old_c_str_bytes.arr, c_len);
    }

    /* ---------- validity invariants after the call ---------- */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }

    return 0;
}
