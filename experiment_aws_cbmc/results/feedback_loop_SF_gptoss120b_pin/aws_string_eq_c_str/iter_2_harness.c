#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    const char *c_str;

    /* nondet choice for NULL or non‑NULL string */
    bool str_is_null = nondet_bool();
    if (str_is_null) {
        str = NULL;
    } else {
        size_t max_len = nondet_uint();
        __CPROVER_assume(max_len < 256);               /* leave room for terminator */
        uint8_t buffer[sizeof(struct aws_string) + 256];
        str = (struct aws_string *)buffer;
        str->allocator = aws_default_allocator();
        str->len = max_len;
        for (size_t i = 0; i < max_len; ++i) {
            str->bytes[i] = nondet_uint8();
        }
        str->bytes[max_len] = 0;                       /* null terminator */
    }

    /* nondet choice for NULL or non‑NULL C string */
    bool c_str_is_null = nondet_bool();
    if (c_str_is_null) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_uint();
        __CPROVER_assume(c_len <= 256);
        char c_buf[257];
        for (size_t i = 0; i < c_len; ++i) {
            c_buf[i] = nondet_uint8();
        }
        c_buf[c_len] = '\0';
        c_str = c_buf;
    }

    /* Preconditions */
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* Snapshot of inputs */
    size_t orig_str_len = 0;
    const uint8_t *orig_str_bytes = NULL;
    if (str != NULL) {
        orig_str_len = str->len;
        orig_str_bytes = str->bytes;
    }
    const char *orig_c_str = c_str;

    /* Call function under verification */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Frame conditions: inputs unchanged */
    if (str != NULL) {
        assert(str->len == orig_str_len);
        assert(memcmp(str->bytes, orig_str_bytes, orig_str_len + 1) == 0);
    }
    if (c_str != NULL) {
        assert(c_str == orig_c_str);
        size_t c_len = strlen(c_str);
        assert(memcmp(c_str, orig_c_str, c_len + 1) == 0);
    }

    /* Result conditions */
    if (result) {
        assert(str != NULL);
        assert(c_str != NULL);
        assert(str->len == strlen(c_str));
        assert(memcmp(str->bytes, c_str, str->len) == 0);
    } else {
        if (str != NULL && c_str != NULL) {
            size_t c_len = strlen(c_str);
            if (str->len == c_len) {
                assert(memcmp(str->bytes, c_str, str->len) != 0);
            } else {
                assert(str->len != c_len);
            }
        } else {
            assert(str == NULL || c_str == NULL);
        }
    }
}
