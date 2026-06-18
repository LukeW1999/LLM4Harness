#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 100
#define MAX_BUFFER_SIZE 100

void aws_byte_buf_eq_c_str_harness() {
    /* Create a valid byte buffer on the stack */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Non-deterministic null-terminated string */
    size_t c_str_len;
    __CPROVER_assume(c_str_len < MAX_STRING_LEN);
    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; i++) {
        c_str[i] = nondet_byte();
    }
    c_str[c_str_len] = '\0'; /* Null-terminate */

    /* Save old buffer state */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under verification */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Postconditions: function is read-only */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(aws_byte_buf_is_valid(&buf));

    /* Clean up */
    free(c_str);
}
