#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 256

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

        /* stack‑allocated buffer for struct + flexible array */
        uint8_t str_buf[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)str_buf;

        /* set fields */
        str->allocator = aws_default_allocator();
        str->len = len;

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

        static char c_buf[MAX_BUFFER_SIZE];
        c_str = c_buf;

        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';
    }

    /* 2. Save old state */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes = {0};
    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    struct store_byte_from_buffer old_c_bytes = {0};
    size_t c_len_saved = 0;
    if (c_str != NULL) {
        for (c_len_saved = 0; c_len_saved < MAX_BUFFER_SIZE; ++c_len_saved) {
            if (c_str[c_len_saved] == '\0') break;
        }
        save_byte_from_array((const uint8_t *)c_str, c_len_saved, &old_c_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Postconditions */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        /* no additional value‑based assertion required */
    }

    /* 5. Unchanged fields */
    if (str != NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes);
    }
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_bytes);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
