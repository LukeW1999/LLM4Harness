#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#define MAX_STRING_LEN 256

void aws_string_eq_c_str_harness(void) {
    /* 1. Nondeterministic inputs */
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;

    /* Stack storage for aws_string and its bytes */
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_STRING_LEN];
    } str_storage;

    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        str = &str_storage.s;
        str->allocator = aws_default_allocator();
        str->len = len;

        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    bool c_is_null = nondet_bool();
    char *c_str = NULL;
    size_t c_len = 0;

    /* Stack storage for C string */
    char c_str_buf[MAX_STRING_LEN + 1];

    if (!c_is_null) {
        c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);

        c_str = c_str_buf;
        c_str[c_len] = '\0';

        /* Assume the C string is valid */
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* 2. Save old state for immutability checks */
    struct store_byte_from_buffer old_str_byte;
    if (str) {
        save_byte_from_array(str->bytes, str->len, &old_str_byte);
    }

    struct store_byte_from_buffer old_c_str_byte;
    if (c_str) {
        save_byte_from_array((uint8_t *)c_str, c_len + 1, &old_c_str_byte);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Post‑condition assertions */
    bool expected = false;
    if (str && c_str) {
        size_t c_actual_len = strlen(c_str);
        if (str->len == c_actual_len) {
            expected = (memcmp(str->bytes, c_str, str->len) == 0);
        }
    } else if (!str && !c_str) {
        expected = true;
    }
    assert(result == expected);

    /* 5. Unchanged fields / immutability */
    if (str) {
        assert(str->allocator == aws_default_allocator());
        assert_byte_from_buffer_matches(str->bytes, &old_str_byte);
    }

    if (c_str) {
        assert_byte_from_buffer_matches((uint8_t *)c_str, &old_c_str_byte);
    }

    /* 6. Validity invariants */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (c_str) {
        assert(aws_c_string_is_valid(c_str));
    }
}
