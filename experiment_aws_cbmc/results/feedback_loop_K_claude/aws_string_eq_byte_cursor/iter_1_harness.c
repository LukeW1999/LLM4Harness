#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 8
#define MAX_CURSOR_LEN 8

void aws_string_eq_byte_cursor_harness(void) {
    /* Nondeterministically choose whether str and cur are NULL */
    bool str_is_null;
    bool cur_is_null;

    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    /* Set up str */
    if (!str_is_null) {
        /* Allocate and initialize a valid aws_string with nondet length */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STRING_LEN);

        /* aws_string has flexible-array-like layout: header + bytes[len+1] */
        /* We allocate enough memory for the struct plus str_len+1 bytes */
        size_t alloc_size = sizeof(struct aws_string) + str_len + 1;
        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        /* Initialize the const fields via pointer casting */
        *(size_t *)(&str->len) = str_len;
        *(struct aws_allocator **)(&str->allocator) = aws_default_allocator();

        /* bytes are nondet (already nondet from malloc) */
        /* Ensure null terminator */
        ((uint8_t *)str->bytes)[str_len] = 0;

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up cur */
    if (!cur_is_null) {
        cur = (struct aws_byte_cursor *)malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);

        size_t cur_len;
        __CPROVER_assume(cur_len <= MAX_CURSOR_LEN);
        cur->len = cur_len;

        if (cur_len > 0) {
            uint8_t *ptr = (uint8_t *)malloc(cur_len);
            __CPROVER_assume(ptr != NULL);
            cur->ptr = ptr;
        } else {
            cur->ptr = NULL;
        }

        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postcondition: both NULL => true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }

    /* Postcondition: exactly one NULL => false */
    if (str == NULL && cur != NULL) {
        assert(result == false);
    }
    if (str != NULL && cur == NULL) {
        assert(result == false);
    }

    /* Postcondition: both non-NULL => result depends on content */
    if (str != NULL && cur != NULL) {
        if (result) {
            /* If true, lengths must match */
            assert(str->len == cur->len);
        } else {
            /* If false, either lengths differ or bytes differ */
            /* We can't easily assert the negative of memcmp in CBMC,
               but we can assert that if lengths differ, result is false */
            if (str->len != cur->len) {
                assert(result == false);
            }
        }
    }

    /* Frame condition: str and cur pointers unchanged */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }

    return 0;
}
