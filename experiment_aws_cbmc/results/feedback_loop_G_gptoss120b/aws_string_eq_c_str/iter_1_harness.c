#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_c_str_harness(void) {
    /* 1. Declare nondeterministic inputs */
    struct aws_string *str = NULL;
    char *c_str = NULL;

    /* Allocate and initialize a possibly non‑NULL aws_string */
    if (nondet_bool()) {
        /* Allocate minimal struct (flexible array member has size 1) */
        str = malloc(sizeof(struct aws_string));
        __CPROVER_assume(str != NULL);

        /* allocator can be NULL (static string) or nondet */
        str->allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(1);
        /* len is nondet but bounded to 0 to avoid out‑of‑bounds on the
         * flexible array member (we only need a valid string for the
         * validity predicate). */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len == 0);

        /* Initialise the single byte in the flexible array (null terminator) */
        str->bytes[0] = 0;

        /* Assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Allocate and initialise a possibly non‑NULL C‑string */
    if (nondet_bool()) {
        size_t c_len = nondet_size_t();
        /* bound the length to a small value to keep the model finite */
        __CPROVER_assume(c_len <= 8);
        c_str = malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);
        /* Fill with nondet bytes and ensure null termination */
        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = nondet_uint8_t();
        }
        c_str[c_len] = '\0';
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* 2. Save old state of the string (if it exists) */
    struct aws_string old_str;
    if (str != NULL) {
        old_str = *str;
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Post‑condition: result must match the specification */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
    }
    assert(result == expected);

    /* 5. Unchanged fields (if str is non‑NULL) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        /* The flexible array member has at least one byte; check it unchanged */
        assert(str->bytes[0] == old_str.bytes[0]);
    }

    /* 6. Validity invariants must still hold */
    assert(str == NULL || aws_string_is_valid(str));
    assert(c_str == NULL || aws_c_string_is_valid(c_str));
}
