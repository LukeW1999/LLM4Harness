#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_byte_buf_from_c_str_harness() {
    /* Non-deterministic input */
    const char *c_str;
    size_t str_len;

    /* Allocate and assume c_str is a valid C string */
    if (nondet_bool()) {
        c_str = NULL;
        str_len = 0;
    } else {
        str_len = nondet_size_t();
        __CPROVER_assume(str_len < MAX_BUFFER_SIZE);
        c_str = malloc(str_len + 1);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(c_str, str_len + 1));
        memset((void *)c_str, 'a', str_len); // Fill with non-null characters
        c_str[str_len] = '\0';
    }

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Postconditions */
    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    } else {
        assert(buf.len == str_len);
        assert(buf.capacity == str_len + 1); // Corrected: capacity should be str_len + 1
        assert(buf.buffer == (uint8_t *)c_str);
        assert(buf.allocator == NULL);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&buf));

    /* Free allocated memory */
    if (c_str != NULL) {
        free((void *)c_str);
    }
}
