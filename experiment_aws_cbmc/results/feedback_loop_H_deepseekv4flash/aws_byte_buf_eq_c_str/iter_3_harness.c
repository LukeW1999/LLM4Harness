#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_c_str_harness() {
    /* Non-deterministic data structures */
    struct aws_byte_buf buf;
    const char *c_str;

    /* Bound and initialize buf */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Build a non-deterministic null-terminated string */
    size_t c_str_len;
    __CPROVER_assume(c_str_len < MAX_STRING_SIZE);
    char *c_str_mutable = malloc(c_str_len + 1);
    __CPROVER_assume(c_str_mutable != NULL);

    /* Fill the string with non-deterministic bytes; ensure null-termination */
    for (size_t i = 0; i < c_str_len; i++) {
        c_str_mutable[i] = nondet_char();
    }
    c_str_mutable[c_str_len] = '\0';
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str_mutable, c_str_len + 1));
    c_str = (const char *)c_str_mutable;

    /* Save old state of buf */
    struct aws_byte_buf old = buf;

    /* Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Postconditions: buf is unchanged */
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
    assert(buf.buffer == old.buffer);

    /* Validity invariant holds */
    assert(aws_byte_buf_is_valid(&buf));

    /* The result is a boolean */
    assert(result == true || result == false);

    /* Clean up */
    free(c_str_mutable);
}
