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

void aws_string_eq_c_str_harness(void) {
    /* 1. nondet inputs */
    struct aws_string *str;
    bool str_is_null = nondet_bool();
    if (str_is_null) {
        str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate struct + flexible array */
        str = malloc(sizeof(struct aws_string) + (len == 0 ? 0 : len - 1));
        __CPROVER_assume(str != NULL);

        /* set fields (const, so cast away constness) */
        *((struct aws_allocator * const *)&str->allocator) = aws_default_allocator();
        *((size_t const *)&str->len) = len;

        /* nondet fill bytes */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* validity */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    char *c_str;
    bool c_is_null = nondet_bool();
    if (c_is_null) {
        c_str = NULL;
    } else {
        size_t max_c_len = MAX_BUFFER_SIZE;
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len < max_c_len);          /* leave room for terminating NUL */

        c_str = malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);

        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';
    }

    /* 2. Save old state */
    struct aws_string old_str = {0};
    struct store_byte_from_buffer old_bytes = {0};
    if (str != NULL) {
        old_str = *str;                                 /* copy header fields */
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    struct store_byte_from_buffer old_c_bytes = {0};
    size_t c_len_saved = 0;
    if (c_str != NULL) {
        /* compute length up to NUL (bounded by MAX_BUFFER_SIZE) */
        for (c_len_saved = 0; c_len_saved < MAX_BUFFER_SIZE; ++c_len_saved) {
            if (c_str[c_len_saved] == '\0') break;
        }
        save_byte_from_array((const uint8_t *)c_str, c_len_saved, &old_c_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Postconditions for both success and failure paths */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        /* when both non‑NULL, the function must not modify either argument */
        /* no additional value‑based assertion (equality) is required here */
    }

    /* 5. Unchanged fields */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes);
    }
    if (c_str != NULL) {
        /* pointer itself must stay the same */
        assert(c_str == c_str);
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_bytes);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
