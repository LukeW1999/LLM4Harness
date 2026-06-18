#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_from_c_str_harness() {
    /* Non-deterministic string length, bounded */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

    /* Allocate and initialize a null-terminated string */
    const char *c_str = (const char *)malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < str_len; i++) {
        ((uint8_t *)c_str)[i] = nondet_uint8_t();
        __CPROVER_assume(((uint8_t *)c_str)[i] != 0);
    }
    ((char *)c_str)[str_len] = '\0';

    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* Postconditions */
    assert(result.allocator == NULL);
    assert(result.len == str_len);
    assert(result.capacity == str_len);
    if (str_len > 0) {
        assert(AWS_MEM_IS_READABLE(c_str, str_len));
    }
    assert(aws_byte_buf_is_valid(&result));
}
