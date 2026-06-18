#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_LEN 64
#define MAX_CSTR_LEN   64

void aws_string_eq_c_str_harness(void) {
    /* 1. Non‑deterministic inputs */
    struct aws_string *str;
    char *c_str;

    /* str may be NULL or a valid aws_string */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate space for struct + flexible array */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* allocator can be NULL (static string) – still considered valid */
        str->allocator = NULL;
        str->len = len;

        /* fill the bytes with nondeterministic data */
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        /* null‑terminator after the data (not counted in len) */
        str->bytes[len] = 0;

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* c_str may be NULL or a valid C string */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len < MAX_CSTR_LEN); /* leave room for terminator */

        c_str = (char *)malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);

        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        /* ensure null‑termination somewhere within the buffer */
        c_str[c_len] = 0;
    }

    /* 2. Save old state of str (if non‑NULL) */
    struct aws_string old_str;
    uint8_t *old_bytes = NULL;
    if (str != NULL) {
        old_str.allocator = str->allocator;
        old_str.len = str->len;
        old_bytes = (uint8_t *)malloc(str->len + 1);
        __CPROVER_assume(old_bytes != NULL);
        for (size_t i = 0; i <= str->len; ++i) {
            old_bytes[i] = str->bytes[i];
        }
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Post‑condition: result matches the specification */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
    }
    assert(result == expected);

    /* 5. Unchanged fields (the function is pure) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_bytes_match(str->bytes, old_bytes, str->len + 1);
    }

    /* 6. Validity invariant */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }

    /* clean up */
    if (str != NULL) {
        free((void *)str);
        free(old_bytes);
    }
    if (c_str != NULL) {
        free(c_str);
    }
}
