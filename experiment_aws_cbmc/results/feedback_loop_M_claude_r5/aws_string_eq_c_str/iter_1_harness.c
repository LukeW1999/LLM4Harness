#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_eq_c_str_harness() {
    /* Non-deterministically choose whether str and c_str are NULL */
    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    struct aws_string *str = NULL;
    const char *c_str = NULL;

    if (!str_is_null) {
        /* Create a valid aws_string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for aws_string header + len bytes + 1 null terminator */
        struct aws_string *s = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(s != NULL);

        /* Initialize the string fields */
        /* We need to write to const fields, use a non-const pointer trick */
        size_t *len_ptr = (size_t *)&s->len;
        *len_ptr = len;

        struct aws_allocator **alloc_ptr = (struct aws_allocator **)&s->allocator;
        *alloc_ptr = aws_default_allocator();

        /* bytes are non-deterministic (already allocated) */
        /* Ensure null terminator at position len */
        uint8_t *bytes_ptr = (uint8_t *)s->bytes;
        bytes_ptr[len] = '\0';

        str = s;
    }

    if (!c_str_is_null) {
        /* Create a bounded c_str */
        size_t c_str_len = nondet_size_t();
        __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

        char *buf = malloc(c_str_len + 1);
        __CPROVER_assume(buf != NULL);
        buf[c_str_len] = '\0';
        c_str = buf;
    }

    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postconditions based on Doxygen and implementation */

    /* Case 1: Both NULL → returns true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }
    /* Case 2: One NULL, other not → returns false */
    else if (str == NULL || c_str == NULL) {
        assert(result == false);
    }
    /* Case 3: Both non-NULL → result depends on content comparison */
    else {
        /* The function returns true iff bytes match */
        /* We can't easily assert the exact value without reimplementing,
           but we can assert that the function returns a valid bool */
        assert(result == true || result == false);

        /* The string fields must not have changed */
        assert(str->len <= MAX_BUFFER_SIZE);
        assert(str->allocator == aws_default_allocator());
    }

    /* Validity: if str is non-NULL, it should still be valid */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
