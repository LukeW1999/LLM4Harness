#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_byte_buf_from_c_str_harness() {
    const char *c_str;
    size_t str_len;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
        str_len = 0; /* not used */
    } else {
        /* Bound the length of the string (excluding null terminator) */
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        /* Allocate memory for the string */
        c_str = (const char *)malloc(sizeof(char) * (str_len + 1));
        __CPROVER_assume(c_str != NULL);
        /* Fill with non-null nondet bytes except the null terminator */
        for (size_t i = 0; i < str_len; i++) {
            ((char *)c_str)[i] = (char)nondet_uint8_t();
            __CPROVER_assume(((char *)c_str)[i] != '\0');
        }
        ((char *)c_str)[str_len] = '\0';
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, str_len + 1));
    }

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Postconditions */
    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    } else {
        if (str_len == 0) {
            assert(buf.len == 0);
            assert(buf.capacity == 0);
            assert(buf.buffer == NULL);
            assert(buf.allocator == NULL);
        } else {
            assert(buf.len == str_len);
            assert(buf.capacity == str_len);
            assert(buf.buffer == (uint8_t *)c_str);
            assert(buf.allocator == NULL);
        }
    }

    /* Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));

    /* Unchanged fields: nothing to assert since we created a new struct */
}
