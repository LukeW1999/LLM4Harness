#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_from_c_str_harness() {
    /* Non-deterministic choice: c_str can be NULL or point to a null-terminated string */
    bool is_null = nondet_bool();
    const char *c_str;
    size_t str_len = 0;

    if (is_null) {
        c_str = NULL;
    } else {
        /* Bound the length of the string */
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* Allocate and initialize the string (including null terminator) */
        c_str = (const char *)malloc(str_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* Fill the string with non-deterministic bytes, ensuring no embedded nulls */
        for (size_t i = 0; i < str_len; i++) {
            ((uint8_t *)c_str)[i] = nondet_uint8_t();
            __CPROVER_assume(((uint8_t *)c_str)[i] != 0);
        }
        /* Ensure null terminator */
        ((char *)c_str)[str_len] = '\0';
    }

    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* Postconditions */
    assert(result.allocator == NULL);

    if (is_null) {
        /* If c_str is NULL, the buffer should have zero length, capacity, and NULL buffer */
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        /* If c_str is non-NULL, the buffer should reflect the string */
        assert(result.len == str_len);
        assert(result.capacity == str_len);
        assert(result.buffer == (uint8_t *)c_str);
        /* The buffer must be readable for its length */
        if (str_len > 0) {
            assert(AWS_MEM_IS_READABLE(c_str, str_len));
        }
    }

    /* The returned buffer must satisfy the validity predicate */
    assert(aws_byte_buf_is_valid(&result));
}
