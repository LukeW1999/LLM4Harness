#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 64

void aws_byte_buf_from_c_str_harness() {
    /* Nondeterministic inputs */
    const char *c_str;
    size_t len;

    /* If c_str is not NULL, assume it points to a valid null-terminated string of bounded length */
    bool c_str_is_null = nondet_bool();
    if (c_str_is_null) {
        c_str = NULL;
        len = 0;
    } else {
        /* Allocate nondet string of bounded length + 1 for null terminator */
        c_str = (const char *)malloc(MAX_STRING_LEN + 1);
        __CPROVER_assume(c_str != NULL);
        /* Pick a length up to MAX_STRING_LEN */
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        /* Assume the string is null-terminated at position len */
        __CPROVER_assume(c_str[len] == '\0');
        /* Assume all characters before len are nondet but readable */
        for (size_t i = 0; i < len; i++) {
            // not needed to assume anything; bytes are unconstrained
        }
        /* Make sure memory is readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, len + 1));
    }

    /* Call the function */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Postconditions */

    /* 1. allocator is always NULL */
    assert(buf.allocator == NULL);

    /* 2. len and capacity */
    if (c_str == NULL) {
        assert(buf.len == 0);
    } else {
        /* buf.len should equal the length of the string (determined by strlen) */
        /* Since we assumed the string length is len, we can assert that */
        assert(buf.len == len);
    }
    assert(buf.capacity == buf.len);

    /* 3. buffer pointer */
    if (buf.capacity == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == (uint8_t *)c_str);
        /* The contents of buffer match the string */
        /* Since buffer points to the same memory, we can assert byte equality */
        for (size_t i = 0; i < buf.len; i++) {
            assert(buf.buffer[i] == (uint8_t)c_str[i]);
        }
    }

    /* 4. Validity of the returned buffer */
    assert(aws_byte_buf_is_valid(&buf));

    /* 5. The input string is not modified (it is const) */
    /* Not required to assert, but we can assert that the memory at c_str is unchanged */
    /* We could check that the first len bytes are unchanged, but the function does not modify them */
    /* This is implicit in the const qualifier */
}
