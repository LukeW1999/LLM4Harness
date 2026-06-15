#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 256

/* nondeterministic helpers provided by CBMC */
extern bool nondet_bool(void);
extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    const char *c_str;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        str->allocator = NULL;
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministically decide if c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t max_c_len = MAX_BUFFER_SIZE;
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len < max_c_len); /* leave room for NUL */

        char *buf = malloc(c_len + 1);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < c_len; ++i) {
            buf[i] = nondet_uint8_t();
        }
        buf[c_len] = '\0';
        c_str = buf;

        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* Save old state of str for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_bytes;
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    /* Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Post‑condition checks */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
        assert(result == expected);
    }

    /* Immutability checks */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes);
    }

    /* Validity invariants must still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
