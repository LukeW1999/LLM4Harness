#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness() {
    /* Non-deterministic parameters */
    struct aws_string *str = nondet_bool() ? malloc(sizeof(struct aws_string)) : NULL;
    size_t len = nondet_size_t();

    /* Bound len to avoid excessive state space */
    __CPROVER_assume(len < MAX_BUFFER_SIZE);

    if (str) {
        /* Allocate contiguous memory for struct + bytes + null terminator */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        str = malloc(total_size);  /* re-assign to valid allocation */
        __CPROVER_assume(str != NULL);

        /* Initialize fields */
        str->allocator = aws_default_allocator();
        str->len = len;

        /* Fill bytes with nondet values and guarantee null terminator */
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        str->bytes[len] = '\0';

        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Save old state if str is non-null */
    struct aws_string old_str;
    if (str) {
        old_str.allocator = str->allocator;
        old_str.len = str->len;
        /* Cannot copy bytes array because size is unknown; we will check immutability via pointer comparison later */
    }

    /* Call function */
    const uint8_t *result = aws_string_bytes(str);

    /* Postconditions */
    if (str) {
        /* Returned pointer must equal str->bytes */
        assert(result == str->bytes);

        /* String remains valid */
        assert(aws_string_is_valid(str));

        /* All fields unchanged */
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        /* bytes pointer is the same (address of first element) */
        /* Since bytes is embedded, no separate pointer to compare */
    } else {
        /* If str is NULL, behavior is undefined; but we still assert result is NULL? */
        /* Typically, passing NULL is invalid, but we can assert that if str is NULL, the result is undefined.
           To simplify, we assume str is non-null for valid harness. */
        assert(result == NULL);  /* Consistent with typical behavior for NULL input */
    }
}
