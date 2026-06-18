#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_string_eq_c_str_harness() {
    /* Case 1: both NULL */
    /* Case 2: str NULL, c_str non-NULL */
    /* Case 3: str non-NULL, c_str NULL */
    /* Case 4: both non-NULL */

    /* Non-deterministically choose which case to test */
    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    const struct aws_string *str = NULL;
    const char *c_str = NULL;

    if (!str_is_null) {
        /* Allocate a valid aws_string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for aws_string header + len bytes + 1 null terminator */
        struct aws_string *s = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(s != NULL);

        /* Initialize the string fields */
        /* We need to write to const fields - use a writable copy approach */
        /* Cast away const for initialization purposes */
        *(size_t *)&s->len = len;
        *(struct aws_allocator **)&s->allocator = NULL;

        /* The bytes are non-deterministic (already non-det from malloc) */
        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(s));

        str = s;
    }

    if (!c_str_is_null) {
        /* Allocate a bounded c-string */
        size_t c_str_len = nondet_size_t();
        __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

        char *cs = malloc(c_str_len + 1);
        __CPROVER_assume(cs != NULL);
        cs[c_str_len] = '\0';

        c_str = cs;
    }

    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postconditions based on Doxygen and implementation */

    /* Case: both NULL -> must return true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    /* Case: exactly one is NULL -> must return false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    /* Case: both non-NULL -> result depends on content comparison */
    /* We can't easily assert the exact value without reimplementing,
       but we can assert the function returns a valid bool */
    if (str != NULL && c_str != NULL) {
        assert(result == true || result == false);
    }

    /* The function is a pure query - it should not modify the string */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
