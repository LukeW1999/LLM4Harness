#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

/* Harness for aws_string_eq_c_str */
void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    char *c_str;

    /* nondet choice for str */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* allocate struct + flexible array */
        str = malloc(sizeof(struct aws_string) + (str_len > 0 ? str_len - 1 : 0));
        __CPROVER_assume(str != NULL);

        *str = (struct aws_string){
            .allocator = aws_default_allocator(),
            .len = str_len
        };

        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondet choice for c_str */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);

        c_str = malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);

        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';

        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* Save old state of str */
    size_t old_str_len = 0;
    uint8_t *old_str_bytes = NULL;
    struct aws_allocator *old_str_allocator = NULL;
    if (str != NULL) {
        old_str_len = str->len;
        old_str_allocator = str->allocator;
        if (old_str_len > 0) {
            old_str_bytes = malloc(old_str_len);
            __CPROVER_assume(old_str_bytes != NULL);
            memcpy(old_str_bytes, str->bytes, old_str_len);
        }
    }

    /* Save old state of c_str */
    size_t old_c_len = 0;
    char *old_c_str = NULL;
    if (c_str != NULL) {
        old_c_len = strlen(c_str);
        old_c_str = malloc(old_c_len + 1);
        __CPROVER_assume(old_c_str != NULL);
        memcpy(old_c_str, c_str, old_c_len + 1);
    }

    /* Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* If either argument is NULL, the result must be false */
    __CPROVER_assume(!(result && (str == NULL || c_str == NULL)));

    /* Post‑condition: no observable state change */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        if (old_str_len > 0) {
            assert(memcmp(str->bytes, old_str_bytes, old_str_len) == 0);
        }
        assert(aws_string_is_valid(str));
    }

    if (c_str != NULL) {
        assert(strcmp(c_str, old_c_str) == 0);
        assert(aws_c_string_is_valid(c_str));
    }

    /* Logical relationship between return value and inputs */
    if (result) {
        /* Both arguments are non‑NULL */
        assert(str != NULL);
        assert(c_str != NULL);

        /* Their contents are identical */
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            ++c_len;
        }
        assert(c_len == str->len);
        for (size_t i = 0; i < str->len; ++i) {
            assert((uint8_t)c_str[i] == str->bytes[i]);
        }
    } else {
        if (str != NULL && c_str != NULL) {
            /* Contents differ */
            bool equal = true;
            size_t c_len = 0;
            while (c_str[c_len] != '\0') {
                if (c_len >= str->len) { equal = false; break; }
                if ((uint8_t)c_str[c_len] != str->bytes[c_len]) { equal = false; break; }
                ++c_len;
            }
            if (c_len != str->len) { equal = false; }
            assert(!equal);
        }
    }

    /* Clean‑up */
    free(old_str_bytes);
    free(old_c_str);
    free(str);
    free(c_str);
}
