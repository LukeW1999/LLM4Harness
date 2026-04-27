#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Set up c_str — non-deterministic null-terminated string */
    /* We use a small fixed-size array to bound the state space */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    /* 3. Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert postconditions */

    /* The function is read-only — it never modifies buf */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));

    /* The return value is a bool — check both paths */
    if (result) {
        /* On true: buf->buffer[0..len-1] equals c_str[0..len-1] and
           c_str[len] == '\0' (i.e., lengths match and content matches).
           We can assert that the c_str length equals buf.len */
        assert(strlen(c_str) == buf.len);
    } else {
        /* On false: either lengths differ or content differs.
           The buf is still unchanged. */
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.buffer == old_buf.buffer);
        assert(buf.allocator == old_buf.allocator);
    }
}

int main(void) {
    aws_byte_buf_eq_c_str_harness();
    return 0;
}
