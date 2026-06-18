#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_eq_c_str_harness(void) {
    /* nondeterministic choice for str being NULL or non‑NULL */
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;

    if (!str_is_null) {
        /* length of the string, bounded */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* allocate raw bytes for the string payload */
        uint8_t *raw_bytes = malloc(str_len);
        __CPROVER_assume(raw_bytes != NULL);

        /* create a valid aws_string from the raw bytes */
        str = aws_string_new_from_array(aws_default_allocator(), raw_bytes, str_len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministic choice for c_str being NULL or non‑NULL */
    bool c_is_null = nondet_bool();
    const char *c_str = NULL;
    size_t c_len = 0;               /* length without the terminating NUL */
    if (!c_is_null) {
        c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);

        char *tmp = malloc(c_len + 1);
        __CPROVER_assume(tmp != NULL);
        tmp[c_len] = '\0';          /* ensure NUL termination */
        c_str = tmp;
    }

    /* Save old immutable state */
    struct aws_string *old_str = str;
    struct store_byte_from_buffer old_str_bytes = {0};
    if (str != NULL) {
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct store_byte_from_buffer old_c_str_bytes = {0};
    if (c_str != NULL) {
        save_byte_from_array((const uint8_t *)c_str, c_len + 1, &old_c_str_bytes);
    }

    /* Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Post‑condition on return value */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
    }
    assert(result == expected);

    /* Unchanged fields for str (if non‑NULL) */
    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    /* Unchanged bytes for c_str (if non‑NULL) */
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_bytes);
    }

    /* Validity invariants */
    assert(str == NULL || aws_string_is_valid(str));
}
