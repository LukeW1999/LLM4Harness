#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_STRING_LEN 256

void aws_string_eq_c_str_harness(void) {
    /* 1. Nondeterministic inputs */
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;
    struct aws_allocator *allocator = aws_default_allocator();

    struct {
        struct aws_string s;
        uint8_t bytes[MAX_STRING_LEN];
    } str_storage;

    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        str = &str_storage.s;
        str->allocator = allocator;
        str->len = len;
        /* bytes are part of the allocation; no need to initialize */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    bool c_str_is_null = nondet_bool();
    char c_str_buf[MAX_STRING_LEN + 1];
    char *c_str = NULL;
    size_t c_len = 0;

    if (!c_str_is_null) {
        c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);
        c_str = c_str_buf;
        c_str[c_len] = '\0';
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str;                     /* copy allocator and len */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct store_byte_from_buffer old_c_str_bytes;
    if (c_str != NULL) {
        save_byte_from_array((uint8_t *)c_str, c_len + 1, &old_c_str_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Postcondition assertions */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        /* both non‑NULL */
        assert(result == aws_array_eq_c_str(str->bytes, str->len, c_str));
    }

    /* 5. Unchanged fields / immutability */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((uint8_t *)c_str, &old_c_str_bytes);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
