#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256U

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    char *c_str;

    /* Allocate storage for aws_string */
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_STRING_LEN];
    } storage;

    /* Non‑deterministic allocation of aws_string */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        str = &storage.s;
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Allocate buffer for C‑string */
    char c_buf[MAX_STRING_LEN + 1];

    /* Non‑deterministic allocation of C‑string */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);

        c_str = c_buf;
        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* Save old state for immutability checks */
    struct {
        struct aws_allocator *allocator;
        size_t len;
    } old_str = {0};

    struct store_byte_from_buffer old_bytes = {0};

    if (str != NULL) {
        old_str.allocator = str->allocator;
        old_str.len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    char *old_c_str = c_str;
    struct store_byte_from_buffer old_c_bytes = {0};

    if (c_str != NULL) {
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            ++c_len;
        }
        save_byte_from_array((uint8_t *)c_str, c_len, &old_c_bytes);
    }

    /* Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Compute expected result manually */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            ++c_len;
        }
        if (c_len != str->len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if ((char)str->bytes[i] != c_str[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }

    /* Assert post‑conditions */
    assert(result == expected);

    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes);
    }
    assert(c_str == old_c_str);
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((uint8_t *)c_str, &old_c_bytes);
    }

    /* Validity invariants must still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
