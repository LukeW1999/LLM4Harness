#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_eq_c_str_harness(void) {
    /* 1. Nondeterministic inputs */
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;

    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate enough memory for the struct plus the flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* initialize const fields (cast away const for harness) */
        ((struct aws_string *)str)->allocator = aws_default_allocator();
        ((struct aws_string *)str)->len = len;

        /* assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    bool c_is_null = nondet_bool();
    char *c_str = NULL;
    size_t c_len = 0;

    if (!c_is_null) {
        c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);

        c_str = malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* nondet contents, ensure null‑termination */
        c_str[c_len] = '\0';

        /* assume the C string is valid */
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
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
        assert(result == expected);
    }

    /* 5. Unchanged fields / immutability */
    if (str) {
        assert(str->allocator == aws_default_allocator());
        assert(str->len == ((struct aws_string *)str)->len);
        assert(str->bytes == ((struct aws_string *)str)->bytes);
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
