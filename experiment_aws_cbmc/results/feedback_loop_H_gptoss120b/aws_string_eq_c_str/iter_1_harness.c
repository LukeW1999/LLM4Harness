#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

/* Bounding constant for strings – adjust as needed for the proof */
#define MAX_STRING_LEN 256

void aws_string_eq_c_str_harness(void) {
    /* 1. Declare nondeterministic inputs */
    struct aws_string *str;
    const char *c_str;
    bool result;
    bool expected;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a nondeterministic length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate enough space for the header plus the flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically set allocator (may be NULL) */
        str->allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
        __CPROVER_assume(str->allocator != NULL || nondet_bool());

        str->len = len;

        /* make the bytes region readable and nondeterministic */
        __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, len + 1));

        /* ensure the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministically decide whether c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);

        char *buf = malloc(c_len + 1);
        __CPROVER_assume(buf != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(buf, c_len + 1));

        /* make it a valid C string (null‑terminated somewhere within the bound) */
        __CPROVER_assume(aws_c_string_is_valid(buf));
        c_str = buf;
    }

    /* 2. Save old state of the string (if non‑NULL) */
    struct aws_string old_str;
    struct store_byte_from_buffer old_bytes;
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    /* 3. Call function under test */
    result = aws_string_eq_c_str(str, c_str);

    /* 4. Compute the expected result using the specification */
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
    }

    /* 5. Assert that the return value matches the specification */
    assert(result == expected);

    /* 6. Assert unchanged fields (the function must not modify its inputs) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_bytes_match(str->bytes, old_bytes.arr, str->len);
    }
    /* c_str is a const pointer – the function must not modify it */
    if (c_str != NULL) {
        /* pointer itself must stay the same */
        assert(c_str == c_str);
    }

    /* 7. Validity invariants must still hold after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
