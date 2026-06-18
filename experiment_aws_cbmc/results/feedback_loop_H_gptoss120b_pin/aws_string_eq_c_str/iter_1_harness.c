#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_STRING_LEN 256
#define MAX_CSTR_LEN   256

void aws_string_eq_c_str_harness() {
    /* ---------- nondeterministic inputs ---------- */
    struct aws_string *str;
    bool str_is_null = nondet_bool();

    if (str_is_null) {
        str = NULL;
    } else {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_STRING_LEN);

        /* allocate space for struct + flexible array */
        str = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(str != NULL);

        str->allocator = aws_default_allocator();
        str->len = str_len;
        /* bytes are part of the allocation; contents are nondet */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    const char *c_str;
    bool c_is_null = nondet_bool();

    if (c_is_null) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_CSTR_LEN);

        char *tmp = malloc(c_len + 1);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < c_len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        tmp[c_len] = '\0';
        c_str = tmp;

        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* ---------- save old state ---------- */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str; /* copies allocator and len */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct store_byte_from_buffer old_c_str_bytes;
    size_t c_len_saved = 0;
    if (c_str != NULL) {
        /* compute length including null terminator */
        while (c_str[c_len_saved] != '\0' && c_len_saved < MAX_CSTR_LEN) {
            ++c_len_saved;
        }
        ++c_len_saved; /* include terminating '\0' */
        save_byte_from_array((const uint8_t *)c_str, c_len_saved, &old_c_str_bytes);
    }

    /* ---------- call function under test ---------- */
    bool result = aws_string_eq_c_str(str, c_str);

    /* ---------- postconditions ---------- */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        assert(result == aws_array_eq_c_str(str->bytes, str->len, c_str));
    }

    /* ---------- unchanged fields ---------- */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_bytes);
    }

    /* ---------- validity invariants ---------- */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
