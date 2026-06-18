#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/ring_buffer.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Bounding constant for the length of the string data */
#define MAX_STRING_LEN 256U

void aws_string_eq_c_str_harness(void) {
    /* -------------------------------------------------------------
     * 1. Non‑deterministic inputs
     * ------------------------------------------------------------- */
    struct aws_string *str;
    char *c_str;

    /* Decide whether the string pointer is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* Allocate a non‑NULL aws_string with a bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Allocate space for the struct plus (len-1) extra bytes for the flexible array */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* Initialise fields */
        str->allocator = aws_default_allocator();
        str->len = len;

        /* Fill the byte array with nondeterministic data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* Assume the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Decide whether the C‑string pointer is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* Allocate a C‑string with a bounded length (including the terminating NUL) */
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);

        c_str = malloc(c_len + 1U);
        __CPROVER_assume(c_str != NULL);

        /* Fill with nondeterministic bytes and ensure NUL termination */
        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';

        /* Assume the C‑string satisfies its validity predicate */
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* -------------------------------------------------------------
     * 2. Save old state for immutability checks
     * ------------------------------------------------------------- */
    struct aws_string old_str;
    struct store_byte_from_buffer old_bytes = {0};

    if (str != NULL) {
        old_str = *str;                     /* shallow copy of the struct header */
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    char *old_c_str = c_str;               /* pointer value */
    struct store_byte_from_buffer old_c_bytes = {0};
    if (c_str != NULL) {
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            ++c_len;
        }
        save_byte_from_array((uint8_t *)c_str, c_len, &old_c_bytes);
    }

    /* -------------------------------------------------------------
     * 3. Call function under test
     * ------------------------------------------------------------- */
    bool result = aws_string_eq_c_str(str, c_str);

    /* -------------------------------------------------------------
     * 4. Compute the expected result using the specification
     * ------------------------------------------------------------- */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
    }

    /* -------------------------------------------------------------
     * 5. Assert post‑conditions
     * ------------------------------------------------------------- */
    assert(result == expected);

    /* No fields are modified by the function – all must remain unchanged */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes);
    }
    assert(c_str == old_c_str);
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((uint8_t *)c_str, &old_c_bytes);
    }

    /* -------------------------------------------------------------
     * 6. Validity invariants must still hold
     * ------------------------------------------------------------- */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }

    /* -------------------------------------------------------------
     * 7. Clean up
     * ------------------------------------------------------------- */
    free(str);
    free(c_str);
}
