#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_c_str_harness() {
    /* Non-deterministically choose buffer length, capacity, and string length */
    size_t buf_len;
    size_t buf_cap;
    size_t c_str_len;
    __CPROVER_assume(buf_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf_cap >= buf_len);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* Initialize the byte buffer */
    struct aws_byte_buf buf;
    buf.len = buf_len;
    buf.capacity = buf_cap;
    buf.allocator = NULL; /* not used in the function, safe to set */
    if (buf_len > 0) {
        buf.buffer = (uint8_t *)malloc(buf_cap);
        __CPROVER_assume(buf.buffer != NULL);
        for (size_t i = 0; i < buf_len; i++) {
            buf.buffer[i] = nondet_uint8();
        }
    } else {
        buf.buffer = NULL;
    }

    /* Initialize the null-terminated string */
    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; i++) {
        /* Ensure no embedded nulls (valid C-string) */
        __CPROVER_assume(c_str[i] != 0);
        c_str[i] = nondet_char();
    }
    c_str[c_str_len] = '\0';

    /* Save old state for immutability checks */
    struct aws_byte_buf old = buf;
    struct store_byte_from_buffer saved;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &saved);
    }

    /* Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Assert that buf is unchanged */
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);
    if (buf.buffer != NULL && buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &saved);
    }

    /* Assert validity invariant */
    assert(aws_byte_buf_is_valid(&buf));

    /* Assert correctness of the result:
     * result == (buf.len == c_str_len && all bytes from 0 to buf.len-1 match) */
    bool expected = (buf.len == c_str_len);
    if (expected) {
        for (size_t i = 0; i < buf.len; i++) {
            if (buf.buffer[i] != (uint8_t)c_str[i]) {
                expected = false;
                break;
            }
        }
    }
    assert(result == expected);
}
