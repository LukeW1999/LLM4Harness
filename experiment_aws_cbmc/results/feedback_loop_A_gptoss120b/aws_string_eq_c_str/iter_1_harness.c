#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_c_str_harness(void) {
    /* 1. Declare inputs (may be NULL) */
    struct aws_string *str;
    const char *c_str;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with bounded length */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate memory for struct + flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* assume the allocated string satisfies the library invariant */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministically decide if c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* allocate a C string with bounded length and a terminating NUL */
        size_t max_c_len = MAX_BUFFER_SIZE;
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len < max_c_len);               /* leave room for NUL */

        char *buf = malloc(c_len + 1);
        __CPROVER_assume(buf != NULL);
        buf[c_len] = '\0';                                 /* ensure NUL termination */
        c_str = buf;

        /* assume the C string satisfies the library invariant */
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* 2. Save old state of str (if non‑NULL) for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_bytes;
    if (str != NULL) {
        old_str = *str;                                    /* shallow copy of fields */
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Post‑condition: result matches the specification */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
        assert(result == expected);
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes);
    }

    /* 6. Validity invariants must still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
