#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Harness for aws_byte_buf_eq_c_str
 *
 * The function compares an aws_byte_buf and a null-terminated string.
 * Returns true if their contents are equivalent.
 * The buffer should NOT contain a null-terminator, or the comparison will always return false.
 *
 * Analysis:
 * 1. Changed fields on success: none (pure comparison, returns bool)
 * 2. Unchanged fields: all fields of buf (len, buffer, capacity, allocator)
 * 3. Failure: returns false (no error code, just bool)
 * 4. Validity invariants: buf must remain valid after the call
 */
void aws_byte_buf_eq_c_str_harness() {
    /* 1. Declare and set up the aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Declare a non-deterministic null-terminated C string */
    /* We use a bounded array to keep the state space manageable */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator is present */
    c_str[c_str_len] = '\0';

    /* 3. Save old state before calling */
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert postconditions */

    /* The function is a pure comparison - no fields should change */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 6. Assert validity invariant still holds */
    assert(aws_byte_buf_is_valid(&buf));

    /* 7. Result is a bool - either true or false */
    /* If result is true, the contents must be equivalent */
    /* If result is false, they are not equivalent */
    /* Both paths are valid - we just assert the result is a valid bool */
    assert(result == true || result == false);
}
