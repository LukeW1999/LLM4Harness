#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

/* The harness for aws_string_eq_c_str */
void aws_string_eq_c_str_harness(void) {
    /* -------------------------------------------------------------
     * 1. Non‑deterministic inputs (may be NULL or a valid object)
     * ------------------------------------------------------------- */
    struct aws_string *str;
    char *c_str;

    /* decide whether each pointer is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a possibly‑valid aws_string */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* allocate space for the struct + flexible array member */
        str = malloc(sizeof(struct aws_string) + (str_len > 0 ? str_len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* initialise the const fields via a compound literal */
        *str = (struct aws_string){
            .allocator = aws_default_allocator(),
            .len = str_len
        };

        /* fill the byte payload with nondet data */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* allocate a possibly‑valid C‑string */
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);

        c_str = malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);

        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';

        /* assume the C‑string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* -------------------------------------------------------------
     * 2. Save old state for immutability checks
     * ------------------------------------------------------------- */
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

    size_t old_c_len = 0;
    char *old_c_str = NULL;
    if (c_str != NULL) {
        old_c_len = strlen(c_str);               /* bounded by the allocation above */
        old_c_str = malloc(old_c_len + 1);
        __CPROVER_assume(old_c_str != NULL);
        memcpy(old_c_str, c_str, old_c_len + 1);
    }

    /* -------------------------------------------------------------
     * 3. Call the function under test
     * ------------------------------------------------------------- */
    bool result = aws_string_eq_c_str(str, c_str);

    /* -------------------------------------------------------------
     * 4. Post‑condition: the function is pure – no observable state change
     * ------------------------------------------------------------- */
    if (str != NULL) {
        /* length and allocator must be unchanged */
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        /* payload bytes must be unchanged */
        if (old_str_len > 0) {
            assert(memcmp(str->bytes, old_str_bytes, old_str_len) == 0);
        }
        /* validity invariant must still hold */
        assert(aws_string_is_valid(str));
    }

    if (c_str != NULL) {
        /* C‑string content must be unchanged */
        assert(strcmp(c_str, old_c_str) == 0);
        /* validity invariant must still hold */
        assert(aws_c_string_is_valid(c_str));
    }

    /* -------------------------------------------------------------
     * 5. Logical relationship between the return value and the inputs
     * ------------------------------------------------------------- */
    if (result) {
        /* Both arguments must be non‑NULL */
        assert(str != NULL);
        assert(c_str != NULL);

        /* Their contents must be identical */
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            ++c_len;
        }
        assert(c_len == str->len);
        for (size_t i = 0; i < str->len; ++i) {
            assert((uint8_t)c_str[i] == str->bytes[i]);
        }
    } else {
        /* If exactly one argument is NULL, result must be false */
        if ((str == NULL) != (c_str == NULL)) {
            assert(true); /* already covered – nothing extra to assert */
        } else {
            /* Both non‑NULL but contents differ */
            assert(str != NULL && c_str != NULL);
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

    /* -------------------------------------------------------------
     * 6. Clean‑up
     * ------------------------------------------------------------- */
    free(old_str_bytes);
    free(old_c_str);
    free(str);
    free(c_str);
}
