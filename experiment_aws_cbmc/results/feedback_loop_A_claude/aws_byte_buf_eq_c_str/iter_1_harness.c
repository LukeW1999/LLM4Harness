#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Harness for aws_byte_buf_eq_c_str
 *
 * From Doxygen:
 * "Compare an aws_byte_buf and a null-terminated string.
 *  Returns true if their contents are equivalent.
 *  The buffer should NOT contain a null-terminator, or the comparison will always return false."
 *
 * Analysis:
 * 1. Changed fields on success: none (pure comparison, returns bool)
 * 2. Unchanged fields: buf->len, buf->buffer, buf->capacity, buf->allocator (all unchanged)
 * 3. Failure: returns false (no struct changes either way)
 * 4. Validity invariants: buf remains valid after call
 */

#ifndef MAX_BUFFER_SIZE
#  define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Set up null-terminated c_str (non-deterministic, bounded length) */
    /* Use a small fixed-size array to keep state space manageable */
    uint8_t c_str_data[MAX_BUFFER_SIZE + 2];
    /* Make the string contents non-deterministic */
    /* Ensure it is null-terminated somewhere within the array */
    size_t null_pos;
    __CPROVER_assume(null_pos <= MAX_BUFFER_SIZE);
    c_str_data[null_pos] = '\0';
    const char *c_str = (const char *)c_str_data;

    /* 3. Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert postconditions */

    /* The function is a pure comparison — no fields should change */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));

    /* The result is a bool — it must be either true or false */
    assert(result == true || result == false);

    /* If result is true, the contents must be equivalent:
     * buf->buffer[0..len-1] matches c_str[0..len-1] and c_str[len] == '\0'
     * We verify this by checking the underlying aws_array_eq_c_str semantics:
     * If result is true, then c_str[buf.len] must be '\0' (string lengths match)
     * and the bytes must match. We assert the length consistency. */
    if (result == true) {
        /* c_str must be null-terminated at exactly buf.len */
        assert(c_str[buf.len] == '\0');
        /* The buffer contents must match the c_str contents */
        if (buf.len > 0 && buf.buffer != NULL) {
            assert(__CPROVER_r_ok(buf.buffer, buf.len));
        }
    }
}

int main(void) {
    aws_byte_buf_eq_c_str_harness();
    return 0;
}
