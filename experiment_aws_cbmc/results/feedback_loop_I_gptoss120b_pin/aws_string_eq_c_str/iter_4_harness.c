#include <aws/common/string.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_STRING_LEN 256
#define MAX_CSTR_LEN   256

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    const char *c_str;

    /* Decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_STRING_LEN);

        /* Stack‑allocated container for aws_string with flexible array */
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } str_container;

        str = &str_container.s;

        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Decide if c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len < MAX_CSTR_LEN);

        char c_buf[MAX_CSTR_LEN];
        for (size_t i = 0; i < c_len; ++i) {
            c_buf[i] = (char)nondet_uint8_t();
        }
        c_buf[c_len] = '\0';
        c_str = c_buf;

        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* Save old state of str (if non‑NULL) */
    struct store_byte_from_buffer old_bytes;
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    /* Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Post‑condition assertions */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    }

    /* Unchanged fields (if str is non‑NULL) */
    if (str != NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes);
        assert(aws_string_is_valid(str));
    }

    /* Validity invariants */
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
